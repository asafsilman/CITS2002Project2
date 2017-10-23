#include "myshell.h"
#include "util.h"


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

        if (*exitstatus == 0){ // 
            int execution_time = end_time.tv_usec - start_time.tv_usec;
            fprintf(stderr, "%i ms\n", execution_time/1000); // convert to milliseconds
        }
        *exitstatus = 0;
    }
    else {				// normal, exit commands
        run_cmd(exitstatus, t);
    }
}

void execute_semicolon_command(SHELLCMD *t, int *exitstatus){

}
