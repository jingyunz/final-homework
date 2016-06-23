#include "exec.h"
#include "globals.h"
#include "build_in.h"
#include "errorprocess.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

void executeCommand()
{
	int i, status;
	int ParaNum;
	int isBackgroupd = 0;
	int inRedirect = 0;
	int outRedirect = 0;
	int outRedirectAppend = 0;
	int isPipe = 0;
	char *in_file;
	char *out_file;
	char *command = commands;
	char **parameters;
	char *command2;
	char **parameters2;


	char *arg_temp[MAXTOKENLEN + 1]; /* for arg array */
	if(arg[1][0] == 0)
	{
		//		execlp(commands, commands, (char *)0);
		//		return;
		ParaNum = 1;
		arg_temp[0] = arg[0];
	}
	else
	{
		for (i = 0; strcmp(arg[i], "#") != 0; i++)
		{
	//		printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
			arg_temp[i] = arg[i];
	//		printf("arg %d: %s\n", i, arg[i]);
		}

		ParaNum = i;
	}
	arg_temp[ParaNum] = NULL;
	parameters = &arg_temp[0];

	//printf("test: %s\n", arg[0]);
	//printf("len: %d\n", ParaNum);

	if(strcmp(parameters[ParaNum-1],"&") ==0)
	{
		isBackgroupd = 1;
		parameters[ParaNum-1] = NULL;
		ParaNum--;
	}

	for(i=0;i<ParaNum;)
	{
		if(strcmp(parameters[i],"<<")==0 || strcmp(parameters[i],"<")==0)
		{
			inRedirect = 1;
			in_file = parameters[i+1];
			parameters[i] = NULL;
			i+=2;
		}
		else if(strcmp(parameters[i],">")==0)
		{
			outRedirect = 1;
			out_file = parameters[i+1];
			parameters[i] = NULL;
			i+=2;
		}
		else if(strcmp(parameters[i],">>")==0)
		{
			outRedirectAppend = 1;
			out_file = parameters[i+1];
			parameters[i] = NULL;
			i+=2;
		}
		else if(strcmp(parameters[i],"|")==0)
		{
			char* pCh;
			isPipe = 1;
			parameters[i] = NULL;
			command2 = parameters[i+1];
			parameters2 = &parameters[i+1];
			for(pCh = parameters2[0]+strlen(parameters2[0]);
					pCh!=&(parameters2[0][0]) && *pCh!='/';pCh--)
				;
			if(*pCh == '/')
				pCh++;
			parameters2[0] = pCh;
			break;
		}
		else
		{
			i++;
		}
	}

	int pipe_fd[2], in_fd, out_fd;
	pid_t ChdPid, ChdPid2;
	if(isPipe) //command2 is not null
	{                
		if(pipe(pipe_fd)<0)
		{
			printf("Wshell error:pipe failed.\n");
			exit(0);
		}
	}  
	if((ChdPid = fork())!=0) //wshell
	{
		if(isPipe)
		{
			if((ChdPid2=fork()) == 0) //command2
			{
				close(pipe_fd[1]);
				close(fileno(stdin)); 
				dup2(pipe_fd[0], fileno(stdin));
				close(pipe_fd[0]); 
				execvp(command2, parameters2);
			}
			else
			{
				close(pipe_fd[0]);
				close(pipe_fd[1]);
				waitpid(ChdPid2,&status,0); //wait command2
			}
		}

		waitpid(ChdPid,&status,0);//wait command1

		/*
		   if(isBackgroupd)
		   {
		   printf("Child pid:%u\n",ChdPid);
		   int i;
		   for(i=0;i<MAXPIDTABLE;i++)
		   if(BPTable[i]==0)
		   BPTable[i] = ChdPid; //register a background process
		   if(i==MAXPIDTABLE)
		   perror("Too much background processes\nThere will be zombine process");                    
		   }
		   else
		   {          
		   waitpid(ChdPid,&status,0);//wait command1
		   } 
		   */
	}
	else //command1
	{

		if(isPipe) //command2 is not null
		{                
			if(!(outRedirect) && !(outRedirectAppend)) // ONLY PIPED
			{
				close(pipe_fd[0]);
				close(fileno(stdout)); 
				dup2(pipe_fd[1], fileno(stdout));
				close(pipe_fd[1]);
			}
			else //OUT_REDIRECT and PIPED
			{
				close(pipe_fd[0]);
				close(pipe_fd[1]);//send a EOF to command2
				if(outRedirect)
					out_fd = open(out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
				else
					out_fd = open(out_file, O_WRONLY|O_APPEND|O_TRUNC, 0666);
				close(fileno(stdout)); 
				dup2(out_fd, fileno(stdout));
				close(out_fd);	        
			}
		}
		else
		{
			if(outRedirect) // OUT_REDIRECT WITHOUT PIPE
			{
				out_fd = open(out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
				close(fileno(stdout)); 
				dup2(out_fd, fileno(stdout));
				close(out_fd);
			}
			if(outRedirectAppend) // OUT_REDIRECT_APPEND WITHOUT PIPE
			{
				out_fd = open(out_file, O_WRONLY|O_CREAT|O_APPEND, 0666);
				close(fileno(stdout)); 
				dup2(out_fd, fileno(stdout));
				close(out_fd);
			}
		}

		if(inRedirect)
		{
			in_fd = open(in_file, O_CREAT |O_RDONLY, 0666);
			close(fileno(stdin)); 
			dup2(in_fd, fileno(stdin));
			close(in_fd); 
		}
		execvp(command, parameters);
	}

}

void forktoexec()
{
	BuildInType command;              /* holds the build in command type */

	if ( (command = isbuildin(tokenString)) )
	{
		runbuildin(command);
	}
	else  /* not build-in commands */
	{
		//printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
		executeCommand();
		//printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);

		/*
		   int i;
		   pid_t pid;
		   int   status;                
		   char  *arg_temp[MAXTOKENLEN + 1]; 
		   if ((pid = fork()) < 0)
		   fprintf(stderr, "fork error...");
		   else if (pid == 0)
		   {
		   if (arg[1][0] != 0)
		   {
		   printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
		   for (i = 0; strcmp(arg[i], "#") != 0; i++)
		   arg_temp[i] = arg[i];
		   arg_temp[i] = NULL;

		   printf("len: %d\n", i);
		   return;

		   execvp(commands, arg_temp);
		   }
		   else
		   {
		   printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
		   return;
		   execlp(commands, commands, (char *)0);
		   }
		   }
		   */

		/* error process */
		error_process();

		return ;
	}
}

