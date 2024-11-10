/** @file help.c
* @mainpage Generic library to implement help
*
*  This API module implement generic function to help usage program
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

/************ PUBLIC FUNCTIONS PROTOTYPES ************************************/
/*!
 * @brief function to implement help usage
 * @param[in] progname pointer as String
 * @param[in] longopts as struct option
 * @param[in] help help_description_t
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
