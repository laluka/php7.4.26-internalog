#include "ilog_thread.h"

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>

#include <curl/curl.h>
#include "lfqueue.h"
#include "ilog_config.h"

#define THREAD_WAIT_MS 100

static pthread_t THREAD = 0;
static atomic_bool SHOULD_TERMINATE = false;
static lfqueue_t* QUEUE = NULL;


// Declarations
void* routine(void*);


// Definitions
void init_ilog_thread() {
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

void join_ilog_thread() {
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

void log_msg(char* msg) {
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


/**
 * @brief NOOP function to bypass libcurl output
 * @return size_t The fake number bof bytes read
 */
size_t noop_cb(void *ptr, size_t size, size_t nmemb, void *data) {
  return size * nmemb;
}

// void post_to_url(const char* url, const char* msg) {
//   CURL* curl;
//   CURLcode res;


//   /* In windows, this will init the winsock stuff */
//   curl_global_init(CURL_GLOBAL_ALL);
 
//   /* get a curl handle */
//   curl = curl_easy_init();
//   if(curl) {
//     /* First set the URL that is about to receive our POST. This URL can
//        just as well be a https:// URL if that is what should receive the
//        data. */
//     curl_easy_setopt(curl, CURLOPT_URL, url);
//     /* Now specify the POST data */
//     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, msg);
//     /* Suppress libcurl output */
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, noop_cb);

//     // Specify the content-type
//     struct curl_slist *hs=NULL;
//     hs = curl_slist_append(hs, "Content-Type: application/json");
//     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
 
//     /* Perform the request, res will get the return code */
//     res = curl_easy_perform(curl);
//     /* Check for errors */
//     if(res != CURLE_OK)
//       fprintf(stderr, "curl_easy_perform() failed: %s\n",
//               curl_easy_strerror(res));

//     curl_slist_free_all(hs);

//     /* always cleanup */
//     curl_easy_cleanup(curl);
//   } else {
//     fprintf(stderr, "curl_easy_init() failed");
//   }
//   curl_global_cleanup();
// }

void* routine(void* _) {
  char* msg = NULL;
  
  // init the UDP context

  // init CURL, to be removed
  CURL* curl;
  CURLcode res;
  curl_global_init(CURL_GLOBAL_ALL);

  /* get a curl handle */
  curl = curl_easy_init();
  if (! curl) {
    curl_global_cleanup();
    perror("Failed to init curl");
    exit(1);
  }

  // Setup constants
  const char* url = ilog_config_get_url();
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, noop_cb);
  struct curl_slist *hs=NULL;
  hs = curl_slist_append(hs, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);

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

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, msg);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    
    free(msg);
    msg = NULL;
  }

  curl_easy_cleanup(curl);
  curl_global_cleanup();
}

