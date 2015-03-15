#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
//#include <mcheck.h>

#include "parser.h"
#include "shell.h"

/**
 * Program that simulates a simple shell.
 * The shell covers basic commands, including builtin commands 
 * (cd and exit only), standard I/O redirection and piping (|). 
 
 */

#define MAX_DIRNAME 100
#define MAX_COMMAND 1024
#define MAX_TOKEN 128

/* Functions to implement, see below after main */
int execute_cd(char** words);
int execute_nonbuiltin(simple_command *s);
int execute_simple_command(simple_command *cmd);
int execute_complex_command(command *cmd);


int main(int argc, char** argv) {
	
	char cwd[MAX_DIRNAME];           /* Current working directory */
	char command_line[MAX_COMMAND];  /* The command */
	char *tokens[MAX_TOKEN];         /* Command tokens (program name, 
					  * parameters, pipe, etc.) */

	while (1) {

		/* Display prompt */		
		getcwd(cwd, MAX_DIRNAME-1);
		printf("%s> ", cwd);

        /* Read the command line */
		fgets(command_line, MAX_COMMAND, stdin);
		/* Strip the new line character */
		if (command_line[strlen(command_line) - 1] == '\n') {
			command_line[strlen(command_line) - 1] = '\0';
		}

		/* Parse the command into tokens */
		parse_line(command_line, tokens);

		/* Check for empty command */
		if (!(*tokens)) {
			continue;
		}
		
		/* Construct chain of commands, if multiple commands */
		command *cmd = construct_command(tokens);
		//print_command(cmd, 0);
    
		int exitcode = 0;
		if (cmd->scmd) {
			exitcode = execute_simple_command(cmd->scmd);
			if (exitcode == -1) {
				break;
			}
		}
		else {
			exitcode = execute_complex_command(cmd);
			if (exitcode == -1) {
				break;
			}
		}
		release_command(cmd);
	}
    
	return 0;
}


/**
 * Changes directory to a path specified in the words argument;
 * For example: words[0] = "cd"
 *              words[1] = "csc209/assignment3/"
 * Your command should handle both relative paths to the current 
 * working directory, and absolute paths relative to root,
 * e.g., relative path:  cd csc209/assignment3/
 *       absolute path:  cd /u/bogdan/csc209/assignment3/
 */
int execute_cd(char** words) {
	
	/** 
	 * TODO:
	 * The first word contains the "cd" string, the second one contains 
	 * the path.
	 * Check possible errors:
	 * - The words pointer could be NULL, the first string or the second 
	 *   string could be NULL, or the first string is not a cd command
	 * - If so, return an EXIT_FAILURE status to indicate something is 
	 *   wrong.
	 */
    if (words == NULL || words[0] == NULL || words[1] == NULL || strncmp(words[0], "cd", 2)){
        printf("argument null\n");
        return (EXIT_FAILURE);
    }
    
	/**
	 * TODO:
	 * The safest way would be to first determine if the path is relative 
	 * or absolute (see is_relative function provided).
	 * - If it's not relative, then simply change the directory to the path 
	 * specified in the second word in the array.
	 * - If it's relative, then make sure to get the current working 
	 * directory, append the path in the second word to the current working
	 * directory and change the directory to this path.
	 * Hints: see chdir and getcwd man pages.
	 * Return the success/error code obtained when changing the directory.
	 */
    
    char cwd[1024] = ""; //Don't necessarily know which size
    
    if (is_relative(words[1]) == 1){
        //Not relative
        strcat(cwd, words[1]);
    }else {
        //Relative
        if (getcwd(cwd, sizeof(cwd)) != NULL){
            strcat(cwd, words[1]);
        }else
            perror("getcwd() error");
    }
    
    if (chdir(cwd) == 0){
        return EXIT_SUCCESS;
    } else {
        perror(cwd);
        return EXIT_FAILURE;
    }
    
}


/**
 * Executes a program, based on the tokens provided as 
 * an argument.
 * For example, "ls -l" is represented in the tokens array by 
 * 2 strings "ls" and "-l", followed by a NULL token.
 * The command "ls -l | wc -l" will contain 5 tokens, 
 * followed by a NULL token. 
 */
int execute_command(char **tokens) {
	/**
	 * TODO:
     * Execute a program, based on the tokens provided.
	 * The first token is the command name, the rest are the arguments 
	 * for the command. 
	 * Hint: see execlp/execvp man pages.
	 * 
	 * - In case of error, make sure to use "perror" to indicate the name
	 *   of the command that failed.
	 *   You do NOT have to print an identical error message to what would 
	 *   happen in bash.
	 *   If you use perror, an output like: 
	 *      my_silly_command: No such file of directory 
	 *   would suffice.
	 * Function returns only in case of a failure (EXIT_FAILURE).
	 */
    int p;
    
    if ((p = execvp(*tokens, tokens))){
        fprintf(stderr, "%s: ", *tokens);
        perror("");
    }
    
    return p;
}


/**
 * Executes a non-builtin command.
 */
