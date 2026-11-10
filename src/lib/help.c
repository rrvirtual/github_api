/** @file help.c
* @brief Command-line help output.
*
* Provides formatted usage and option descriptions.
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Rodrigo Junior.
* All rights reserved.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <getopt.h>
#include "help.h"

/************ DATA TYPES, CONSTANTS, MACROS ***********************************/

/******************************************************************************/

/************ PUBLIC FUNCTION BODIES ******************************************/
/**
 * @brief Print command-line usage and option descriptions, then exit.
 * @param[in] progname Program path displayed in the usage line.
 * @param[in] longopts NULL-terminated long-option table.
 * @param[in] help NULL-terminated descriptions corresponding to longopts.
 * @note This function terminates the process with EXIT_FAILURE.
 */
void
help_usage(char *progname, const struct option *longopts,
               const help_description_t *help)
{
	int i;

	fprintf(stderr, "Usage: %s [options]\nOptions:\n", basename(progname));
	for (i = 0; help[i].description; ++i)
	{
		printf("-%c or --%s %s: %s\n", longopts[i].val, longopts[i].name, 
		       help[i].argument? help[i].argument : "" , help[i].description);
	}

	exit(EXIT_FAILURE);
}
/******************************************************************************/
