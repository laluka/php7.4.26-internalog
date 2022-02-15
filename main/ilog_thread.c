#include "ilog_thread.h"

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "lfqueue.h"
#include "ilog_config.h"
#include "ilog_udpclient.h"

#define THREAD_WAIT_MS 100

static pthread_t THREAD = 0;
static atomic_bool SHOULD_TERMINATE = false;
static lfqueue_t* QUEUE = NULL;


// Declarations
void* routine(void*);


// Definitions
void ilog_thread_init() {
  if (! ilog_is_enabled()) { return; }

  if (THREAD != 0) {
    perror("Trying to start the ilog thread a second time, abort\n");
    exit(1);
  } else if (QUEUE != NULL) {
    perror("Trying to create the msg queue a second time, abort\n");
    exit(1);
  }

  // Init the queue before starting the thread
  QUEUE = malloc(sizeof(lfqueue_t));

  if (lfqueue_init(QUEUE) != 0) {
    perror("Failed to start the msg queue, abort\n");
    exit(1);
  }

  // Init the thread
  int ret = pthread_create(&THREAD, NULL, routine, NULL);

  if (ret != 0) {
    switch(ret) {
      case EAGAIN:
        perror("Failed to start ILOG thread : Insufficient resources\n");
        break;
      case EINVAL:
        perror("Failed to start ILOG thread : Invalid settings\n");
        break;
      case EPERM:
        perror("Failed to start ILOG thread : Invalid permissions\n");
        break;
      default:
        perror("Failed to start ILOG thread : Unknown error\n");
    }
    exit(1);
  }
}

void ilog_thread_join() {
  if (! ilog_is_enabled()) { return; }

  if (THREAD <= 0) {
    perror("No ILOG thread PID provided before join, abort\n");
    exit(2);
  }

  if (SHOULD_TERMINATE) {
    perror("Termination signal was already sent to ILOG thread, abort \n");
    exit(3);
  }

  // Tell the ILOG thread it should empty the queue then terminate
  SHOULD_TERMINATE = true;

  pthread_join(THREAD, NULL);
}

void ilog_thread_log_msg(char* msg) {
  int attempts = 10;

  /*Enqueue*/
  while (lfqueue_enq(QUEUE, (void*) msg) == -1 && attempts >= 0) {
    perror("ILOG queue seems full, retrying\n");
    --attempts;
  }
  
  if (attempts < 0) {
    perror("ILOG queue seems full, aborted after 10 attempts\n");
    free(msg);
    return;
  }
}

void* routine(void* _) {
  char* msg = NULL;
  
  int port = ilog_config_get_port();
  const char* ip = ilog_config_get_ip();

  // init the UDP context
  ilog_udp_client_t udp_client;
  if (ilog_start_udp_client(&udp_client, port, ip) != 0) {
    free(ip);
    perror("Failed to init ILOG udp client\n");
    exit(1);
  }

  for (;;) {
    // routine

    msg = (char*) lfqueue_single_deq(QUEUE);

    if (msg == NULL) {
      // Queue is empty, check for termination signal
      if (SHOULD_TERMINATE) {
        break;
      }

      // If not, we shall wait a bit
      lfqueue_sleep(THREAD_WAIT_MS);
      continue;
    }

    ilog_log_msg_to_udp(&udp_client, msg, strlen(msg));

    free(msg);
    msg = NULL;
  }

  free(ip);
  ilog_close_udp_client(&udp_client);
}

