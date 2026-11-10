/** @file github.c
* @mainpage Generic library to access Github
*
*  This API module access github through his API
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Rodrigo Junior.
* All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <cjson/cJSON.h>

#include "system.h"
#include "github.h"

/************ DATA TYPES, CONSTANTS, MACROS ***********************************/

/******************************************************************************/

/************ PUBLIC FUNCTIONS PROTOTYPES ************************************/

/*!
 * @brief Read the GitHub token from the GITHUB_TOKEN environment variable.
 * @return pointer to the environment-owned token, or NULL when unset
 */
char *github_get_user_token(void)
{
	return getenv("GITHUB_TOKEN");
}

/*!
 * @brief initialize github struct.
 * @param[in] p_github_conf as github_conf_t struct format
 * @return EXIT_SUCCESS if runs well or EXIT_FAILURE
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

/*!
 * @brief get github user informations
 * @param[in] p_github_conf as github_conf_t struct format
 * @return EXIT_SUCCESS if runs well or EXIT_FAILURE
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
	cJSON *p_repos_url = cJSON_GetObjectItemCaseSensitive(p_json, "repos_url");
	if (!cJSON_IsString(p_repos_url) || !p_repos_url->valuestring)
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	int repos_url_len = snprintf(p_github_conf->repos_url,
	                             sizeof(p_github_conf->repos_url), "%s",
	                             p_repos_url->valuestring);
	if (repos_url_len < 0 ||
	    (size_t)repos_url_len >= sizeof(p_github_conf->repos_url))
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

/*!
 * @brief get github user repo informations
 * @param[in] p_github_conf as github_conf_t struct format
 * @return EXIT_SUCCESS if runs well or EXIT_FAILURE
 */
int github_get_user_repo_list(github_conf_t *p_github_conf)
{
	int ret = EXIT_SUCCESS;

	if (!p_github_conf || !p_github_conf->p_user_token || !p_github_conf->repos_url[0])
		return EXIT_FAILURE;

	char *p_ret_github = url_handle_tls_get(p_github_conf->repos_url, p_github_conf->p_user_token, false);

	cJSON *p_json = NULL;
	cJSON *p_array = NULL;

	p_json = cJSON_Parse(p_ret_github);
	if (!cJSON_IsArray(p_json))
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	int rcv_array_size = cJSON_GetArraySize(p_json);
	// printf("p_repos_url %d \n", rcv_array_size);
	printf("\n You have the repositories: \n");
	for (int idx=0; idx < rcv_array_size; idx++)
	{
		p_array = cJSON_GetArrayItem(p_json, idx);
		cJSON *p_name = cJSON_GetObjectItemCaseSensitive(p_array, "name");
		if (!cJSON_IsString(p_name) || !p_name->valuestring)
		{
			ret = EXIT_FAILURE;
			goto out;
		}
		printf("\tRepo Name: %s\n", p_name->valuestring);
	}
	printf("\n");

out:
	if (p_json)
		cJSON_Delete(p_json);
	if(p_ret_github)
		system_free((void**)&p_ret_github);
	return ret;
}

/*!
 * @brief create github repo 
 * @param[in] p_repo_name name of repository as string format
 * @return EXIT_SUCCESS if runs well or EXIT_FAILURE
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

/*!
 * @brief delete github repo 
 * @param[in] p_repo_name name of repository as string format
 * @param[in] p_github_conf as github_conf_t struct format
 * @return EXIT_SUCCESS if runs well or EXIT_FAILURE
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
		printf("Erro to delete repo (%s)\n", p_repo_name);
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
