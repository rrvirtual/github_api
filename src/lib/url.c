
/** @file url.c
* @brief HTTP transport helpers based on libcurl.
*
* Implements authenticated GitHub API requests and response handling.
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Rodrigo Junior.
* All rights reserved.
*/

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <curl/curl.h>

#include "url.h"
#include "system.h"


/************ DATA TYPES, CONSTANTS, MACROS ***********************************/
/*! @brief Struct to use with url_handle_url_to_post()
 * @see url_handle_url_to_post()
 */
struct url_data
{
	size_t size;
	char *data;
};

typedef enum {
	URL_METHOD_GET,
	URL_METHOD_POST,
	URL_METHOD_DELETE
} e_url_method;
/******************************************************************************/

/************ PRIVATE FUNCTION PROTOTYPES *************************************/
static size_t
write_data(void *ptr, size_t size, size_t nmemb, void *user_data);

static bool
append_header(struct curl_slist **headers, const char *value);

static char
*curl_call_handler(const char *url, const char *auth_token, e_url_method url_method,
                   const char *data_to_post, const curl_content_type_e content_type,
                   long timeout, const bool b_insecure);

/******************************************************************************/

/************ PRIVATE FUNCTION BODIES *****************************************/
/**
 * @brief Append one response chunk to the dynamically allocated response body.
 * @param[in] ptr Pointer to the received data chunk.
 * @param[in] size Size in bytes of each data element.
 * @param[in] nmemb Number of data elements in the chunk.
 * @param[in,out] user_data Pointer to the response buffer state.
 * @return Number of bytes consumed, or zero on invalid input or allocation
 *         failure.
 */
static size_t
write_data(void *ptr, size_t size, size_t nmemb, void *user_data)
{
	if (!ptr || !user_data || !size || !nmemb || nmemb > SIZE_MAX / size)
	{
		return 0;
	}

	struct url_data *data = (struct url_data *)user_data;
	size_t index = data->size;
	size_t n = size * nmemb;
	char *tmp;

	if (n == SIZE_MAX || data->size > SIZE_MAX - n - 1)
	{
		return 0;
	}

	data->size += n;

	tmp = realloc(data->data, data->size + 1); /* +1 for '\0' */

	if (tmp)
	{
		data->data = tmp;
	}
	else
	{
		fprintf(stderr, "Failed to allocate memory.\n");
		return 0;
	}

	memcpy((data->data + index), ptr, n);
	data->data[data->size] = '\0';

	return size * nmemb;
}

/**
 * @brief Append a header to a libcurl header list.
 * @param[in,out] headers Address of the header-list pointer.
 * @param[in] value Header line to append.
 * @return true if the header was appended, otherwise false.
 */
static bool
append_header(struct curl_slist **headers, const char *value)
{
	struct curl_slist *updated = curl_slist_append(*headers, value);
	if (!updated)
	{
		return false;
	}
	*headers = updated;
	return true;
}

/**
 * @brief Perform an authenticated HTTP request using libcurl.
 * @param[in] url Absolute request URL, no longer than MAX_URL_LENGHT.
 * @param[in] auth_token GitHub API token used for the Authorization header.
 * @param[in] url_method HTTP method to perform.
 * @param[in] data_to_post Request body for POST, otherwise NULL.
 * @param[in] content_type Request content type.
 * @param[in] timeout Request timeout in milliseconds.
 * @param[in] b_insecure If true, disable TLS peer and host verification.
 * @return Heap-allocated response body on a successful 2xx response; the caller
 *         owns the result and must free it. Returns NULL on invalid input,
 *         transport failure, or non-2xx response.
 */
