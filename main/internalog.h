#ifndef INTERNALOG_H
#define INTERNALOG_H

#include "zend.h"
#include "json-builder.h"

/**
 * @brief Log the function name with the given parameters
 * 
 * @param args array of zval parameters, must have argc len
 * @param argc the length of the array
 * @param function_name the name of the function to log
 */
void log_zval_parameters(zval* args, int argc, const char* function_name);

/**
 * @brief Automatically log the function parameters
 * 
 * @param name The name of the function to log, expected char*
 * @note It is mandatory that this macro is set in the main scope of the function
 */
#define ILOG_FUNCTION(name)\
  zval       *args_;\
  int         argc_;\
  \
  argc_ = ZEND_NUM_ARGS();\
  args_ = safe_emalloc(argc_, sizeof(zval), 0);\
  \
  if (zend_get_parameters_array_ex(argc_, args_) == FAILURE) {\
    char* error_log_fmt_ = "%s : failed to get parameters";\
    ssize_t bufsz_ = snprintf(NULL, 0, error_log_fmt_, name);\
    char* error_log_ = malloc(bufsz_ + 1);\
    snprintf(error_log_, bufsz_ + 1, error_log_fmt_, name);\
    perror(error_log_);\
    free(error_log_);\
  } else {\
    log_zval_parameters(args_, argc_, name);\
  }\
  \
  efree(args_);\

#pragma GCC diagnostic ignored "-Wcomment"
// Lecacy POC
// /* Setup server with : nc -lnvkup 8888 */
// #include <netdb.h>
// #define LALUKA_LOG(lalu_p_str_logme) {\
//   int lalu_port = 8888;\
//   char * lalu_ip = "127.0.0.1";\
//   char * lalu_log_fmt = "LALUKA : %s\n";\
//   int lalu_sock;\
//   struct sockaddr_in lalu_server_addr;\
//   struct hostent *lalu_host;\
//   char* lalu_log_line;\
//   ssize_t lalu_bufsz;\
//   \
//   lalu_host = (struct hostent *)gethostbyname(lalu_ip);\
//   lalu_bufsz = snprintf(NULL, 0, lalu_log_fmt, lalu_p_str_logme);\
//   lalu_log_line = malloc(lalu_bufsz + 1);\
//   snprintf(lalu_log_line, lalu_bufsz + 1, lalu_log_fmt, lalu_p_str_logme);\
//   \
//   if ((lalu_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {\
//       perror("socket");\
//       exit(1);\
//   }\
//   \
//   lalu_server_addr.sin_family = AF_INET;\
//   lalu_server_addr.sin_port = htons(lalu_port);\
//   lalu_server_addr.sin_addr = *((struct in_addr *)lalu_host->h_addr);\
//   bzero(&(lalu_server_addr.sin_zero), 8);\
//   \
//   sendto(lalu_sock, lalu_log_line, strlen(lalu_log_line), 0, (struct sockaddr *)&lalu_server_addr, sizeof(struct sockaddr));\
//   close(lalu_sock);\
//   free(lalu_log_line);\
// }

#pragma GCC diagnostic ignored "-Wcomment"
// Not working attempt, could dig more in the future to get more context information
// #define LOG_STATE()\
//   json_value* js = zend_ht_to_json_array(&(compiler_globals.filenames_table));\
//   json_serialize_opts opts = { json_serialize_mode_multiline, 0, 2 };\
//   char* js_str = malloc(json_measure_ex(js, opts));\
//   json_serialize_ex(js_str, js, opts);\
//   LALUKA_LOG(js_str);\

#endif