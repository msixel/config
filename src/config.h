/*
 * config.h
 *
 *  Created on: Aug 12, 2019
 *      Author: marcos
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdlib.h>

#define CONFIG_MAX_LINE_LEN 255
#define CONFIG_ROOT_NAME ""
#define CONFIG_PATH_SEPARATOR "/"
#define CONFIG_ERR_MESG_HEAP_FAIL "failed to allocate memory on heap"
#define CONFIG_ERR_MESG_CONF_FILE_NOT_FOUND "configuration file not found [%s]\n"
#define CONFIG_ERR_MESG_CONF_INVALID_INDENTATION "invalid configuration file. incorrect indentation."

struct CONFIG_NODE;
typedef struct CONFIG_NODE CONFIG_NODE;
struct CONFIG_NODE {
	char* name;
	char* path;
	char** values;
	short values_count;
	CONFIG_NODE *parent;
	CONFIG_NODE **children;
	short children_count;
	short level;
};

CONFIG_NODE* load_config(const char*);

CONFIG_NODE* find_node_by_path(const CONFIG_NODE*, const char*);

void free_config(CONFIG_NODE*);

#endif /* CONFIG_H_ */
