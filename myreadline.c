#include "globals.h"
#include "myreadline.h"
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

/* for read buf */
static char *line_read = (char *)0;
/* for commands auto-completion */
static char *commandarray[] = {"login", "shutdown", "halt", "reboot", "install", "mount",
		           "umount", "chsh", "exit", "last", "file", "mkdir", "grep",
                           "dd", "find", "mv", "ls", "dif", "cat", "ln", "df", "top",
                           "free", "quota", "at", "lp", "adduser", "groupadd", "kill",
                           "kill", "crontab", "ifconfig", "ip", "ping", "netstat",
                           "telnet", "ftp", "route", "rlogin", "rcp", "finger", "mail",
                           "nslookup", "passwd", "su", "umask", "chgrp", "chmod","chown",
                           "chattr", "sudo", "ps", "tar", "unzip", "gunzip", "unarj",
                           "mtools", "man", "unendcode", "uudecode", "which", "who", "echo", (char*)NULL};

char *rl_gets()
{
	if (line_read){
		free(line_read);
		line_read = (char *)0;
	}

	char prompt[50];
	sprintf(prompt, "[jingyun %s]$ ", strrchr(pwd, '/') + 1);
	line_read = readline(prompt);

	if (line_read && *line_read)
		add_history(line_read); /* add command to history */
	return line_read;
}

char *command_generator(const char *text, int state)
{
	const char *name;
	static int list_index;
	static int len;

	if (!state){
		list_index = 0;
		len = strlen(text);
	}

	while ( (name = commandarray[list_index++]) )
	{
		//printf("state: %d, len: %d, text: %s, %s\n", state, len, text, commandarray[list_index-1]);
		if (strncmp(name, text, len) == 0)
			return strdup(name);
	}

	return (char *)0;
}

char **command_completion(const char *text, int start, int end)
{
	char **matches = NULL;

	if (start == 0)
		matches = rl_completion_matches(text, command_generator);

	return matches;
}

void initialize_readline()
{
	rl_readline_name = "jysh";
	rl_attempted_completion_function = command_completion;
}

