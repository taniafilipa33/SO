#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include "argus.h"

extern void asprintf();
char *name;

/**
 * Capta "sair" e elimina ficheiros
 */
void sigint_handler(int signum)
{
  execlp("rm", "rm", name, NULL);
  exit(1);
}


/**
 * Criação do pipe que permite comunicar com o servidor.
 * Cria fork e processo filho é responsável por enviar os pedidos
 * quer por shell ou por linha de comando;
 * processo pai é responsável por receber as respetivas respostas.
 */
int main(int argc, char const *argv[])
{

  int amount_read = 0;

  int mypid = getpid();
  asprintf(&name, "client@%d", mypid);

  if (mkfifo(name, 0666) < 0)
  {
    perror("mkfifo");
  }
   write(1, "\033[1;36margus$ \033[0m ", sizeof("\033[1;38margus$ \033[0m "));

  if (fork())
  {
    char buff[4096];
    int inpipe = open("inpipe", O_WRONLY);
    char *strout;
    if (argc > 1)
    {
      for (int i = 1; i < argc; i++)
      {
        strcat(buff, argv[i]);
        strcat(buff, " ");
      }
      buff[strlen(buff) - 1] = '\0';
      asprintf(&strout, "%d %s", mypid, buff);
      write(inpipe, strout, sizeof(buff));
      free(strout);
      close(inpipe);
    }
    else
    {

      while ((amount_read = read(0, buff, sizeof(buff))))
      {
        buff[strlen(buff) - 1] = '\0';
        if (!strcmp(buff, "sair"))
        {
          sigint_handler(0);
          break;
        }
        asprintf(&strout, "%d %s", mypid, buff);
        write(inpipe, strout, sizeof(buff));
        memset(buff, 0, sizeof(buff));
        free(strout);
      }
      close(inpipe);
    }
  }

  else
  {
    char buff[4096];
    int outpipe = open(name, O_RDONLY);
    open(name, O_WRONLY);
    while (amount_read = read(outpipe, buff, sizeof(buff)))
    {
      write(1, buff, amount_read);
      write(1, "\033[1;36margus$ \033[0m ", sizeof("\033[1;38margus$ \033[0m "));
      memset(buff, 0, sizeof(buff));
    }

    close(outpipe);
  }

  return 0;
}