static char
*curl_call_handler(const char *url, const char *auth_token, e_url_method url_method,
                   const char *data_to_post, const curl_content_type_e content_type,
                   long timeout, const bool b_insecure)
{
	if (!url || !url[0] || strlen(url) > MAX_URL_LENGHT ||
	    !auth_token || !auth_token[0] ||
	    (url_method == URL_METHOD_POST && !data_to_post))
	{
		return NULL;
	}

	struct url_data data = {0, NULL};
	CURL *curl = NULL;
	// Headers
	struct curl_slist *headers = NULL;
	if (URL_METHOD_POST == url_method)
	{
		if (content_type == CURL_CONTENT_TYPE_RAW)
		{
			if (!append_header(&headers, "Accept: application/json") ||
			    !append_header(&headers, "Content-Type: application/json"))
				goto out;
		}

		if (content_type == CURL_CONTENT_TYPE_X_FORM_WWW_URL_ENCODED)
		{
			if (!append_header(&headers, "Content-Type: application/x-www-form-urlencoded"))
				goto out;
		}

		if (!append_header(&headers, "charsets: utf-8"))
			goto out;
	}
	else
	{
		if (content_type == CURL_CONTENT_TYPE_RAW)
		{
			if (!append_header(&headers, "Accept: application/vnd.github+json") ||
			    !append_header(&headers, "X-GitHub-Api-Version: 2022-11-28"))
				goto out;
		}

		if (content_type == CURL_CONTENT_TYPE_X_FORM_WWW_URL_ENCODED)
		{
			if (!append_header(&headers, "Content-Type: application/x-www-form-urlencoded"))
				goto out;
		}
	}

	char auth_header[STR_CODE_TABLE_SIZE];
	int auth_header_len = snprintf(auth_header, sizeof(auth_header),
	                               "Authorization: Bearer %s", auth_token);
	if (auth_header_len < 0 || (size_t)auth_header_len >= sizeof(auth_header))
		goto out;
	if (!append_header(&headers, auth_header))
		goto out;

	if (!headers)
		goto out;

	curl = curl_easy_init();
	if (!curl)
	{
		goto out;
	}

	data.data = malloc(1); /* reasonable size initial buffer */
	if (NULL == data.data)
	{
		fprintf(stderr, "Failed to allocate memory.\n");
		goto out;
	}
	data.data[0] = '\0';

	if (URL_METHOD_DELETE == url_method)
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	}

	if (URL_METHOD_POST == url_method)
	{
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_to_post);
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl");

	if (b_insecure)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
	}

	char errbuf[CURL_ERROR_SIZE];
	errbuf[0] = 0;
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL,1);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		size_t len = strlen(errbuf);
		fprintf(stderr, "\nlibcurl: (%d) ", res);
		if(len)
		{
			fprintf(stderr, "%s%s", errbuf, ((errbuf[len - 1] != '\n') ? "\n" : ""));
		}
		else
		{
			fprintf(stderr, "%s\n", curl_easy_strerror(res));
		}
		system_free((void **) &data.data);
	}
	else
	{
		long response_code = 0;
		if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code) != CURLE_OK ||
		    response_code < 200 || response_code >= 300)
		{
			fprintf(stderr, "GitHub API returned HTTP %ld.\n", response_code);
			system_free((void **)&data.data);
		}
	}

out:
	if (curl)
	{
		curl_easy_cleanup(curl);
	}

	if (headers)
	{
		/* free the custom headers */
		curl_slist_free_all(headers);
	}

	return data.data;
}
/******************************************************************************/

/************ PUBLIC FUNCTION BODIES *****************************************/
/**
 * @brief Send an authenticated HTTPS GET request.
 * @param[in] url_to_get Absolute request URL.
 * @param[in] auth_token GitHub API token.
 * @param[in] b_insecure If true, disable TLS certificate verification.
 * @return Heap-allocated response body on success; caller must free it.
 *         Returns NULL for invalid input or a failed request.
 */
char
*url_handle_tls_get(const char *url_to_get, const char *auth_token, const bool b_insecure)
{
	return curl_call_handler(url_to_get, auth_token, URL_METHOD_GET, NULL, CURL_CONTENT_TYPE_RAW,
	                         URL_DEFAULT_CURL_TIMEOUT, b_insecure);
}

/**
 * @brief Send an authenticated HTTPS POST request with a JSON body.
 * @param[in] url_to_post Absolute request URL.
 * @param[in] data_to_post JSON request body.
 * @param[in] auth_token GitHub API token.
 * @param[in] b_insecure If true, disable TLS certificate verification.
 * @return Heap-allocated response body on success; caller must free it.
 *         Returns NULL for invalid input or a failed request.
 */
char
*url_handle_tls_post(const char *url_to_post, const char *data_to_post,
                                 const char *auth_token, const bool b_insecure)
{
	return curl_call_handler(url_to_post, auth_token, URL_METHOD_POST, data_to_post,
	                         CURL_CONTENT_TYPE_RAW, URL_DEFAULT_CURL_TIMEOUT, b_insecure);
}

/**
 * @brief Send an authenticated HTTPS DELETE request.
 * @param[in] url_to_delete Absolute request URL.
 * @param[in] auth_token GitHub API token.
 * @param[in] b_insecure If true, disable TLS certificate verification.
 * @return Heap-allocated response body on success; caller must free it.
 *         Returns NULL for invalid input or a failed request.
 */
char
*url_handle_tls_delete(const char *url_to_delete, const char *auth_token, const bool b_insecure)
{
	return curl_call_handler(url_to_delete, auth_token, URL_METHOD_DELETE, NULL,
	                         CURL_CONTENT_TYPE_RAW, URL_DEFAULT_CURL_TIMEOUT, b_insecure);
}
/******************************************************************************/
