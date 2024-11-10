#ifndef URL_H_
#define URL_H_

/************ DATA TYPES, CONSTANTS, MACROS ***********************************/
#define STR_CODE_TABLE_SIZE			256
#define MAX_URL_LENGHT				2048
#define MAX_URL_ENCODED_LENGHT		3192
#define URL_DEFAULT_CURL_TIMEOUT	5000L

typedef enum
{
	CURL_CONTENT_TYPE_RAW,
	CURL_CONTENT_TYPE_X_FORM_WWW_URL_ENCODED,
	CURL_CONTENT_TYPE_FORM_DATA
} curl_content_type_e;
/******************************************************************************/

/************ PUBLIC FUNCTIONS PROTOTYPES ************************************/
char
*url_handle_tls_get(const char *url_to_get, const char *auth_token, const bool b_insecure);
char
*url_handle_tls_post(const char *url_to_post, const char *data_to_post,
                                 const char *auth_token, const bool b_insecure);
char
*url_handle_tls_delete(const char *url_to_delete, const char *auth_token, const bool b_insecure);
/******************************************************************************/

#endif //URL_H_
