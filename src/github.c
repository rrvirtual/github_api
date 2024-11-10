/** @file github.c
* @mainpage Program to access Github
*
*  Service to access github and do small task through its API
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Rodrigo Junior.
* All rights reserved.
*/

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lib/github.h"
#include "params.h"

/******************************************************************************/
/*!
* @brief Main function.
*/
int main(int argc, char **pp_argv)
{
	int ret = EXIT_SUCCESS;
	github_conf_t github_conf;
	cmd_args_t    *p_prog_args = NULL;

	p_prog_args = params_handle(argc, pp_argv);

	ret = github_initialize(&github_conf);
	if (EXIT_FAILURE == ret)
	{
		goto out;
	}

	switch(p_prog_args->e_action_list)
	{
		case CREATE_REPO:
			github_create_user_repo(p_prog_args->p_repo_name);
		break;
		case DELETE_REPO:
			github_delete_user_repo(p_prog_args->p_repo_name, &github_conf);
		break;
		case LIST_REPO:
			github_get_user_repo_list(&github_conf);
		break;
		case LIST_MR:
		default:
			printf("\n This module was not implemented! \n\n");
		break;
	}

out:
	return ret;
}
/******************************************************************************/
