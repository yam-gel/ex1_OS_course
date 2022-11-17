#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "hw1_315393462.h"

void execute_internal_command(char **);
void execute_external_command(char **);
int parse_string();
int is_background_command();

struct process_info
{
    int pid;
    char command[MAX_COMMAND_LENGTH];
};

void hw1shell$()
{
    char command[MAX_COMMAND_LENGTH];
    char *parsed_command[MAX_WORD_LENGTH];
    pid_t pid;
    int child_state, background_command=0;
    int background;
    struct process_info jobs[4];
    while (1)
    {
        printf("%s", "hw1shell$ ");
        fgets(command, MAX_COMMAND_LENGTH, stdin);
        //removing the trailing '\n' from the string
        command[strcspn(command, "\n")] = 0;
        
        

        if (strlen(command)==0)
             //input was empty
             continue;

        background=parse_string(command, parsed_command);

        if (!strcmp(parsed_command[0], "cd") || !strcmp(parsed_command[0], "exit"))
        {
            // internal commands 
            execute_internal_command(parsed_command);
        }
        else if (!strcmp(parsed_command[0], "jobs"))
        {
            background_jobs(parsed_command[0], jobs);
        }
        else
        {
            pid = fork();
            if (pid == 0)
            {
                //printf("child process, pid = %u\n",getpid());
                if (execvp(parsed_command[0], parsed_command) == -1)
                    printf("invalid command\n");
                return;   
            }
            
            else
            {  
                if (background)
                {
                    printf("hw1shell: pid %d started\n", pid);
                    for (int i=0; i<4; i++)
                    {
                        if (jobs[i].pid)
                        {
                            if (i==3)
                                printf("hw1shell: too many background commands running");
                        }
                        else
                        {
                            jobs[i].pid = pid;
                            strcpy(jobs[i].command, command);
                            printf("%d  %s\n", jobs[i].pid, jobs[i].command);
                            break;
                        }
                    }
                    continue;
                }
                //printf("this is the father");
                else if (waitpid(pid, &child_state, 0) > 0)
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

void background_jobs(char* command, struct process_info* jobs)
{
    for (int i=0; i<4; i++)
    {
        if (jobs[i].pid)
            printf("%d\t%s\n", jobs[i].pid, jobs[i].command);
    }
}

void execute_internal_command(char** parsed_command) //linux commands are executed here (ls, cd, etc...)
{

    printf("parsed command is:\n");

    if (!strcmp(parsed_command[0], "exit"))
    {
        exit(0);
    }

    else if (!strcmp(parsed_command[0], "cd"))
    {
        change_dir(parsed_command[1]); 
    }
}


int parse_string(char* str, char** parsed_command)
//this function is parsing the sting from user and returns a list of the words in the command, retunrs 1 for background command and 0 for foreground
{
    int i;
    
    for (i = 0; i < MAX_COMMAND_LENGTH; i++)
    {
        parsed_command[i] = strsep(&str, " ");
  
        if (parsed_command[i] == NULL)
            {
            if (strchr(parsed_command[i-1], '&'))
            //& is the last character
            {
                if (strlen(strchr(parsed_command[i-1], '&'))==1)
                    printf("background command\n");
                    parsed_command[i-1] = strtok(parsed_command[i-1], "&");
                    printf("the command is now %s\n", parsed_command[i-1]);
                    return 1;
            }
            else
                return 0;
            }
        if (strlen(parsed_command[i]) == 0)
            i--;
    }
}

int main(int argc, int argv)
{
    hw1shell$();
    return 0;
}
