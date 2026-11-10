
/** @file system.c
* @brief System utility functions.
*
* Provides common memory-management helpers.
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Rodrigo Junior.
* All rights reserved.
*/

#include <stdlib.h>
#include "system.h"

/************ PUBLIC FUNCTION BODIES ******************************************/
/**
 * @brief Free a heap-allocated pointer and set it to NULL.
 * @param[in,out] p Address of the pointer to free; must not be NULL.
 */
void
system_free(void **p)
{
	if (*p)
	{
		free(*p);
		*p = NULL;
	}
}
/******************************************************************************/