int execute_nonbuiltin(simple_command *s) {
	/**
	 * TODO:
     * Check if the in, out, and err fields are set (not NULL),
	 * and, IN EACH CASE:
	 * - Open a new file descriptor (make sure you have the correct flags,
	 *   and permissions);
	 * - redirect stdin/stdout/stderr to the corresponding file.
	 *   (hint: see dup2 man pages).
	 * - close the newly opened file descriptor in the parent as well. 
	 *   (Avoid leaving the file descriptor open across an exec!) 
	 * - finally, execute the command using the tokens (see execute_command
	 *   function above).
	 * This function returns only if the execution of the program fails.
	 */
    
    if (s->in != NULL){
        printf("STDIN not null\n");
        int fd0 = open(s->in, O_RDWR);
        dup2(fd0, STDIN_FILENO);
        close(fd0);
    }
    
    if (s->out != NULL){
        printf("STDOUT not null\n");

        int fd1 = open(s->out, O_RDWR | O_APPEND | O_CREAT, S_IRWXU);
        dup2(fd1, STDOUT_FILENO);
        close(fd1);
    }
    
    if (s->err != NULL){
        printf("STDERR not null\n");

        int fd2 = open(s->err, O_RDWR | O_APPEND | O_CREAT, S_IRWXU);
        dup2(fd2, STDERR_FILENO);
        close(fd2);
    }

    return (execute_command(s->tokens));
}


/**
 * Executes a simple command (no pipes).
 */
int execute_simple_command(simple_command *cmd) {
	/**
	 * TODO: 
	 * Check if the command is builtin.
	 * 1. If it is, then handle BUILTIN_CD (see execute_cd function provided) 
	 *    and BUILTIN_EXIT (simply exit with an appropriate exit status).
	 * 2. If it isn't, then you must execute the non-builtin command. 
	 * - Fork a process to execute the nonbuiltin command 
	 *   (see execute_nonbuiltin function above).
	 * - The parent should wait for the child.
	 *   (see wait man pages).
	 */
    if (cmd->builtin > 0){
        if (cmd->builtin == BUILTIN_CD){
            execute_cd(cmd->tokens);
        }
        else if (cmd->builtin == BUILTIN_EXIT){
            exit(EXIT_SUCCESS);
        }
    }
    else {
        int r;
        if((r = fork()) == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
            
        } else if (r > 0) { //parent
            int status;
            if(wait(&status) != -1)  {
                if(WIFEXITED(status)) {
                    return (WEXITSTATUS(status));
                } else {
                    return(WEXITSTATUS(status));
                }
            }
        } else { //child
            execute_nonbuiltin(cmd);
        }
    }
    
    return 0;
}


/**
 * Executes a complex command.  A complex command is two commands chained 
 * together with a pipe operator.
 */
int execute_complex_command(command *c) {
	/**
	 * TODO:
	 * Check if this is a simple command, using the scmd field.
	 * Remember that this will be called recursively, so when you encounter
	 * a simple command you should act accordingly.
	 * Execute nonbuiltin commands only. If it's exit or cd, you should not 
	 * execute these in a piped context, so simply ignore builtin commands. 
	 */
    if (c->scmd){
        if (c->scmd->builtin != 1){
            execute_nonbuiltin(c->scmd);
        }
    }
	

	/** 
	 * Optional: if you wish to handle more than just the 
	 * pipe operator '|' (the '&&', ';' etc. operators), then 
	 * you can add more options here. 
	 */

	if (!strcmp(c->oper, "|")) {
		
		/**
		 * TODO: Create a pipe "pfd" that generates a pair of file 
		 * descriptors, to be used for communication between the 
		 * parent and the child. Make sure to check any errors in 
		 * creating the pipe.
		 */
        int pfd[2];
        
        int result;
        if((result = pipe(pfd)) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
			
		/**
		 * TODO: Fork a new process.
		 * In the child:
		 *  - close one end of the pipe pfd and close the stdout 
		 * file descriptor.
		 *  - connect the stdout to the other end of the pipe (the 
		 * one you didn't close).
		 *  - execute complex command cmd1 recursively. 
		 * In the parent: 
		 *  - fork a new process to execute cmd2 recursively.
		 *  - In child 2:
		 *     - close one end of the pipe pfd (the other one than 
		 *       the first child), and close the standard input file 
		 *       descriptor.
		 *     - connect the stdin to the other end of the pipe (the 
		 *       one you didn't close).
		 *     - execute complex command cmd2 recursively. 
		 *  - In the parent:
		 *     - close both ends of the pipe. 
		 *     - wait for both children to finish.
		 */
        int r;
        if((r = fork()) == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
            
        }
        
        else if (r > 0) { //parent1
            int q;
            
            if((q = fork()) == -1){
                perror("fork");
                exit(EXIT_FAILURE);
            }
            
            else if (q > 0){ //parent2
                int status;
                int i;

                if (close(pfd[0]) == -1){
                    perror("close");
                    exit(EXIT_FAILURE);
                }
                
                if (close(pfd[1]) == -1){
                    perror("close");
                    exit(EXIT_FAILURE);
                }

                //Wait for both children
                for(i = 0; i < 2; i++) { //2 children processes
                    
                    if (wait(&status) != -1) {
                        if(WIFEXITED(status)) {
                            //printf("[%d] Child exited with %d in parent 2\n", getpid(),
                                   //WEXITSTATUS(status));
                        } else {
                            return(EXIT_FAILURE);
                        }
                    }
                }
                
                
            }
            
            else { //child 2
                close(pfd[1]);
                
                dup2(pfd[0], STDIN_FILENO);
            
                exit(execute_complex_command(c->cmd2));
            }
        }
        
        else { //child 1
            close(pfd[0]);
            
            dup2(pfd[1], STDOUT_FILENO);

            exit(execute_complex_command(c->cmd1));
        }
		
	}
	return 0;
}
