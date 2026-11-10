/** @file github.c
* @brief Command-line entry point for the GitHub API client.
*
* Executes the action selected from the command-line arguments.
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
/**
 * @brief Parse command-line options and execute the selected GitHub operation.
 * @param[in] argc Number of command-line arguments.
 * @param[in] pp_argv Command-line argument vector.
 * @return EXIT_SUCCESS when the operation succeeds, otherwise EXIT_FAILURE.
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
			ret = github_create_user_repo(p_prog_args->p_repo_name);
		break;
		case DELETE_REPO:
			ret = github_delete_user_repo(p_prog_args->p_repo_name, &github_conf);
		break;
		case LIST_REPO:
			ret = github_get_user_repo_list(&github_conf);
		break;
		case LIST_MR:
		default:
			printf("\n This module was not implemented! \n\n");
			ret = EXIT_FAILURE;
		break;
	}

out:
	return ret;
}
/******************************************************************************/
