#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Read a user id and password from standard input, 
   - create a new process to run the validate program
   - use exec (probably execlp) to load the validate program.
   - send 'validate' the user id and password on a pipe, 
   - print a message 
        "Password verified" if the user id and password matched, 
        "Invalid password", or 
        "No such user"
     depending on the return value of 'validate'.

Setting the character arrays to have a capacity of 256 when we are only
expecting to get 10 bytes in each is a cheesy way of preventing most
overflow problems.
*/

#define MAXLINE 256
#define MAXPASSWD 10

void strip(char *str, int capacity) {
    char *ptr;
    if((ptr = strchr(str, '\n')) == NULL) {
        str[capacity - 1] = '\0';
    } else {
        *ptr = '\0';
    }
}


int main(void) {
    char userid[MAXLINE];
    char password[MAXLINE];

    /* Read a user id and password from stdin */
    printf("User id:\n");
    if((fgets(userid, MAXLINE, stdin)) == NULL) {
        fprintf(stderr, "Could not read from stdin\n"); 
        exit(1);
    }
    strip(userid, MAXPASSWD);

    printf("Password:\n");
    if((fgets(password, MAXLINE, stdin)) == NULL) {
        fprintf(stderr, "Could not read from stdin\n"); 
        exit(1);
    }
    strip(password, MAXPASSWD);
    
    int fd[2];
    int excode=3;
    
    int result;
    if((result = pipe(fd)) == -1) {
        perror("pipe");
        exit(1);
    }
    
    int r;
    if((r = fork()) == -1) {
        perror("fork");
        exit(1);
        
    } else if (r > 0) { //parent
        // Parent will write to child
        close(fd[0]);

        if(write(fd[1], userid, MAXPASSWD) == -1){
            perror("write to pipe");
        }
        
        if ( write(fd[1], password, MAXPASSWD) == -1) {
            perror("write to pipe");
        }
        
        //printf("[%d] finished writing\n", getpid());
        
        close(fd[1]);
        //printf("[%d] stdin has been closed, waiting for child\n", getpid());
        
        int status;
        if(wait(&status) != -1)  {
            if(WIFEXITED(status)) {
                //printf("[%d] Child exited with %d\n", getpid(),
                       //WEXITSTATUS(status));
                excode = WEXITSTATUS(status);
            } else {
                printf("[%d] Child exited abnormally\n", getpid());
            }
        }
        
        if (excode == 0) {
            printf("Password verified\n");
        } else if(excode == 2){
            printf("Invalid password\n");
        } else if(excode == 3){
            printf("No such user\n");
        }

    } else { //child
        close(fd[1]);
        
        dup2(fd[0], STDIN_FILENO);
    
        close(fd[0]);
        
        execlp("./validate", NULL);
    }

    
    return 0;
}
