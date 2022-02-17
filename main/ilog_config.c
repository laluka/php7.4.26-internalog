#include "ilog_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "cJSON.h"

// JSON keys of the config file
#define KEY_HOST "ihost"
#define KEY_PORT "iport"
#define KEY_ENABLE "enable"

// Misc
#define CONFIG_PATH "/etc/internalog.conf"
#define DEFAULT_CONF "{\"ihost\":\"0.0.0.0\",\"iport\":5555}" 

// Env variables
#define _ENV_MAKEFLAG "MAKEFLAGS"
#define _ENV_TEST_PHP_SRCDIR "TEST_PHP_SRCDIR"
extern char** environ;

bool check_env_for(const char* name) {
  char **s = environ;
  
  for (; *s; s++) {
    size_t eq_idx = strchr(*s, '=') - *s;
    if (strncmp(*s, name, eq_idx) == 0) {
      return true;
    }
  }

  return false;
} 

cJSON* load_config_file(const char* config_path) {
  char* buffer = NULL;
  int string_size, read_size;

  // Open the config file
  FILE* handler = fopen(config_path, "r");

  if (handler == NULL) {
    // Failed to open config file
    perror("Failed to open config file\nFallback to default configuration\n");
    return cJSON_ParseWithLength(DEFAULT_CONF, sizeof(DEFAULT_CONF));
  }

  // Compute string size
  fseek(handler, 0, SEEK_END);
  string_size = ftell(handler);
  rewind(handler);

  // Allocate and read
  buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
  read_size = fread(buffer, sizeof(char), string_size, handler);

  // fread doesn't set it so put a \0 in the last position
  // and buffer is now officially a string
  buffer[string_size] = '\0';

  if (string_size != read_size) {
    // Something went wrong
    free(buffer);
    fclose(handler);
    perror("Failed to read config file\nFallback to default configuration\n");
    return cJSON_ParseWithLength(DEFAULT_CONF, sizeof(DEFAULT_CONF));
  }

  // Parse
  cJSON* config = cJSON_ParseWithLength(buffer, read_size);

  if (config == NULL) {
    // Failed parsing
    free(buffer);
    fclose(handler);
    perror("Failed to parse config file\nFallback to default configuration\n");
    return cJSON_ParseWithLength(DEFAULT_CONF, sizeof(DEFAULT_CONF));
  }

  // free
  free(buffer);
  fclose(handler);

  return config;
}

static cJSON* CONFIG = NULL;
// static bool for quicker access since it will be called really often
static bool ENABLED;

const char* ilog_config_get_ip() {
  cJSON* ip_json = cJSON_GetObjectItem(CONFIG, KEY_HOST);
  if (ip_json == NULL || cJSON_IsInvalid(ip_json) || !cJSON_IsString(ip_json)) {
    perror("ILOG config : missing or invalid field \"" KEY_HOST "\"\n");
    exit(1);
  }
  return cJSON_GetStringValue(ip_json);
}

int ilog_config_get_port() {
  cJSON* port_json = cJSON_GetObjectItem(CONFIG, KEY_PORT);
  if (port_json == NULL || cJSON_IsInvalid(port_json) || !cJSON_IsNumber(port_json)) {
    perror("ILOG config : missing or invalid field \"" KEY_PORT "\"\n");
    exit(1);
  }
  return (int)cJSON_GetNumberValue(port_json);
}

bool ilog_is_enabled() {
  return ENABLED;
}

void ilog_config_init()
{
  if (CONFIG != NULL) {
    perror("ILOG was already configured, aborting\n");
    exit(1);
  }

  // PHP calls itself during compilation, we want to disable during compilation
  // but not during testing
  bool compiling = check_env_for(_ENV_MAKEFLAG) && (! check_env_for(_ENV_TEST_PHP_SRCDIR));

  CONFIG = load_config_file(CONFIG_PATH);

  // Check if we should disable ILOG
  if (compiling) {
    ENABLED = false;
  } else {
    cJSON* enable_item = cJSON_GetObjectItem(CONFIG, KEY_ENABLE);

    if (enable_item == NULL || cJSON_IsTrue(enable_item)) {
      ENABLED = true;
    } else {
      if (! cJSON_IsBool(enable_item)) {
        perror("ILOG config :  invalid field \"" KEY_ENABLE "\"\n");
        exit(1);
      }
    }

  }
}
