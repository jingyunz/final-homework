/*
 * src/build_in.c
 *
 * (C) 2014 HuangJinDong
 *
 */

#include "globals.h"
#include "build_in.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* toggle current directory */
void do_cd();

/* display current directory */
void do_pwd();

/* exit current shell with exit number */
void do_exit();

/* read .history which contains historical commands */
void do_history();

int isbuildin(char *s)
{
	BuildInType command = NO;

	if(!strcmp(s, "cd"))
		command = CD;
	else if (!strcmp(s, "pwd"))
		command = PWD;
	else if (!strcmp(s, "exit"))
		command = EXIT;
	else if (!strcmp(s, "history"))
		command = HISTORY;

	return command;
}

void runbuildin(BuildInType command)
{
	switch(command)
	{
		case CD:
			do_cd();
			break;
		case PWD:
			do_pwd();
			break;
		case EXIT:
			do_exit();
			break;
		case HISTORY:
			do_history();
			break;
		case NO:/* nothing */
		default:break;	
	}
}

void do_cd()
{
	/* change to the arg[1] always */
	if (arg[1][0] != 0){
		if (!chdir(arg[1]))
		{
			int len = strlen(arg[1]);
			int hasEnd = FALSE;
			if(len >= 1)
			{
				if(arg[1][len-1] == '/')
				{
					hasEnd = TRUE;
				}
			}
			if(hasEnd == FALSE)
			{
				arg[1][len] = '/';
				arg[1][len+1] = '\0';
			}
			strcpy(pwd, arg[1]); /* update the current work director */
			printf("pwd: %s\n", pwd);
		}
		else{
			/* error process */
			if (errno == ENOENT){
				fprintf(stdout, "jysh: cd: %s: No such file or directory\n", arg[1]);
				errno = 0; /* resume */
			}
			else if (errno == ENOTDIR){
				fprintf(stdout, "jysh: cd: %s: Not a directory\n", arg[1]);
				errno = 0;
			}
		}
	}
}

void do_exit()
{
	exit(atoi(arg[1])); /* exit number from arg[1] */
}

void do_pwd()
{
	fprintf(stdout, "%s\n", pwd);
}

void do_history()
{
	int  row = 1; /* line number */
	char temp[MAXTOKENLEN + 1];
	FILE *history = fopen(".history", "r");
	while (fgets(temp, MAXTOKENLEN + 1, history) != NULL){
		fprintf(stdout, "%d %s", row, temp);
		row++;
	}
	fclose(history);
}

