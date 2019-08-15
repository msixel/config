/*
 ============================================================================
 Name        : main.c
 Author      : Marcos Farias Sixel
 Version     :
 Copyright   : Your copyright notice
 Description : Config test
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "config.h"

void print_hierarchy(CONFIG_NODE* node)
{
	int i;

	if (node==NULL)
	{
		fprintf(stderr, "NODE has null value");
		return;
	}

	fprintf(stdout, "[level %d] %s [", node->level, node->path==NULL?"":node->path);
	for (i=0; i<node->values_count; i++)
		fprintf(stdout, "%s ", node->values[i]);
	fputs("]\n", stdout);

	for (i=0; i<node->children_count; i++)
		print_hierarchy(node->children[i]);
}

int main(int argc, char** argv) {
	CONFIG_NODE *root_node;

	if ((root_node=load_config("/home/marcos/eclipse-cpp-workspace/config/resources/test.conf"))==NULL)
		return EXIT_FAILURE;

	puts("printing...");
	print_hierarchy(root_node);

	CONFIG_NODE *tmp;
	tmp = find_node_by_path(root_node, (char*)"/secao2");
	puts("printing...");
	print_hierarchy(tmp);

	tmp = find_node_by_path(root_node, (char*)"/desconhecida");
	puts("printing...");
	print_hierarchy(tmp);

	tmp = find_node_by_path(root_node, (char*)"/secao2/entrada3/subentrada1");
	puts("printing...");
	print_hierarchy(tmp);

	puts("end");
	free_config(root_node);
	return EXIT_SUCCESS;
}
