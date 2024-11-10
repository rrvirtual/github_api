#include <CUnit/Basic.h>
#include "common.h"


void test_system_null_and_empty(void)
{
	char *p_test_null = NULL;
	system_free((void **)&p_test_null);
	CU_ASSERT_PTR_NULL(p_test_null);
}