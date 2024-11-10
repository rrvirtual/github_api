#ifndef PARAMS_H_
#define PARAMS_H_

#include <stdbool.h>
#include <stdint.h>

/************ DATA TYPES, CONSTANTS, MACROS ***********************************/
typedef enum {
	NONE,
	LIST_REPO,
	LIST_MR,
	CREATE_REPO,
	DELETE_REPO
} type_action_e;

typedef struct param_s {
	char          *p_repo_name;
	type_action_e  e_action_list;
} cmd_args_t;
/******************************************************************************/


/************ PUBLIC FUNCTIONS PROTOTYPES ************************************/
cmd_args_t
*params_handle(int argc, char **argv);
/******************************************************************************/

#endif //PARAMS_H_