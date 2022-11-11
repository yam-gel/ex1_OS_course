#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "hw1_315393462.h"

void execute_command();

void hw1shell$()
{
    char command[MAX_COMMAND_LENGTH];
    pid_t pid;
    int child_state;
    while (1)
    {
        printf("hw1shell$ ");
        fgets(command, MAX_COMMAND_LENGTH, stdin);
        //removing the trailing '\n' from the string
        command[strcspn(command, "\n")] = 0;
    
        pid = fork();

        if (pid == 0)
        {
            printf("child process, pid = %u\n",getpid());

            execute_command(command);    
        }

        else
        {  
            if (waitpid(pid, &child_state, 0) > 0)
            {
                printf("hw1shell: pid %d finished\n", pid);
                if (!strcmp(command, "exit"))
                    return 2;
            }
        }

    }
}


void change_dir(char* command) 
{
    char* path = strtok(command,"cd ");
    //printf("substring is: %s\n", path);
    chdir(path);
    //printf("cwd is: %s\n", getcwd(path, 100));
    if (chdir(path) == -1)
        printf("invalid command\n");
}

void backround_job(char* command)
{
    printf("not implemented yet\n");
}

void execute_command(char* command) //linux commands are executed here (ls, cd, etc...)
{
    //if (!strcmp("&", command[strlen(command)-1])) //check if job goes to bg
    //{
    //    backround_job(command); //function is not implemented yet
    //}

    if (strstr(command, "ls")) //TODO: add "ls -l" option
    {
        execlp("/bin/ls", "/bin/ls",NULL);
    }
            

    else if (strstr(command, "cd"))
    {
        change_dir(command); 
    }

    else if (strstr(command, "pwd"))
    {
        printf("%s\n", getcwd(command, 100));
    }
}



int main(int argc, int argv)
{
    hw1shell$();
    return 0;
}
