#include "internalog.h"

#include "json-builder.h"
#include <curl/curl.h>

json_value* zend_type_to_json_type(zval* val);
json_value* zend_ht_to_json_array(HashTable* zend_ht);
void post_json_to_url(const char* url, json_value* json);

/**
 * @brief Translate a Zend HashTable (used as arrays) into a json array
 */
json_value* zend_ht_to_json_array(HashTable* zend_ht) {
  json_value* json_arr = json_array_new(0);
  HashPosition hpos = 0;

  zend_hash_internal_pointer_reset_ex(zend_ht, &hpos);
  zval* data = zend_hash_get_current_data_ex(zend_ht, &hpos);
  
  while (data) {
    // Translate to JSON then push
    json_array_push(json_arr, zend_type_to_json_type(data));

    // Try to read next val
    zend_hash_move_forward_ex(zend_ht, &hpos);
    data = zend_hash_get_current_data_ex(zend_ht, &hpos);
  }

  return json_arr;
}

/**
 * @brief Translate any Zend type into its correspoding json type if
 * possible
 * 
 * @returns A valid json_value* ptr. If the type is unknown or unsupported
 * returns a json string with the description of the issue
 */
json_value* zend_type_to_json_type(zval* val) {
  switch (Z_TYPE_P(val)) {
    case IS_NULL:
        return json_null_new();
      case IS_TRUE:
        return json_boolean_new(1);
        break;
      case IS_FALSE:
        return json_boolean_new(0);
        break;
      case IS_LONG:
        return json_integer_new(Z_LVAL_P(val));
        break;
      case IS_DOUBLE:
        return json_double_new(Z_DVAL_P(val));
        break;
      case IS_STRING:
        return json_string_new(Z_STRVAL_P(val));
        break;
      case IS_RESOURCE:
        return json_string_new("zend_resource : not yet supported");
        break;
      case IS_ARRAY:
        return zend_ht_to_json_array(Z_ARRVAL_P(val));
        break;
      case IS_OBJECT:
        return json_string_new("zend_object : not yet supported");
        break;
      default:
        return json_string_new("unknown type");
        break;
  }
}

void log_zval_parameters(zval* args, int argc, const char* function_name) {
  // Create the function payload
  json_value* json = json_object_new(0);
  json_object_push(json, "name", json_string_new(function_name));
  
  // Create the parameter list
  json_value* parameters_arr = json_array_new(0);

  // Add parameters to the list
  for (int i = 0; i<argc; ++i) {
    json_array_push(parameters_arr, zend_type_to_json_type(&args[i]));
  }

  // Add the parameter list to the main object
  json_object_push(json, "parameters", parameters_arr);

  // POST the function payload
  post_json_to_url("localhost:5555", json);

  // free
  json_builder_free(json);
}

/**
 * @brief NOOP function to bypass libcurl output
 * @return size_t The fake number bof bytes read
 */
size_t noop_cb(void *ptr, size_t size, size_t nmemb, void *data) {
  return size * nmemb;
}

void post_json_to_url(const char* url, json_value* json) {
  CURL* curl;
  CURLcode res;

  // Serialize the JSON
  json_serialize_opts opts = { json_serialize_mode_packed};
  char* json_str = malloc(json_measure_ex(json, opts));
  json_serialize_ex(json_str, json, opts);

  /* In windows, this will init the winsock stuff */
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */
  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
    curl_easy_setopt(curl, CURLOPT_URL, url);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);
    /* Suppress libcurl output */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, noop_cb);

    // Specify the content-type
    struct curl_slist *hs=NULL;
    hs = curl_slist_append(hs, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
 
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    curl_slist_free_all(hs);

    /* always cleanup */
    curl_easy_cleanup(curl);
  } else {
    fprintf(stderr, "curl_easy_init() failed");
  }
  curl_global_cleanup();

  free(json_str);
}