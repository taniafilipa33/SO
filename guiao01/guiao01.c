#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdio.h>
#include <stdlib.h>
#include "person.h"
#include <string.h>

#define DEZMEGAS 10*1024*1024

int criaFicheiro(int argc, char const *argv[]){
	int fd,i;
	char a = 'a';
	fd = open(argv[1], O_RDONLY| O_WRONLY | O_CREAT,0640);
	for(i=0; i< DEZMEGAS; i++){
		write(fd,&a,1);
	}
	close(fd);
	return 0;
}


int mycp(int argc, char const *argv[]){
	int fd;
	char* buff[100];
	fd = open(argv[1], O_RDONLY);
	int fd2 = open(argv[2], O_RDONLY| O_WRONLY | O_CREAT,0640);
	
	int n;
	while(n>0){
	n = read(fd,buff,1);
	write(fd2,buff,n);
	}
	close(fd);
	close(fd2);
	return 0;
  }

int mycat(int argc, char const *argv[])
{
	char buf[100];
	int n = 1;

	while (n > 0) { //continuamente le do STDIN e escreve no STDOUT até não ler nada	
		/* 0 = STDIN */
		n = read(0,buf,1);
		/* 1 = STDOUT */
		write(1,buf,n); //ignora o valor do retorno de write.
	}

	return 0;
}

int exercicio3(int argc, char const *argv[])
{
	/* code */
	return 0;
}


//EXERCICIO 7

int new_person(char* name, int age){
	Person p;
	p.age = age;
	strcpy(p.name,name);
	int fd = open(FILENAME,O_CREAT | O_APPEND );
	write(fd,&p,sizeof(Person));
	if(res<0){
		perror("Error crating person");
		return -1;
	}
}


int person_change_age(char*name,int age){
	int bytes_read;
	int res;
	int fd = open(FILENAME,O_READ);
	Person h;

	while((res=read(fd,&h,sizeof(Person)))>0 && r){
		if(strcmp(h.name,name)==0) 
			r=0;
	}
	if(r==0) 	h.age=age;
	lseek(fd,sizeof(Person),SEEK_CUR)
}

int person_change_age_2(long pos, int age){
	Person p;
	int fd = open(FILENAME,O_RDWR,0600);

	int seek_res = lseek(fd,pos*sizeof(Person),SEEK_SET);
	if(seek_res<0){
		perror("Error lseek");
		return -1;
	}

	int bytes_read = read(fd,&p,sizeof(Person));
	if(bytes_read<0){
		perror("Error read");
		return -1;
	}

	printf("Read Person name %s age - %d\n",p.name,p.age);

	p.age=age;

	seek_res = lseek(fd, -sizeof(Person),SEEK_CUR);
	if(seek_res<0){
		perror("Error lseek");
		return -1;
	}

	int res = write(fd,&p,sizeof(Person));
	while(res<0){
		perror("Error write");
		return -1;
	}

	printf("Write Person name %s age %d\n",p.name,p.age);

	close(fd);
}

int main(){
	// new_person("Tania",21);
	person_change_age("Tania",20);
	return 0;
}



















