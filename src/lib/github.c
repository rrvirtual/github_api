/** @file github.c
* @brief GitHub API operations.
*
* Implements authenticated user and repository operations.
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Rodrigo Junior.
* All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <cjson/cJSON.h>

#include "system.h"
#include "github.h"

/************ DATA TYPES, CONSTANTS, MACROS ***********************************/

/******************************************************************************/

/************ PUBLIC FUNCTION BODIES ******************************************/
/**
 * @brief Read the API token from the `GITHUB_TOKEN` environment variable.
 * @return Borrowed pointer to the environment-owned token, or NULL if unset.
 */
char *github_get_user_token(void)
{
	return getenv("GITHUB_TOKEN");
}

/**
 * @brief Initialize the configuration and retrieve the authenticated user.
 * @param[out] p_github_conf Configuration structure to initialize.
 * @return EXIT_SUCCESS on success, otherwise EXIT_FAILURE.
 */
int github_initialize(github_conf_t *p_github_conf)
{
	int ret = EXIT_SUCCESS;

	if (!p_github_conf)
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	p_github_conf->p_user_token = github_get_user_token();
	if (!p_github_conf->p_user_token || !p_github_conf->p_user_token[0])
	{
		fprintf(stderr, "Error: set the GITHUB_TOKEN environment variable.\n");
		ret = EXIT_FAILURE;
		goto out;
	}

	ret = github_get_user_info(p_github_conf);
out:
	return ret;
}

/**
 * @brief Retrieve the authenticated user's login.
 * @param[in,out] p_github_conf Configuration containing the token and receiving
 *                              the user's login.
 * @return EXIT_SUCCESS on success, otherwise EXIT_FAILURE.
 */
int github_get_user_info(github_conf_t *p_github_conf)
{
	int ret = EXIT_SUCCESS;
	if (!p_github_conf || !p_github_conf->p_user_token)
		return EXIT_FAILURE;

	char *p_ret_github = url_handle_tls_get(GITHUB_API_USER_URL, p_github_conf->p_user_token, false);

	cJSON *p_json        = NULL;

	p_json = cJSON_Parse(p_ret_github);
	if (!cJSON_IsObject(p_json))
	{
		ret = EXIT_FAILURE;
		goto out;
	}
	cJSON *p_login = cJSON_GetObjectItemCaseSensitive(p_json, "login");
	if (!cJSON_IsString(p_login) || !p_login->valuestring)
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	int login_len = snprintf(p_github_conf->login, sizeof(p_github_conf->login), "%s",
	                         p_login->valuestring);
	if (login_len < 0 || (size_t)login_len >= sizeof(p_github_conf->login))
	{
		ret = EXIT_FAILURE;
		goto out;
	}

out:
	if (p_json)
		cJSON_Delete(p_json);
	if(p_ret_github)
		system_free((void**)&p_ret_github);
	return ret;
}

/**
 * @brief List all public and private repositories accessible to the token.
 * @param[in] p_github_conf Configuration containing the API token.
 * @return EXIT_SUCCESS on success, otherwise EXIT_FAILURE.
 */
int github_get_user_repo_list(github_conf_t *p_github_conf)
{
	int ret = EXIT_SUCCESS;
	size_t page = 1;
	char page_url[MAX_URL_LENGHT + 1];
	char *p_ret_github = NULL;
	cJSON *p_json = NULL;

	if (!p_github_conf || !p_github_conf->p_user_token)
		return EXIT_FAILURE;

	printf("\n You have the repositories: \n");
	while (true)
	{
		int page_url_len = snprintf(page_url, sizeof(page_url),
		                            "%s?visibility=all&per_page=100&page=%zu",
		                            GITHUB_API_USER_REPOS_URL, page);
		if (page_url_len < 0 || (size_t)page_url_len >= sizeof(page_url))
		{
			ret = EXIT_FAILURE;
			goto out;
		}

		p_ret_github = url_handle_tls_get(page_url, p_github_conf->p_user_token, false);
		p_json = cJSON_Parse(p_ret_github);
		if (!cJSON_IsArray(p_json))
		{
			ret = EXIT_FAILURE;
			goto out;
		}

		int repo_count = cJSON_GetArraySize(p_json);
		for (int idx = 0; idx < repo_count; idx++)
		{
			cJSON *p_repo = cJSON_GetArrayItem(p_json, idx);
			cJSON *p_name = cJSON_GetObjectItemCaseSensitive(p_repo, "name");
			cJSON *p_private = cJSON_GetObjectItemCaseSensitive(p_repo, "private");
			if (!cJSON_IsObject(p_repo) || !cJSON_IsString(p_name) ||
			    !p_name->valuestring || !cJSON_IsBool(p_private))
			{
				ret = EXIT_FAILURE;
				goto out;
			}

			printf("\tRepo Name: %s (%s)\n", p_name->valuestring,
			       cJSON_IsTrue(p_private) ? "private" : "public");
		}

		cJSON_Delete(p_json);
		p_json = NULL;
		system_free((void **)&p_ret_github);

		if (repo_count < 100)
			break;
		if (page == SIZE_MAX)
		{
			ret = EXIT_FAILURE;
			goto out;
		}
		page++;
	}
	printf("\n");

out:
	if (p_json)
		cJSON_Delete(p_json);
	if (p_ret_github)
		system_free((void**)&p_ret_github);
	return ret;
}

