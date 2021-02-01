#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

#define READ 0
#define WRITE 1

int ex1(){
    pid_t pid;
    int fd[2], status;
    char buffer[1024];
    char msg[14] = "Hello, World!";

    if( pipe(fd) < 0 ){
        perror("Error creating pipe!");
        return 1;
    }

    pid = fork();
    if( pid < 0 ){
        perror("Error creating child process!");
        return 1;
    }
    if( !pid ){
        close(fd[WRITE]);
        read(fd[READ],buffer,1024);
        printf("--- Child process (%d) ---\n",getpid());
        printf("Msg : %s\n",buffer);
        close(fd[READ]);
        _exit(0);
    }
    else{
        close(fd[READ]);
        sleep(5);
        write(fd[WRITE],msg,strlen(msg));
        wait(&status);
        close(fd[WRITE]);
        printf("--- Parent process (%d) ---\n",getpid());
        if( WIFEXITED(status) )
            printf("Child process (%d) ended with sucess!\n",pid);
        else 
            printf("Error in child process (%d)!\n",pid);
    }
    return 0;
}


int ex2(){
    pid_t pid;
    int fd[2], status, n;
    char buffer[1024];

    if( pipe(fd) < 0 ){
        perror("Error creating pipe!");
        return 1;
    }
    pid = fork();
    if( pid < 0 ){
        perror("Error creating child process!");
        return 1;
    }
    if( !pid ){
        close(fd[WRITE]);
        while( (n = read(fd[READ],buffer,1024)) > 0 ){
            printf("--- Child process (%d) ---\n",getpid());
            printf("Msg : %s\n",buffer);
        }
        close(fd[READ]);
        _exit(0);
    }
    else{
        close(fd[READ]);
        while( (n = read(0,buffer,1024)) > 0 )
                write(fd[WRITE],buffer,n);
        close(fd[WRITE]);
        wait(&status);
        printf("--- Parent process (%d) ---\n",getpid());
        if( WIFEXITED(status) )
            printf("Child process (%d) ended with sucess!\n",pid);
        else 
            printf("Error in child process (%d)!\n",pid);
    }
    return 0;
}