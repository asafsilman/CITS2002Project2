#include "myshell.h"
#include "util.h"

#define FILE_ACCESS 0600

void run_cmd(int *exitstatus, SHELLCMD *t)
{
    pid_t  pid = fork();
    switch (pid){
        case 0 : // child process
            if (t->infile != NULL) {
                execute_infile(t);
            }
            
            if (t->outfile != NULL ) {
                execute_outfile(t);
            }

            if (t->argv[0][0] == '/' || t->argv[0][0] == '.'){
                // Path Given
                execv(t->argv[0], t->argv); // attempt to start process
                if(run_shellscript(t->argv)) {break;} // attempt to start shellscript
            }
            else{
                // Search path for command
                search_path_run(t->argv);
            }
            // If execv failed, and process image did not change
            fprintf(stderr, "%s is not a valid command\n", t->argv[0]);
            exit(EXIT_FAILURE);
            break;
        case -1 : //fork failed
            *exitstatus	= EXIT_FAILURE;
            break;
        default : // parent process
            wait( exitstatus ); //exit status is written once child terminates
            // CAN PRINT THE EXIT CODE HERE
            *exitstatus=  WEXITSTATUS(*exitstatus);
			break;
    }
}

void search_path_run(char **argv)
{
    char *path_p = getenv("PATH");
    const char s[2] = ":"; //seperator
    char *token;
    char command_buffer[256]; // Buffer to hold command

    char *command = calloc(strlen(argv[0])+2, sizeof(char));
    strcat(command, "/"); 
    strcat(command, argv[0]); 

    token = strtok(path_p, s);
    // iterate through PATH variable
    while(token != NULL)
    {
        strcpy(command_buffer, token);
        strcat(command_buffer, command);
        execv(command_buffer, argv); //if this works, the process image is replaced. execution of loop will stop
        token = strtok(NULL, s);
    }

    free(command);
}

// Redirects standard input to file specified in SHELLCMD
void execute_infile(SHELLCMD *t)
{
    int ifd = open(t->infile, O_RDONLY);
    dup2(ifd, STDIN_FILENO);
    close(ifd);
}

void execute_outfile(SHELLCMD *t)
{
    /* This function works for appending, for truncating file data 
       and for creating a new file if did not previously exist */
       
    int out;
    if (t->append) {
        out = open(t->outfile, O_WRONLY|O_CREAT|O_APPEND, FILE_ACCESS);        
    }
    else {
        out = open(t->outfile, O_WRONLY|O_CREAT|O_TRUNC, FILE_ACCESS);
    }
    dup2(out, STDOUT_FILENO);
}

bool run_shellscript(char **argv)
{
    char *script_name = argv[0];
    // SHELLCMD *t;
    pid_t  pid;
    
    int access_status = access(script_name, R_OK);
    if (access_status != 0) { return false; }
    FILE *script = fopen(script_name, "r");
    int script_fd = fileno(script);
    // printf("Running Shellscript %s Access status is %i\n", script_name, access_status);

    pid = fork();
    switch (pid){
        case 0 : // child process
            close(STDIN_FILENO);
            dup2(script_fd, STDIN_FILENO);
            execv(argv0, &argv0);
            exit(0);
        case -1 : perror("fork failed"); break; // fork error
        default : // parent process
            wait(NULL);
    }
    
    fclose(script);
    return true;
}
