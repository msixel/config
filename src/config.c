/*
 * config.c
 *
 *  Created on: Aug 12, 2019
 *      Author: marcos
 */
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stropts.h>

int number_of_tabs (const char* line)
{
	int i, count;
	count=0;
	for (i=0; i<strlen(line); i++)
		if (line[i]=='\t')
			++count;
		else
			break;
	return count;
}

CONFIG_NODE* parse_name_and_values(CONFIG_NODE* newnode, const char* line)
{
	char *line_dup, *line_dup_ptr;

	if ((line_dup_ptr=line_dup=strdup(line))==NULL)
	{
		fputs(CONFIG_ERR_MESG_HEAP_FAIL, stderr);
		return (CONFIG_NODE*)NULL;
	}

	char *token;
	if ((token=strtok_r(line_dup_ptr, " ", &line_dup_ptr)))
	{
		newnode->name = strdup(token);
		while ((token=strtok_r(line_dup_ptr, " ", &line_dup_ptr)))
		{
			newnode->values_count++;
			if ((newnode->values = realloc(newnode->values, sizeof(char*) * newnode->values_count))==NULL)
			{
				fputs(CONFIG_ERR_MESG_HEAP_FAIL, stderr);
				return (CONFIG_NODE*)NULL;
			}
			newnode->values[newnode->values_count - 1] = strdup(token);
		}
		free(line_dup);
	}
	else
	{
		newnode->name=line_dup;
	}
	return newnode;
}

CONFIG_NODE* create_node(const char* line, const short line_len, const short level, CONFIG_NODE* parent)
{
	CONFIG_NODE *newnode;

	if ((newnode = malloc(sizeof(CONFIG_NODE)))==NULL)
	{
		fputs(CONFIG_ERR_MESG_HEAP_FAIL, stderr);
		return (CONFIG_NODE*)NULL;
	}

	newnode->level = level;

	parse_name_and_values(newnode, line+level);

	if (parent==NULL)
	{
		newnode->path = strdup(newnode->name);
	}
	else
	{
		if ((newnode->path=calloc(sizeof(char), strlen(parent->path) + strlen(CONFIG_PATH_SEPARATOR) + strlen(newnode->name)))==NULL)
		{
			fputs(CONFIG_ERR_MESG_HEAP_FAIL, stderr);
			return (CONFIG_NODE*)NULL;
		}
		strcpy(newnode->path, parent->path);
		strcat(newnode->path, CONFIG_PATH_SEPARATOR);
		strcat(newnode->path, newnode->name);

		parent->children_count++;
		if ((parent->children = realloc(parent->children, sizeof(CONFIG_NODE) * parent->children_count))==NULL)
		{
			fputs(CONFIG_ERR_MESG_HEAP_FAIL, stderr);
			return (CONFIG_NODE*)NULL;
		}
		parent->children[parent->children_count - 1] = newnode;
	}

	newnode->parent = parent;
	return newnode;
}

CONFIG_NODE* parse_line(const char* line, const short line_len, const short tabs, const short previous_tabs,
		CONFIG_NODE* previous_node)
{
	CONFIG_NODE *newnode;

	if (tabs == previous_tabs)
		newnode = create_node(line, line_len, tabs,
				previous_node->parent);
	else
		if (tabs > previous_tabs)
		{
			if (tabs > previous_tabs + 1)
			{
				fputs(CONFIG_ERR_MESG_CONF_INVALID_INDENTATION, stderr);
				return (CONFIG_NODE*)NULL;//abending due memory heap fail
			}
			newnode = create_node(line, line_len, tabs,
					previous_node);
		}
		else
		{
//			(tabs < previous_tabs)
			short tabs_decreasing;
			tabs_decreasing = previous_tabs - tabs;

			CONFIG_NODE *parent_tmp;
			parent_tmp = previous_node->parent;

			int i;
			for (i=tabs_decreasing; i>0; i--) {
				parent_tmp = parent_tmp==NULL?NULL:parent_tmp->parent;
			}

			newnode = create_node(line, line_len, tabs, parent_tmp);
		}

	return newnode;
}

CONFIG_NODE* load_config(const char* filename)
{
	CONFIG_NODE *root_node;
	FILE* file;
	char line[CONFIG_MAX_LINE_LEN];

	if ((file=fopen(filename, "r"))==NULL)
	{
		fprintf(stderr, CONFIG_ERR_MESG_CONF_FILE_NOT_FOUND, filename);
		return NULL;
	}

	if ((root_node = create_node((char*)&CONFIG_ROOT_NAME, 0, -1, NULL))==NULL)
		return root_node;//abending due memory heap fail
	root_node->parent = root_node;

	CONFIG_NODE *previous_node;
	previous_node = root_node;
	short previous_tabs=0;

	while (fgets(line, CONFIG_MAX_LINE_LEN, file)!=NULL)
	{
		CONFIG_NODE *newnode;
		short line_len;

		line_len = strlen(line);
		line[line_len - 1] = '\0';
		--line_len;
		short tabs;
		tabs = number_of_tabs(line);

		if (line[0]=='\0' ||
				line[tabs]=='\0')
			continue;

		if ((newnode=parse_line(line, line_len, tabs, previous_tabs, previous_node))==NULL)
			return (CONFIG_NODE*)NULL;//abending due memory heap fail
		previous_node = newnode;
		previous_tabs = tabs;
	}

	fclose(file);

	return root_node;
}

CONFIG_NODE* find_child_by_name(const CONFIG_NODE* node, const char* child_name)
{
	int i;
	for (i=0; i<node->children_count; i++)
		if (strcmp(node->children[i]->name, child_name)==0)
			return node->children[i];
	return (CONFIG_NODE*)NULL;
}

CONFIG_NODE* find_node_by_path(const CONFIG_NODE* node, const char* path)
{
	char *path_dup, *path_dup_ptr;
	char *token;
	CONFIG_NODE* current_node;

	if ((path_dup_ptr=path_dup=strdup(path))==NULL)
	{
		fputs(CONFIG_ERR_MESG_HEAP_FAIL, stderr);
		return (CONFIG_NODE*)NULL;
	}

	current_node = (CONFIG_NODE*)node;
	while ((token=strtok_r(path_dup, CONFIG_PATH_SEPARATOR, &path_dup)))
		if ((current_node=find_child_by_name(current_node, token))==NULL)
			break;
	free(path_dup_ptr);
	return current_node;
}

void free_config(CONFIG_NODE* node)
{
	short i;
	for (i=0; i<node->children_count; i++)
	{
		free_config(node->children[i]);
		node->children_count--;
	}
	free(node->children);

	for (i=0; i<node->values_count; i++)
	{
		free((char*)node->values[i]);
		node->values_count--;
	}
	free(node->values);
	free(node->name);
	free(node->path);
	free(node);
}
