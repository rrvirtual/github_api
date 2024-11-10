#include <CUnit/Basic.h>
#include "common.h"

void test_url_null_and_empty(void)
{
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
}