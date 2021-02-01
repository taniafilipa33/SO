#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "guiao00.h"

int main()
{
	int size = 3;
	int *vector = malloc(sizeof(size));

	fill(vector,size,'2'); //fill com '2';

	printf("%d\n",  find(vector,size,3));  //find um valor que quisermos
	
	return 0;
}