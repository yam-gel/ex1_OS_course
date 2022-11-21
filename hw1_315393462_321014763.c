#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "hw1_315393462_321014763.h"

typedef struct process_info
{
    int pid;
    char command[MAX_COMMAND_LENGTH];
}process_info;

void execute_internal_command(char **);
void execute_external_command(char **);
int parse_string();
int is_background_command();
void print_errors(int, char*, int);
void background_jobs(char*, struct process_info*);
void check_zombies(process_info* );

void hw1shell$()
{
    char command[MAX_COMMAND_LENGTH];
    char *parsed_command[MAX_WORD_LENGTH];
    pid_t pid;
    int child_state, background_command=0;
    int background;
    process_info jobs[4];

    for (int j = 0; j<4; j++)
    {
        //make sure all jobs pid are initiallized to 0
        if (jobs[j].pid != 0)
            jobs[j].pid = 0;
    }


    while (1)
    {
        check_zombies(jobs); //check if there are bg jobs that finished running and handle appropriately
        printf("%s", "hw1shell$ ");
        fgets(command, MAX_COMMAND_LENGTH, stdin);
        command[strcspn(command, "\n")] = 0; //removing the trailing '\n' from the string
        
        char bg_command[MAX_COMMAND_LENGTH]; 
        strcpy(bg_command, command);

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
            check_zombies(jobs);
            background_jobs(parsed_command[0], jobs);
        }
        else
        {
            pid = fork();
            if (pid == 0)
            {
                if (execvp(parsed_command[0], parsed_command) == -1) //check if system call fails 
                {
                    print_errors(11, NULL, 0);
                    print_errors(13, parsed_command[0], -1);
                }
                    
                return;   
            }
            
            else
            {  
                if (background) 
                {             
                    check_zombies(jobs);
                    for (int i=0; i<4; i++)
                    {
                        if (jobs[i].pid) // check if there are already 4 backroud jobs running and handle appropriately
                        {
                            if (i==3){
                                print_errors(6, NULL, 0);
                                break;
                            }
                                
                        }
                        else
                        {
                            printf("hw1shell: pid %d started\n", pid);
                            strtok(bg_command, "&");
                            jobs[i].pid = pid;
                            strcpy(jobs[i].command, bg_command);
                            break;
                        }
                    }
                    continue;
                }
                
                
                else if (waitpid(pid, &child_state, 0) == -1)
                {
                    print_errors(13, parsed_command[0], -1);
                }

            }
        }

    }
}

void check_zombies(process_info* jobs) //checks if there are backround jobs that finished running
{
    for (int i = 0; i<4; i++)
    {
        pid_t pid=jobs[i].pid;
        pid_t pid2 = waitpid(pid , NULL, WNOHANG);
        if (pid2 != 0){
            if (pid != 0)
                printf("hw1shell: pid %d finished\n", pid);
            jobs[i].pid = 0;
            strcpy(jobs[i].command, "\0"); 
        }
    }
}

void print_errors(int error, char* system_call, int errno) //error handler
{
    if (error == 6)
    {
        printf("hw1shell: too many background commands running\n");
    }

    if (error == 11)
    {
        printf("hw1shell: invalid command\n");
    }

    if (error == 13)
    {
        printf("hw1shell: %s failed, errno is %d\n", system_call, errno);
    }
}

void change_dir(char* path) //internal command "cd" execution
{
    int i = chdir(path); 

    if ( i == -1)
        print_errors(11, NULL, 0);
}

void background_jobs(char* command, struct process_info* jobs) //print backroung jobs
{
    for (int i=0; i<4; i++)
    {
        if (jobs[i].pid)
            printf("%d\t%s\n", jobs[i].pid, jobs[i].command);
    }
}

void execute_internal_command(char** parsed_command) //linux internal commands are executed here
{

    if (!strcmp(parsed_command[0], "exit"))
    {
        while(wait(NULL) > 0);
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
                    parsed_command[i-1] = strtok(parsed_command[i-1], "&");
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
