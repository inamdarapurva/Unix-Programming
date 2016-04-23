#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX 250
typedef struct jobs{   //for storing jobs
	int status;
	char name[100];
	int id;
}jobs;
char *exitCommand = "exit\n";
char *listCommand = "list\n";

int main (int argc, char *argv [ ] )
{
	printf("It's a custom shell\n");
	printf("To quit shell type exit or Exit\n");
	int i,j;
	int m=0,n=0;
	pid_t pid;
	jobs job[100];
	while (1) {

		char * command; //get command from user

		// split command with space
		char* tokenArray[256] = {NULL}; 
		int totalToken = 0; //  track of total no. of tokens
		char *pipeArray[256]={NULL};
		int pCommand =0;
		char *semicolonArray[256]={NULL};
		int colonCommand=0;
		char *redirectToken[256]={NULL};
		int redirectCommand=0;
		char* backgroundArray[256]={NULL};
		int backCount=0;
		char *colonArray[256]={NULL};
		int colonCount=0;
		char* firstArray[256]={NULL};
		int firstCount=0;
		int status;
		int x;
		printf("Shell%%\n ");
		fgets (command, sizeof(command), stdin); // Accepting command from user 

		char *cinput = malloc (strlen(command) + 1);
		strcpy (cinput,command); 
		char *input2 =malloc(strlen(command)+1);//input to handle background processes
		strcpy(input2,command);
		if(strstr(cinput,"|")!=NULL) // if command contains pipe
		{
			printf("Contains pipe\n");
			char *pipeTokens =strtok(cinput,"|");//split the command on pipe
			while(pipeTokens != NULL)
			{
				pipeArray[pCommand]=strdup(pipeTokens);
				pCommand++;
				pipeTokens= strtok(NULL,"|");
			}
			if(pCommand==2)
			{
			printf("Command::%s\n",pipeArray[0]);
			printf("Pipe\n");
			printf("Command::%s\n",pipeArray[1]);
			/******Passing all pipe commands to pipe*****/
			
			/******Implementing command execution***********/
			int pfds[2];
			pipe(pfds);
			if (!fork()) {
			    close(1);       /* close normal stdout */
			    dup(pfds[1]);   /* make stdout same as pfds[1] */
			    close(pfds[0]); /* we don't need this */
			    execlp("ls", "ls", NULL);
			} else {
			    close(0);       /* close normal stdin */
			    dup(pfds[0]);   /* make stdin same as pfds[0] */
			    close(pfds[1]); /* we don't need this */
                            execlp("grep", "grep", "out", NULL);
			   
			}
			}
			/*else
			{
			
			for(x=0;x<pCommand;x++)
			{
		          
			  system(pipeArray[x]);
			}

			}*/
		}
		else if(strstr(cinput,";")!=NULL) //if contains semicolon
		{
			printf("Contains only semicolon\n");
			char *semicolonTokens =strtok(cinput,";");//split the command on semicolon
			while(semicolonTokens != NULL)
			{
				semicolonArray[colonCommand]=strdup(semicolonTokens);
				colonCommand++;
				semicolonTokens =strtok(NULL,";");
			}
			printf("Command::%s\n",semicolonArray[0]);
			printf("Semicolon(;)\n");
			printf("Command::%s\n",semicolonArray[1]);
		}
		else if(strstr(cinput,"<")!=NULL || strstr(cinput,">")!=NULL)//if command contains redirection opeartor
		{
			printf("Contains Redirection\n");
			char delimiter[] ="<,>";
			redirectToken[m] =strtok(cinput,delimiter);
			while(redirectToken[m] !=NULL)
			{
			//printf("command[%d]:%s",m,redirectToken[m]);
				m++;
				redirectToken[m]=strtok(NULL,delimiter);
			}
			printf("Command::%s\n",redirectToken[0]);
			printf("Redirection::<\n");
			printf("Input File::%s\n",redirectToken[1]);
			printf("Redirection::>\n");
			printf("Output File::%s\n",redirectToken[2]);
		}

		else // if contains only spaces
		{
			char* tokens = strtok (cinput," "); // split the command into tokens using space as separator
			while (tokens != NULL )
			{
				tokenArray[totalToken] = strdup(tokens); 
				totalToken++;
				tokens = strtok(NULL," ");//Last token is separated
			} 
			if(strcmp(exitCommand, tokenArray[0]) == 0)
			{
				//printf("Have a great day! Shell shutting down.\n");
				exit(0);
			}
			if(strcmp(listCommand, tokenArray[0]) == 0)
			{
				system("ls");
			}
			printf("Command:%s\n",tokenArray[0]);
			printf("Options:%s\n",tokenArray[1]);
			printf("Arguments:%s\n",tokenArray[2]);
		}

		if(strstr(input2,"&")!=NULL)
		{
			char *background =strtok(input2,"&");
			while(background !=NULL)
			{
				backgroundArray[backCount]=strdup(background);
				backCount++;
				background=strtok(NULL,"&");
			}
			if(strstr(backgroundArray[0],";")!=NULL)
			{
				char* firstString;
				firstString =backgroundArray[0];
				//printf("Contains semicolon in first string\n");
				char* firstToken = strtok(firstString,";");
				while(firstToken !=NULL)
				{
					firstArray[firstCount]=strdup(firstToken);
					firstCount++;
					firstToken=strtok(NULL,";");
				}
				//printf("%s",firstArray[0]);
				//printf("%s",firstArray[1]);
				pid=fork();
				if(pid==0)//child when ;,&
				{
					printf("child\n");
					printf("%d\n",getpid());
					system(firstArray[0]);
					system(firstArray[1]);
				}
				else//parent when ;,&
				{
					wait(&status);
					printf("parent\n");
					printf("%d\n",getppid());
				}
			}
			else//only &
			{
				pid=fork();
				if(pid==0)//child when &
				{
					printf("%d\n",getpid());
					//printf("Child process\n");
					char *cmd = backgroundArray[0];
					system(cmd);
					//signal(SIGTSTP,ctrlZ);

				}
				else//parent when &
				{
					wait(&status);
					printf("%d\n",getppid());
					//sleep(1);
					//printf("Parent process\n");
				}
			}
		}
	}//end of while
	return 0;
} 

