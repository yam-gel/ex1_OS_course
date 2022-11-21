#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "hw1_315393462_321014763.h"


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

int main(int argc, int argv)
{
    hw1shell$();
    return 0;
}
