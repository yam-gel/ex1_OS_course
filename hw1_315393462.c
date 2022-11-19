#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "hw1_315393462.h"

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
void check_zombies(pid_t, int , process_info* );

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
        //strcpy(jobs[j].command, 0);
    }


    while (1)
    {
        check_zombies(pid,child_state, jobs);
        printf("%s", "hw1shell$ ");
        fgets(command, MAX_COMMAND_LENGTH, stdin);
        //removing the trailing '\n' from the string
        command[strcspn(command, "\n")] = 0;
        char bg_com[MAX_COMMAND_LENGTH]; 
        strcpy(bg_com, command);

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
                {
                    print_errors(11, NULL, 0);
                    print_errors(13, parsed_command[0], -1);
                }
                    
                return;   
            }
            
            else
            {  
                if (background) //TODO, Valeria: check that child processes are not terminated and still running on bg, using 
                {               //waitpid & WNOHANG
                    printf("hw1shell: pid %d started\n", pid);
                    //check_zombies(pid, child_state, jobs);
                    for (int i=0; i<5; i++)
                    {
                        if (jobs[i].pid)
                        {
                            if (i==4){
                                printf("hw1shell: too many background commands running\n");
                                break;
                            }
                                
                        }
                        else
                        {
                            strtok(bg_com, "&");
                            jobs[i].pid = pid;
                            strcpy(jobs[i].command, bg_com);
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

                else if (waitpid(pid, &child_state, 0) == -1)
                {
                    print_errors(13, parsed_command[0], -1);
                }

                check_zombies(pid, child_state, jobs);
            }
        }
        
    }
}

void check_zombies(pid_t pid, int child_state, process_info* jobs)
{
    //printf("hw1shell: pid %d started\n", pid);

    if (waitpid(pid , &child_state, WNOHANG) != 0)
    {
        //printf("found zombie process %d\n", pid);
        for (int i = 0; i<4; i++)
        {
            if (jobs[i].pid == pid){
                jobs[i].pid = 0;
                strcpy(jobs[i].command, "\0"); 
            }
        }

        //printf("hw1shell: pid %d finished\n", pid);                
    }
}

void print_errors(int error, char* system_call, int errno)
{
    if (error == 11)
    {
        printf("hw1shell: invalid command\n");
    }

    if (error == 13)
    {
        printf("hw1shell: %s failed, errno is %d\n", system_call, errno);
    }
}
void change_dir(char* path)
{
    int i = chdir(path); 

    if ( i == -1)
        print_errors(11, NULL, 0);
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

    if (!strcmp(parsed_command[0], "exit")) //TODO: need to finish all child processes, and if nedded, free allocated memory 
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
                    printf("the command is now %s\n", parsed_command[i-1]); //is it needed? 
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
