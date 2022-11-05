#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "hw1_315393462.h"





int main(int argc, int argv)
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
        // currently for every input the execution is of "ls"
        printf("child process, pid = %u\n",getpid());
        
        execlp("/bin/ls", "/bin/ls",NULL);
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
    return 0;
}
