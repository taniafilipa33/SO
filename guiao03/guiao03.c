#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int ex1(){
    execlp("ls","ls","-l",NULL);
    
    // Não é impresso pois o programa é subsituido pelo ls
    printf("Hello, world!\n");

    return 0;
}

int ex2(){
    pid_t pid;
    int status;

    if( !(pid = fork()) ){
        execl("/bin/ls", "/bin/ls", "-l", NULL);
        _exit(0);
    }
    else{
        wait(&status);
        if( WIFEXITED(status) )
            puts("Sucess");
        else 
            puts("Child error");
    }
    return 0;
}

int ex3(int argc, char** argv){
    for(int i = 0; i < argc; i++)
        printf("%s\n",argv[i]);

    return 0;
}

int ex4(int argc, char** argv){
    pid_t pid;

    if( !(pid = fork()) ){
        strcpy(argv[0], "dif");
        execvp("./ex3",argv);
        for(int i = 0; i < argc; i++)
        printf("%s\n",argv[i]);
        _exit(0);
    }
    else
        wait(NULL);

    for(int i = 0; i < argc; i++)
        printf("%s\n",argv[i]);
    return 0;
}

int ex5(int argc, char** argv){
    pid_t pid[argc - 1];
    int i, status;

    for(i = 0; i < argc - 1; i++)
        if( !(pid[i] = fork()) ){
            execlp(argv[i + 1], argv[i + 1], NULL);
            _exit(0);
        }
    for(i = 0; i < argc - 1; i++)
        wait(NULL);
        if( !WIFEXITED(status) )
            perror("Child process error!");

	return 0;
}

////////////////////exercicio6

static char** split(char* command){
    int i = 0;
    char** argv = malloc(sizeof(char*) * 100);

    char* c = strtok(command, " ");
    while( c ){
        c = strtok(NULL," "); 
        argv[i++] = strdup(c); 
    }
    argv[i] = 0;

    return argv;
}

int sys(char* command){
    int status;
    pid_t pid;
    char** argv = split(command);

    if( !(pid = fork()) ){
        execlp(argv[0],argv[0],argv + 1);
        _exit(-1);
    }
    else
        wait(&status);
    
    return (WEXITSTATUS(status) == -1) ? -1 : 0; 
}

int main(){
   /* por acabar */ 

	return 0;
}



//exercicios adicionais!

///////////// exercicio 1

char** words(char* in, int* nr_words){
    int i = 0, size = 10;
    char** ws = malloc(sizeof(char*) * size + 1);
    char* w;

    w = strtok(in," ");
    while( w ){
        if( i == size ){
            size *= 2;
            ws = realloc(ws, sizeof(char*) * size);
        }
        ws[i++] = strdup(w);
        w = strtok(NULL," ");
    }
    ws[i] = 0;
    *nr_words = i;

    return ws;
}

void freeWords(char** words, int nr_words){
    for(int i = 0; i < nr_words; i++)
        free(words[i]);
    free(words);
}

void printWords(char** words, int nr_words){
    for(int i = 0; i < nr_words; i++)
        printf("%s\n",words[i]);
}

void execute(char** argv, int argc, int background){
    pid_t pid;

    switch( background ){
        case 0 : pid = fork();
                 if( !pid ){
                     execvp(argv[0],argv);
                     printf("Comando Inválido!\n");
                     _exit(1);
                 }
                 else
                     wait(NULL);
                 break; 
        case 1 : pid = fork();
                 if( !pid ){
                     execvp(argv[0],argv);
                     printf("Comando Inválido!\n");
                     _exit(1);
                 }
                 break;
    }
}

int runBackground(char* word){
    int b = 0;
    if(word[strlen(word) - 1] == '&'){
        word[strlen(word) - 1] = 0;
        b = 1;
    }
    return b;
}

int main(int argc, char** argv){
    char** ws;
    int nr_words, background, n = 1;
    char b[1024];
    char prompt[9] = "bash >> ";

    while( 1 ){
        write(1,prompt,9);
        n = readln(0,b,1024);
        if( !strcmp("quit",b) )
            break;
        ws = words(b,&nr_words);
        background = runBackground(ws[nr_words - 1]);
        execute(ws,nr_words,background);
        freeWords(ws,nr_words);
    }
    return 0;
}
///////////////////////////


////////////////////