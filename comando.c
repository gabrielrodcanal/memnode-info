#include "comando.h" 
#include "memnode_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *f;

int main(int argc, char * argv[]) {
	int i, flag;
	char procname[50];
	char options[NR_OPT]; /* used to sort the options */

	if(argc < 2 || argv[1][0] != '-') {
		printf("memnode_info -option(s)\n");
		exit(-1);
	}
	strcpy(procname, "/proc/");
	strcat(procname, PROC_NAME);
	if((f = fopen(procname, "r")) == NULL) {
		printf("Check the module %s is loaded\n", PROC_NAME);
		exit(-2);
	}
	
	argv++; /* argv[1] */
	(*argv)++; /* argv[1][1], skip '-' */
	flag = 1;

	while(flag) {
		switch(**argv) {	/* sorting */
			case 'n':
				options[0] = 'n';
				break;
			case 'z':
				options[1] = 'z';
				break;
			case 'p':
				options[2] = 'p';
				break;
			case 'h':
				options[3] = 'h';
				/* deleting other options. Help option always
				   alone */
				for(i = 0; i < NR_OPT-1; i++)
					options[i] = 0;
				flag = 0;
				break;
			default:
				printf("Wrong option\n");
				exit(-3);
		}
		(*argv)++;
		if(**argv == '\0')
			flag = 0;
	}
	
	for(i = 0; i < NR_OPT; i++) { 
		show_info(options[i]);
	}
	
	fclose(f);
	
	return 0;
}

void show_info(char opt) {
	int i; 
	char *line = NULL;
	size_t len = 0;
	fseek(f, 0, SEEK_SET);
	switch(opt) {
		case 'n':
			read_from_to(NODE_INFO, ZONE_INFO);
			break;
		case 'z':
			read_from_to(ZONE_INFO, PAGES_INFO);
			break;
		case 'p':
			read_from_to(PAGES_INFO, NULL);
			break;
		case 'h':
			printf("%s\n", HELP_MSG);
		case 0:
			break;
	}
}	

void read_from_to(char *from, char *to) {
	char *line;
	size_t len;
	getline(&line, &len, f);
	while(strcmp(line, from) != 0) {
		getline(&line, &len, f);
	}
	printf("%s", line);
	if(to == NULL) {
		while(getline(&line, &len, f) != -1) {
			printf("%s", line);
		}
	}
	else {
		getline(&line, &len, f);
		while(strcmp(line, to) != 0) {
			printf("%s", line);
			getline(&line, &len, f);
		}
	}
	free(line);
}

