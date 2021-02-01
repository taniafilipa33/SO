#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
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