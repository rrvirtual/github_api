#ifndef HELP_H_
#define HELP_H_

/************ DATA TYPES, CONSTANTS, MACROS ***********************************/
typedef struct help_description_s {
	char *description;
	char *argument;
} help_description_t;
/******************************************************************************/

/************ PUBLIC FUNCTIONS PROTOTYPES ************************************/
void
help_usage(char *progname, const struct option *longopts,
               const help_description_t *help);
/******************************************************************************/

#endif //HELP_H_