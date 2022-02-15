#include "ilog_udpclient.h"

#include "ilog_config.h"

int ilog_start_udp_client(ilog_udp_client_t* client, int port, const char* ip) {
  // Creating socket file descriptor
  if ( (client->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
    return 1;
  }

  memset(&client->servaddr, 0, sizeof(client->servaddr));

  // Filling server information
  client->servaddr.sin_family = AF_INET;
  client->servaddr.sin_port = htons(port);
  client->servaddr.sin_addr.s_addr = inet_addr(ip);

  return 0;
}

void ilog_close_udp_client(ilog_udp_client_t* client) {
  close(client->sockfd);
}

void ilog_log_msg_to_udp(const ilog_udp_client_t* client, const char* msg, 
    size_t size) {
  sendto(
      client->sockfd, 
      msg, 
      size, 
      MSG_CONFIRM, 
      &client->servaddr, 
      sizeof(client->servaddr)
  );
}
