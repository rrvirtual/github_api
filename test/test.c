#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "common.h"

void setUp()
{
}

void tearDown()
{
}

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
	test_github_string();
}

int main() {
	CU_initialize_registry();

	CU_pSuite suite = CU_add_suite("GitHUB API Tests", 0, 0);
	CU_add_test(suite, "test of lib/url", test_lib_url);
	CU_add_test(suite, "test of lib/system", test_lib_system);
	CU_add_test(suite, "test of lib/github", test_lib_github);

	CU_basic_run_tests();
	CU_cleanup_registry();

	return 0;
}




