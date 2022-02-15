#ifndef _ILOG_CONFIG_H__
#define _ILOG_CONFIG_H__

#include <stdbool.h>

/** 
 * @note returned value should be freed by the caller
 */
const char* ilog_config_get_ip();
int ilog_config_get_port();
bool ilog_is_enabled();

void ilog_config_init();

#endif //_ILOG_CONFIG_H__