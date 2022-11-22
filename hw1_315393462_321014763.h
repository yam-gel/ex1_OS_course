#define MAX_COMMAND_LENGTH  100
#define MAX_WORD_LENGTH 50

typedef struct process_info
{
    int pid;
    char command[MAX_COMMAND_LENGTH];
}process_info;



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

void check_zombies(process_info* jobs) //checks if there are backround jobs that finished running
{
    pid_t pid;
    
    while((pid = waitpid(-1,NULL,WNOHANG))>0){
        for (int i = 0; i<4; i++){
            if (jobs[i].pid == pid){
                printf("hw1shell: pid %d finished\n", pid);
                //printf("pid is: %d\n", jobs[i].pid);
                jobs[i].pid = 0;
                strcpy(jobs[i].command, "\0"); 
            }

        }
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
