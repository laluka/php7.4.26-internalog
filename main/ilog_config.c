#include "ilog_config.h"

#include <stdio.h>
#include <stdlib.h>

#include "cJSON.h"

// JSON keys of the config file
#define KEY_HOST "intake"

// Misc
#define CONFIG_PATH "/etc/internalog.conf"
#define DEFAULT_CONF "{\"intake\":\"0.0.0.0:5555\"}" 

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
    perror("Failed to parse config file\nFallback to default configuration");
    return cJSON_ParseWithLength(DEFAULT_CONF, sizeof(DEFAULT_CONF));
  }

  // free
  free(buffer);
  fclose(handler);

  return config;
}

static cJSON* CONFIG = NULL;

const char* ilog_config_get_url()
{
  cJSON* url_json = cJSON_GetObjectItem(CONFIG, KEY_HOST);
  return cJSON_GetStringValue(url_json);
  // return cJSON_GetObjectItem(CONFIG, KEY_HOST);
}

void init_config()
{
  CONFIG = load_config_file(CONFIG_PATH);
  int a = 2;
}