/**
 * @brief Create a repository for the authenticated user.
 * @param[in] p_repo_name Non-empty repository name.
 * @return EXIT_SUCCESS on success, otherwise EXIT_FAILURE.
 */
int github_create_user_repo(const char *p_repo_name)
{
	int ret = EXIT_SUCCESS;
	cJSON *p_json = NULL;
	cJSON *p_json_ret_github = NULL;
	char *p_msg_to_send = NULL;
	char *p_ret_github = NULL;

	if (!p_repo_name || !p_repo_name[0])
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	p_json = cJSON_CreateObject();
	if (!p_json)
	{
		ret = EXIT_FAILURE;
		goto out;
	}
	if (!cJSON_AddStringToObject(p_json, "name", p_repo_name) ||
	    !cJSON_AddStringToObject(p_json, "description", "Created by C github API"))
	{
		ret = EXIT_FAILURE;
		goto out;
	}


	p_msg_to_send = cJSON_Print(p_json);
	if (!p_msg_to_send)
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	p_ret_github = url_handle_tls_post(GITHUB_API_CREATE_REPO, p_msg_to_send,
	                                   github_get_user_token(), false);
	p_json_ret_github = cJSON_Parse(p_ret_github);
	if (!cJSON_IsObject(p_json_ret_github))
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	cJSON *p_repos_url = cJSON_GetObjectItemCaseSensitive(p_json_ret_github, "name");
	if (!cJSON_IsString(p_repos_url) || !p_repos_url->valuestring)
	{
		printf("\n Error to create repository (%s)\n", p_repo_name);
		ret = EXIT_FAILURE;
		goto out;
	}

	printf("\n The repository (%s) has been created!\n\n", p_repo_name);

out:
	if (p_json)
	{
		system_free((void**)&p_msg_to_send);
		cJSON_Delete(p_json);
	}
	if (p_json_ret_github)
	{
		cJSON_Delete(p_json_ret_github);
	}
	if(p_ret_github)
		system_free((void**)&p_ret_github);

	return ret;
}

/**
 * @brief Delete a repository owned by the authenticated user.
 * @param[in] p_repo_name Non-empty repository name.
 * @param[in] p_github_conf Configuration containing the login and API token.
 * @return EXIT_SUCCESS on success, otherwise EXIT_FAILURE.
 */
int github_delete_user_repo(const char *p_repo_name, github_conf_t *p_github_conf)
{
	int ret = EXIT_SUCCESS;
	char *p_ret_github = NULL;

	if (!p_repo_name)
	{
		ret = EXIT_FAILURE;
		goto out;
	}
	if (!p_github_conf || !p_github_conf->p_user_token || !p_github_conf->login[0] ||
	    !p_repo_name[0])
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	char github_repo_delete_url[MAX_URL_LENGHT + 1];
	int url_len = snprintf(github_repo_delete_url, sizeof(github_repo_delete_url),
	                       "%s/%s/%s", GITHUB_API_DELETE_REPO,
	                       p_github_conf->login, p_repo_name);
	if (url_len < 0 || (size_t)url_len >= sizeof(github_repo_delete_url))
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	p_ret_github = url_handle_tls_delete(github_repo_delete_url,
	                                     p_github_conf->p_user_token, false);

	if (!p_ret_github || strlen(p_ret_github) != 0)
	{
		printf("Error deleting repository (%s)\n", p_repo_name);
		ret = EXIT_FAILURE;
		goto out;
	}

	printf("\n The repository (%s) has been deleted!\n\n", p_repo_name);

out:
	if(p_ret_github)
		system_free((void**)&p_ret_github);
	return ret;
}
/******************************************************************************/
