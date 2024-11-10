
/** @file system.c
* @mainpage Generic library to implement system features
*
*  This API module implement generic function to handle the system
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Rodrigo Junior.
* All rights reserved.
*/

#include <stdlib.h>
#include "system.h"

/************ PUBLIC FUNCTIONS PROTOTYPES ************************************/
/*!
 * @brief generic function to free a pointer dynamically allocated (malloc).
 * @param[in] **p pointer address
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
