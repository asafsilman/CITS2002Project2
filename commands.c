#include "myshell.h"
#include "util.h"

#define READ 0
#define WRITE 1

void execute_cmd_command(SHELLCMD *t, int *exitstatus)
{
    // INTERNAL COMMANDS
    if(strcmp(t->argv[0], "exit") == 0){
        // If first argument is 'exit', strcmp returns 0
        printf("\n");
        exit(EXIT_SUCCESS);
    }
    else if(strcmp(t->argv[0], "cd") == 0){
        // If first argument is 'cd', strcmp returns 0
        if(t->argc==1) {
            chdir(HOME);
        }
        else {
            chdir(t->argv[1]);
        }
        *exitstatus = 0;
    }
    else if(strcmp(t->argv[0], "time") == 0){
        // If first argument is 'exit', strcmp returns 0    
        struct timeval  start_time;
        struct timeval  end_time;
        
        // Case if time has no arguments
        if (t->argc == 1){
            *exitstatus = EXIT_SUCCESS;
            return;
        }

        (t->argv)++; // increment argv by 1
        (t->argc)--; // decrement argc by 1

        gettimeofday( &start_time, NULL );
        *exitstatus = execute_shellcmd(t);
        (t->argv)--; // increment argv by 1        
        gettimeofday( &end_time, NULL );

        if (*exitstatus == EXIT_SUCCESS){ // 
            int execution_time = end_time.tv_usec - start_time.tv_usec;
            fprintf(stderr, "%i ms\n", execution_time/1000); // convert to milliseconds
        }
        *exitstatus = EXIT_SUCCESS;
    }
    else {				// normal, exit commands
        run_cmd(exitstatus, t);
    }
}

void execute_semicolon_command(SHELLCMD *t, int *exitstatus)
{
	execute_shellcmd(t->left);
	if (t->right == NULL){
		*exitstatus = EXIT_FAILURE; return;
	}
    *exitstatus = execute_shellcmd(t->right); // Project Requirement [Step 4]
}

void execute_and_command(SHELLCMD *t, int *exitstatus)
{
	int last_exit;
    last_exit = execute_shellcmd(t->left);
    if (last_exit != EXIT_SUCCESS) {
        *exitstatus = last_exit;
        return;
    }

    *exitstatus = execute_shellcmd(t->right);
}

void execute_or_command(SHELLCMD *t, int *exitstatus)
{	
	int last_exit;
    last_exit = execute_shellcmd(t->left);
    if (last_exit == EXIT_SUCCESS) {
        *exitstatus = last_exit;
        return;
    }

    *exitstatus = execute_shellcmd(t->right);
}

void execute_subshell_command(SHELLCMD *t, int *exitstatus)
{
    // TODO stdin/out redirection
    pid_t  pid = fork();
    switch (pid){
        case 0 : // child process
            *exitstatus = execute_shellcmd(t->left); break;
        case -1 : //fork failed
            *exitstatus	= EXIT_FAILURE; break;
        default : wait(NULL); break;// parent process
    }
}

void execute_pipe_command(SHELLCMD *t, int *exitstatus)
{
    int pipefd[2];
    int prv_stdout, prv_stdin = 0;

    if (pipe(pipefd) == -1){
        perror("pipe"); // PIPE ERROR
        *exitstatus = EXIT_FAILURE; return;
    }

    switch (fork()){
        case 0 :  // Child fork
            // Save stdout, will be used to reset stdout
            prv_stdout = dup(STDOUT_FILENO);

            // Configure stdout
            close(STDOUT_FILENO);
            dup2(pipefd[WRITE], STDOUT_FILENO);
            close(pipefd[READ]);

            execute_shellcmd(t->left); // Run left command

            dup2(prv_stdout, STDOUT_FILENO); //Reset stdout
            exit(0);
        case -1 : //fork failed
            *exitstatus	= EXIT_FAILURE; break;
        default :
            wait(NULL); // Wait for child to finish
            prv_stdin = dup(STDIN_FILENO); // Save stdin

            // Configure stdin
            close(STDIN_FILENO);
            dup2(pipefd[READ], STDIN_FILENO);
            close(pipefd[WRITE]);

            *exitstatus = execute_shellcmd(t->right); // Run right command

            dup2(prv_stdin, STDIN_FILENO); //Reset stdin
    }
}