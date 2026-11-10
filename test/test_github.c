#include <CUnit/Basic.h>
#include "common.h"

void test_github_null_and_empty(void)
{
	CU_ASSERT_EQUAL(github_initialize(NULL), EXIT_FAILURE);
	CU_ASSERT_EQUAL(github_get_user_info(NULL), EXIT_FAILURE);
	CU_ASSERT_EQUAL(github_get_user_repo_list(NULL), EXIT_FAILURE);
	CU_ASSERT_EQUAL(github_create_user_repo(NULL), EXIT_FAILURE);
	CU_ASSERT_EQUAL(github_create_user_repo(""), EXIT_FAILURE);
	CU_ASSERT_EQUAL(github_delete_user_repo(NULL, NULL), EXIT_FAILURE);
}

void test_github_missing_token(void)
{
	const char *current_token = getenv("GITHUB_TOKEN");
	char *saved_token = current_token ? strdup(current_token) : NULL;
	if (current_token && !saved_token)
	{
		CU_FAIL("unable to preserve GITHUB_TOKEN for test");
		return;
	}

	if (unsetenv("GITHUB_TOKEN") != 0)
	{
		CU_FAIL("unable to unset GITHUB_TOKEN for test");
		free(saved_token);
		return;
	}
	github_conf_t conf = {0};
	CU_ASSERT_EQUAL(github_initialize(&conf), EXIT_FAILURE);

	if (saved_token)
	{
		if (setenv("GITHUB_TOKEN", saved_token, 1) != 0)
			CU_FAIL("unable to restore GITHUB_TOKEN after test");
		free(saved_token);
	}
	else if (unsetenv("GITHUB_TOKEN") != 0)
	{
		CU_FAIL("unable to keep GITHUB_TOKEN unset after test");
	}
}

void test_github_token_from_environment(void)
{
	const char *current_token = getenv("GITHUB_TOKEN");
	char *saved_token = current_token ? strdup(current_token) : NULL;
	if (current_token && !saved_token)
	{
		CU_FAIL("unable to preserve GITHUB_TOKEN for test");
		return;
	}

	if (setenv("GITHUB_TOKEN", "unit-test-token", 1) != 0)
	{
		CU_FAIL("unable to set GITHUB_TOKEN for test");
		free(saved_token);
		return;
	}

	CU_ASSERT_STRING_EQUAL(github_get_user_token(), "unit-test-token");

	if (saved_token)
	{
		if (setenv("GITHUB_TOKEN", saved_token, 1) != 0)
			CU_FAIL("unable to restore GITHUB_TOKEN after test");
		free(saved_token);
	}
	else if (unsetenv("GITHUB_TOKEN") != 0)
	{
		CU_FAIL("unable to keep GITHUB_TOKEN unset after test");
	}
}
