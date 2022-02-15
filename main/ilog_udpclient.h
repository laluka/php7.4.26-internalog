#ifndef __ILOG_UDPCLIENT_H__
#define __ILOG_UDPCLIENT_H__

#include <netinet/in.h>

typedef struct ilog_udp_client {
  int sockfd;
  struct sockaddr_in  servaddr;

} ilog_udp_client_t;

/**
 * @brief Init the ILOG udp client
 */
int ilog_start_udp_client(ilog_udp_client_t* client, int port, const char* ip);

/**
 * @brief Close the socket of the UDP client
 */
void ilog_close_udp_client(ilog_udp_client_t* client);

/**
 * @brief Send a message using the given UDP client
 */
void ilog_log_msg_to_udp(const ilog_udp_client_t* client, const char* msg, size_t size);

#endif //__ILOG_UDPCLIENT_H__