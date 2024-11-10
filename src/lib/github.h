#ifndef GITHUB_H_
#define GITHUB_H_

#include "url.h"

/************ DATA TYPES, CONSTANTS, MACROS ***********************************/
#define GITHUB_USER_TOKEN	""
#define GITHUB_API_USER_URL	"https://api.github.com/user"
#define GITHUB_API_CREATE_REPO	"https://api.github.com/user/repos"
#define GITHUB_API_DELETE_REPO	"https://api.github.com/repos"
#define GITHUB_MAX_USR_LEN      39 // Github documentation on 2024/11/09

typedef struct github_conf_s {
	char		repos_url[MAX_URL_LENGHT + 1];
	char		login[GITHUB_MAX_USR_LEN + 1];
	char		*p_user_token;
} github_conf_t;
/******************************************************************************/

/************ PUBLIC FUNCTIONS PROTOTYPES ************************************/
char *github_get_user_token(void);
int github_initialize(github_conf_t *p_github_conf);
int github_get_user_info(github_conf_t *p_github_conf);
int github_get_user_repo_list(github_conf_t *p_github_conf);
int github_create_user_repo(const char *p_repo_name);
int github_delete_user_repo(const char *p_repo_name, github_conf_t *p_github_conf);
/******************************************************************************/

#endif //GITHUB_H_
