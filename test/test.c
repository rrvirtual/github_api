#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "common.h"

void test_lib_url()
{
	test_url_null_and_empty();
}

void test_lib_system()
{
	test_system_null_and_empty();
}

void test_lib_github()
{
	test_github_null_and_empty();
	test_github_missing_token();
	test_github_token_from_environment();
}

int main() {
	if (CU_initialize_registry() != CUE_SUCCESS)
		return EXIT_FAILURE;

	CU_pSuite suite = CU_add_suite("GitHUB API Tests", 0, 0);
	if (!suite ||
	    !CU_add_test(suite, "test of lib/url", test_lib_url) ||
	    !CU_add_test(suite, "test of lib/system", test_lib_system) ||
	    !CU_add_test(suite, "test of lib/github", test_lib_github))
	{
		CU_cleanup_registry();
		return EXIT_FAILURE;
	}

	CU_basic_run_tests();
	unsigned int failures = CU_get_number_of_failures();
	CU_cleanup_registry();

	return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}


