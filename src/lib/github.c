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
#include <string.h>
#include <cjson/cJSON.h>

#include "system.h"
#include "github.h"

/************ DATA TYPES, CONSTANTS, MACROS ***********************************/

/******************************************************************************/

/************ PUBLIC FUNCTIONS PROTOTYPES ************************************/

/*!
 * @brief to call HTTPS URL as GET method.
 * @warning To make it simple, we create a function that returns the token
 * storaged in github.h
 * @return pointer github token
 */
char *github_get_user_token(void)
{
	return GITHUB_USER_TOKEN;
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

	if (!p_github_conf->p_user_token || strlen(p_github_conf->p_user_token ) == 0)
		printf("\n Warning: You must set your github token on lib/github.conf, option GITHUB_USER_TOKEN\n\n");

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
	char *p_ret_github = url_handle_tls_get(GITHUB_API_USER_URL, p_github_conf->p_user_token, true);

	cJSON *p_json        = NULL;

	p_json = cJSON_Parse(p_ret_github);
	if (!p_json)
	{
		ret = EXIT_FAILURE;
		goto out;
	}
	cJSON *p_repos_url = cJSON_GetObjectItem(p_json, "repos_url");
	if (!p_repos_url)
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	snprintf(p_github_conf->repos_url, MAX_URL_LENGHT, "%s", p_repos_url->valuestring);

	cJSON *p_login = cJSON_GetObjectItem(p_json, "login");
	if (!p_login)
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	snprintf(p_github_conf->login, GITHUB_MAX_USR_LEN, "%s", p_login->valuestring);

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

	char *p_ret_github = url_handle_tls_get((char *)p_github_conf->repos_url, GITHUB_USER_TOKEN, true);

	cJSON *p_json = NULL;
	cJSON *p_array = NULL;

	p_json = cJSON_Parse(p_ret_github);
	if (!p_json)
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
		{
			cJSON *p_name = cJSON_GetObjectItem(p_array, "name");
			printf("\tRepo Name: %s\n", p_name->valuestring);
		}
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

	if (!p_repo_name)
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	cJSON *p_json = NULL;
	p_json = cJSON_CreateObject();
	if (!p_json)
	{
		ret = EXIT_FAILURE;
		goto out;
	}
	cJSON_AddStringToObject(p_json, "name", p_repo_name);
	cJSON_AddStringToObject(p_json, "description", "Created by C github API");


	char *p_msg_to_send	= NULL;
	p_msg_to_send = cJSON_Print(p_json);

	char *p_ret_github = url_handle_tls_post(GITHUB_API_CREATE_REPO, p_msg_to_send, GITHUB_USER_TOKEN, true);

	cJSON *p_json_ret_github = NULL;

	p_json_ret_github = cJSON_Parse(p_ret_github);
	if (!p_json_ret_github)
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	cJSON *p_repos_url = cJSON_GetObjectItem(p_json_ret_github, "name");
	if (!p_repos_url)
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

	if (!p_repo_name)
	{
		ret = EXIT_FAILURE;
		goto out;
	}

	char github_repo_delete_url[MAX_URL_LENGHT];
	snprintf(github_repo_delete_url, MAX_URL_LENGHT, "%s/%s/%s", GITHUB_API_DELETE_REPO, p_github_conf->login, p_repo_name);

	char *p_ret_github = NULL;
	p_ret_github = url_handle_tls_delete(github_repo_delete_url, GITHUB_USER_TOKEN, true);

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
