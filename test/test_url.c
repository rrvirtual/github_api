#include <CUnit/Basic.h>
#include "common.h"

void test_url_null_and_empty(void)
{
	char oversized_url[MAX_URL_LENGHT + 2];
	memset(oversized_url, 'a', sizeof(oversized_url) - 1);
	oversized_url[sizeof(oversized_url) - 1] = '\0';

	CU_ASSERT_PTR_NULL(url_handle_tls_get(NULL, NULL, true));
	CU_ASSERT_PTR_NULL(url_handle_tls_get(NULL, "", true));
	CU_ASSERT_PTR_NULL(url_handle_tls_get("", NULL, true));
	CU_ASSERT_PTR_NULL(url_handle_tls_get("", "", true));

	CU_ASSERT_PTR_NULL(url_handle_tls_post(NULL, NULL, NULL, true));
	CU_ASSERT_PTR_NULL(url_handle_tls_post(NULL, NULL, "", true));
	CU_ASSERT_PTR_NULL(url_handle_tls_post("", NULL, NULL, true));
	CU_ASSERT_PTR_NULL(url_handle_tls_post("", "", NULL, true));

	CU_ASSERT_PTR_NULL(url_handle_tls_delete(NULL, NULL, true));
	CU_ASSERT_PTR_NULL(url_handle_tls_delete(NULL, "", true));
	CU_ASSERT_PTR_NULL(url_handle_tls_delete("", NULL, true));
	CU_ASSERT_PTR_NULL(url_handle_tls_delete("", "", true));

	CU_ASSERT_PTR_NULL(url_handle_tls_get("https://api.github.com/user", NULL, false));
	CU_ASSERT_PTR_NULL(url_handle_tls_get("https://api.github.com/user", "", false));
	CU_ASSERT_PTR_NULL(url_handle_tls_post("https://api.github.com/user", "{}", NULL, false));
	CU_ASSERT_PTR_NULL(url_handle_tls_delete("https://api.github.com/user", NULL, false));
	CU_ASSERT_PTR_NULL(url_handle_tls_get(oversized_url, "test-token", false));
}
