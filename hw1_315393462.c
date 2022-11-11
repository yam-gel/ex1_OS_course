#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "hw1_315393462.h"

void execute_internal_command(char **);
void execute_external_command(char **);
void parse_string();
int is_background_command();

void hw1shell$()
{
    char command[MAX_COMMAND_LENGTH];
    char *parsed_command[MAX_WORD_LENGTH];
    pid_t pid;
    int child_state, background_command=0;
    while (1)
    {
        printf("%s", "hw1shell$ ");
        fgets(command, MAX_COMMAND_LENGTH, stdin);
        //removing the trailing '\n' from the string
        command[strcspn(command, "\n")] = 0;
        
        parse_string(command, parsed_command);

        if (!parsed_command[0])
            //input was empty
            continue;
        if (!strcmp(parsed_command[0], "cd") || !strcmp(parsed_command[0], "exit") || !strcmp(parsed_command[0], "jobs"))
        {
            // internal commands 
            execute_internal_command(parsed_command);
        }
        else
        {
            pid = fork();
            if (pid == 0)
            {
                
                printf("child process, pid = %u\n",getpid());
                execvp(parsed_command[0], parsed_command);
                return;   
            }

            else
            {  
                if (waitpid(pid, &child_state, 0) > 0)
                {
                    printf("hw1shell: pid %d finished\n", pid);
                }
            }
        }
    }
}


void change_dir(char* path)
{
    chdir(path);
    //printf("cwd is: %s\n", getcwd(path, 100));
    if (chdir(path) == -1)
        printf("invalid command\n");
}

void backround_job(char* command)
{
    printf("not implemented yet\n");
}

void execute_internal_command(char** parsed_command) //linux commands are executed here (ls, cd, etc...)
{
    //if (!strcmp("&", command[strlen(command)-1])) //check if job goes to bg
    //{
    //    backround_job(command); //function is not implemented yet
    //}

    if (!strcmp(parsed_command[0], "exit"))
    {
        exit(0);
    }

    else if (!strcmp(parsed_command[0], "cd"))
    {
        change_dir(parsed_command[1]); 
    }
}


void parse_string(char* str, char** parsed_command)
{
    int i;
  
    for (i = 0; i < MAX_COMMAND_LENGTH; i++)
    {
        parsed_command[i] = strsep(&str, " ");
  
        if (parsed_command[i] == NULL)
            break;
        if (strlen(parsed_command[i]) == 0)
            i--;
    }
}

int main(int argc, int argv)
{
    hw1shell$();
    return 0;
}
