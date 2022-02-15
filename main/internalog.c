#include "internalog.h"

#include "ilog_thread.h"
#include "ilog_config.h"


cJSON* zend_type_to_json_type(zval* val);
cJSON* zend_ht_to_json_array(HashTable* zend_ht);

/**
 * @brief Translate a Zend HashTable (used as arrays) into a json array
 */
cJSON* zend_ht_to_json_array(HashTable* zend_ht) {
  cJSON* json_arr = cJSON_CreateArray();
  HashPosition hpos = 0;

  zend_hash_internal_pointer_reset_ex(zend_ht, &hpos);
  zval* data = zend_hash_get_current_data_ex(zend_ht, &hpos);
  
  while (data) {
    // Translate to JSON then push
    cJSON_AddItemToArray(json_arr, zend_type_to_json_type(data));

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
cJSON* zend_type_to_json_type(zval* val) {
  switch (Z_TYPE_P(val)) {
    case IS_NULL:
        return cJSON_CreateNull();
      case IS_TRUE:
        return cJSON_CreateTrue();
        break;
      case IS_FALSE:
        return cJSON_CreateFalse();
        break;
      case IS_LONG:
        return cJSON_CreateNumber(Z_LVAL_P(val));
        break;
      case IS_DOUBLE:
        return cJSON_CreateNumber(Z_DVAL_P(val));
        break;
      case IS_STRING:
        return cJSON_CreateString(Z_STRVAL_P(val));
        break;
      case IS_RESOURCE:
        return cJSON_CreateString("zend_resource : not yet supported");
        break;
      case IS_ARRAY:
        return zend_ht_to_json_array(Z_ARRVAL_P(val));
        break;
      case IS_OBJECT:
        return cJSON_CreateString("zend_object : not yet supported");
        break;
      default:
        return cJSON_CreateString("unknown type");
        break;
  }
}

void log_zval_parameters(zval* args, int argc, const char* function_name) {
  if (! ilog_is_enabled()) { return; }
  
  // Create the function payload
  cJSON* json = cJSON_CreateObject();
  cJSON_AddItemToObject(json, "name", cJSON_CreateString(function_name));
  
  // Create the parameter list
  cJSON* parameters_arr = cJSON_CreateArray();

  // Add parameters to the list
  for (int i = 0; i<argc; ++i) {
    cJSON_AddItemToArray(parameters_arr, zend_type_to_json_type(&args[i]));
  }

  // Add the parameter list to the main object
  cJSON_AddItemToObject(json, "parameters", parameters_arr);

  // Serialize the JSON
  char* json_str = cJSON_PrintUnformatted(json);

  // log
  log_msg(json_str);

  // free
  cJSON_Delete(json);
}