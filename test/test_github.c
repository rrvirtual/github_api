#include <CUnit/Basic.h>
#include "common.h"

void test_github_string(void)
{
	CU_ASSERT_STRING_EQUAL(github_get_user_token(), "");
}

void test_github_null_and_empty(void)
{
	CU_ASSERT_EQUAL(github_initialize(NULL), EXIT_FAILURE);
}