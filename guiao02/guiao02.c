#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>



int teste(){
	pid_t pd;
	int i = 2;
	if((pd=fork())==0){
		i++;

		printf("Sou filho, o meu PID é %d e o do meu pai é %d, SURPRISE: %d\n",getpid(),getppid(),i);
	}
	else{
		sleep(1);
		i--;
		printf("Sou pai, o meu PID é %d, SURPRISE: %d\n",getpid(),i);
	}
	printf("%d\n",i);
	return 0;
}

int ex1(){
    printf("PID = %d\nPPID = %d\n",getpid(),getppid());
	return 0;
}


// Nota : se nao se colocar o wait no bloco do processo pai, ha possibilidade
// do processo pai acabar primeiro que o processo filho. Neste caso diz-se que
// o processo filho ficou orfao

int ex2(){
	int status;
	pid_t pf;
	pf = fork();
	if(pf==0){
		printf("Sou o filho: %d\n O meu Pai:%d\n",getpid(),getppid());
	}
	else {
		wait(&status);
		printf("Sou o pai: %d\n O meu Pai:%d\n O mu filho:%d\n",getpid(),getppid(),pf);
	}
	return 0;
}


// Nota: a macro WEXITSTATUS permite aceder ao valor de saida de um processo filho

int ex3(){
	pid_t pf;
	int status;
	for(int i =0; i<10;i++){
		if((pf=fork())==0){
		printf("CHILD  O meu Pid:%d  O pid do meu Pai:%d   A minha ordem:%d\n",getpid(),getppid(), i);
		sleep(2);
		_exit(i);
		}
		else{
			wait(&status);
	printf("DAD A ordem so meu filho:%d\n", WEXITSTATUS(status));
	
		}
	}
	return 0;
}


// Nota: a macro WEXITSTATUS permite aceder ao valor de saida de um processo filho

int ex4(){
    pid_t pid;
    int status;

    for(int i = 0; i < 10; i++){
       if( (pid=fork())==0 ){
           // child process
           printf("PID = %d\nPPID = %d\n a minha said =%d\n",getpid(), getppid(),i);
           sleep(2);
           _exit(i);
       }
    }
    // parent is waiting for all processes end their job
    for(int i = 0; i < 10; i++){
        pid = wait(&status);
        printf("Child %d finished! Exit code %d\n", pid, WEXITSTATUS(status));
    }
	return 0;
}


int ex5(){
    pid_t pid;

    for(int i = 0; i < 10; i++){
        pid = fork();
        if( pid == -1 ){
            perror("Fork failed!");
            return 1;
        }
        if( !pid ){
            printf("--- Child %d ---\n", i);
            printf("PPID = %d\nPID = %d\n",getppid(),getpid());
            sleep(1);
        }
        else{
            pid = wait(NULL);
            printf("Child awakes = %d\n", pid);
            _exit(0);
        }
    }
	return 0;
}


///////////////// ex6/7	
#define LINES 5
#define COLS  10000

void genMatrix(int matrix[LINES][COLS]){
    for(int i = 0; i < LINES; i++)
        for(int j = 0; j < COLS; j++)
            matrix[i][j] = rand() % 10;
}

int find(int line[], int x){
    int i;
    for(i = 0; i < COLS && line[i] != x; i++);
    return (i == COLS) ? 0 : 1;
}

int main(int argc, char** argv){
    int n = atoi(argv[1]);
    int line, status;
    int matrix[LINES][COLS];
    int lines[LINES];
    memset(lines,0,LINES);
    pid_t pid;

    genMatrix(matrix);

    for(int i = 0; i < LINES; i++){
        pid = fork();
        if( !pid ){
            line = (find(matrix[i],n)) ? i : -1;
            _exit(line);
        }
    }

    for(int i = 0; i < LINES; i++){
        wait(&status);
        if( WEXITSTATUS(status) >= 0 )
            lines[WEXITSTATUS(status)] = 1;
    }

    printf("--- Lines that contain %d ---\n", n);
    for(int i = 0; i < LINES; i++)
        if( lines[i] )
            printf("Line %d\n", i);

	return 0;
}

///////////////////