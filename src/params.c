/** @file params.c
 * @brief input arguments.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2024 Rodrigo Junior.
 * All rights reserved.
*/

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "params.h"
#include "lib/system.h"
#include "lib/help.h"

/************ DATA TYPES, CONSTANTS, MACROS ***********************************/
#define DEFAULT_ARGS      "h?c:d:lm:"
#define MAX_ARGUMENTS_LEN 127
/******************************************************************************/

/************ STATIC DATA DECLARATIONS ****************************************/
static help_description_t const help[] =
{
	{"Show this help"                           , NULL},
	{"Create a repo"                            , "[name]"},
	{"Delete a repo"                            , "[name]"},
	{"List the Users repo"                      , NULL},
	{"List the MR Open"                         , "[name]"},
	{NULL, NULL}
};

static struct option const longopts[] =
{
	{"help",            no_argument,       NULL, 'h'},
	{"create",          required_argument, NULL, 'c'},
	{"delete",          required_argument, NULL, 'd'},
	{"list",            no_argument,       NULL, 'l'},
	{"list_mr",         required_argument, NULL, 'm'},
	{NULL,              no_argument,       NULL,  0}
};
/******************************************************************************/

/************ PUBLIC FUNCTION BODIES ******************************************/
cmd_args_t
*params_handle(int argc, char **argv)
{
	int option = 0;
	char arguments[MAX_ARGUMENTS_LEN + 1];
	help_description_t *help_description = (help_description_t*)help;
	static cmd_args_t param = {
		.e_action_list = NONE,
		.p_repo_name = NULL
	};

	memset(arguments, 0, sizeof(arguments));
	strncpy(arguments, DEFAULT_ARGS, MAX_ARGUMENTS_LEN);
	while ((option = getopt_long(argc, argv, arguments, longopts,
	                             (int *) 0)) != EOF)
	{
		switch (option)
		{
			case 'c':
				param.e_action_list = CREATE_REPO;
				param.p_repo_name = optarg;
			break;
			case 'd':
				param.e_action_list = DELETE_REPO;
				param.p_repo_name = optarg;
			break;
			case 'l':
				param.e_action_list = LIST_REPO;
			break;
			case 'm':
				param.e_action_list = LIST_MR;
				param.p_repo_name = optarg;
			break;
			case 'h':
			case '?':
			default:
				help_usage(argv[0], longopts, help);
			break;
		}
	}

	if (NONE == param.e_action_list && !param.p_repo_name)
	{
		help_usage(argv[0], longopts, help_description);
		system_free((void**)&help_description);
	}

	return &param;
}
/******************************************************************************/
