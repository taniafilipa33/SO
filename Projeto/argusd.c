#include <sys/types.h>

#include <unistd.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "argus.h"
#include <sys/stat.h>
#include <signal.h>

extern void asprintf();

char read_buffer[4096];
int read_buffer_pos = 0;
int read_buffer_end = 0;
int nComandos;
int pid2 = 0;
int pids_count = 0;
int *pids;

time_t tInatividade;
time_t tExecucao = 10;

int inde;
char *name;

/**
 * Capta ctrl-c e elimina ficheiros
 */
void sigint_handler(int signum)
{
  unlink("inpipe");
  unlink("historico.txt");
  unlink("log.txt");
  exit(1);
}

/**
 * Cria e guarda um comando no ficheiro histórico em modo Running
 */
int conexao(char *comando)
{
  int fd1;
  Comandos c;

  c.pid = pid2;
  strcpy(c.comando, comando);
  strcpy(c.status, "Running");

  if ((fd1 = open("historico.txt", O_RDWR, 0666)) < 0)
  {
    perror("open");
    exit(1);
  }

  c.id = nComandos;

  lseek(fd1, 0, SEEK_END);
  if (write(fd1, &c, sizeof(Comandos)) < 0)
  {
    perror("write");
  }

  close(fd1);
  return 0;
}

/**
 * Após terminar o comando atualiza o seu estado.
 * Estados:
 * concluido;
 * max execução.
 */
int statusUpdate(int index, char *status)
{
  int n = 0;
  int found = 0;
  Comandos p;

  int fd;

  if ((fd = open("historico.txt", O_RDWR, 0666)) < 0)
  {
    perror("open");
    exit(1);
  }

  int bytes_read = 0;

  while ((bytes_read = read(fd, &p, sizeof(Comandos))) > 0)
  {

    if (index == p.id)
    {
      if (!strcmp(p.status, "Running"))
      {
        strcpy(p.status, status);
        lseek(fd, -sizeof(Comandos), SEEK_CUR);
        write(fd, &p, sizeof(Comandos));
        found = 1;
      }
    }
  }
  return 0;
}

/**
 * Função chamada para executar um determinado comando.
 * Utiliza pipes anónimos.
 * Colocado o output no ficheiro log.txt.
 */
int executar(char *cmd, char *nome, int id)
{

  char **args = (char **)malloc(4 * sizeof(char *));
  char **args2 = (char **)malloc(4 * sizeof(char *));
  int i = 0;
  int k = 0;
  char *copia = (char *)malloc(4 * sizeof(char));

  strcpy(copia, cmd);

  args[i] = strtok(cmd, "|");

  while (args[i] != NULL)
  {
    args[++i] = strtok(NULL, "|");
  }

  char **commands[i];
  char *g[i][10];

  for (int m = 0; m < i; m++)
  {

    k = 0;
    args2[k] = strtok(args[m], " ");

    while (args2[k] != NULL)
    {

      args2[k + 1] = strtok(NULL, " ");
      k += 1;
    }

    for (int n = 0; n < k; n++)
    {
      g[m][n] = args2[n];
    }

    g[m][k] = NULL;

    commands[m] = g[m];
  }

  int tt = 0;

  int beforePipe = 0;
  int afterPipe[2];
  int n = i;
  for (int t = 0; t < n; t++)
  {

    if (t < n - 1)
      pipe(afterPipe);

    if ((pid2 = fork()) == 0)
    {
      if (t > 0)
      {
        dup2(beforePipe, 0);
        close(beforePipe);
      }
      if (t < n - 1)
      {
        dup2(afterPipe[1], 1);
        close(afterPipe[0]);
        close(afterPipe[1]);
      }
      if (t == n - 1)
      {

        int p = -1;
        int pipe_log[2];
        pipe(pipe_log);

        if ((p = fork()) == 0)
        {
          int fd1 = -1;
          if ((fd1 = open("log.txt", O_CREAT | O_WRONLY, 0666)) < 0)
          {
            perror("open");
            exit(1);
          }
          int ini = lseek(fd1, 0, SEEK_END);
          int fim = ini;
          dup2(pipe_log[0], 0);
          close(pipe_log[1]);
          close(pipe_log[0]);
          int fdo;
          fdo = open(nome, O_WRONLY);
          dup2(fdo, 1);
          close(fdo);
          char a[512];
          int r = 0;
          while ((r = read(0, a, 512)) > 0)
          {
            write(1, a, r);
            write(fd1, a, r);
            fim += r;
          }
          setIndices(ini, fim, id);
          close(fd1);
          _exit(1);
        }
        dup2(pipe_log[1], 1);
        close(pipe_log[1]);
        close(pipe_log[0]);
      }
      execvp(commands[t][0], commands[t]);

      _exit(1);
    }

    if (t < n - 1)
      close(afterPipe[1]);
    if (t > 0)
      close(beforePipe);
    beforePipe = afterPipe[0];
  }
  conexao(copia);
  return 0;
}

/**
 *Guarda os índices dos outputs do ficheiro log. 
 */
int setIndices(int ini, int fim, int id)
{

  int fd;

  if ((fd = open("historico.txt", O_RDWR, 0666)) < 0)
  {
    perror("open");
    exit(1);
  }

  lseek(fd, id * sizeof(Comandos), SEEK_SET);
  lseek(fd, 400 + 2 * sizeof(int), SEEK_CUR);
  write(fd, &ini, sizeof(int));
  write(fd, &fim, sizeof(int));

  close(fd);

  return 0;
}

/**
 * Encontra o output para um determinado comando id
 */
void output(int id, char *pipeC)
{
  int fd;
  char *result = (char *)malloc(4 * sizeof(char));
  int ini = 0, fim = 0;

  if ((fd = open("historico.txt", O_RDWR, 0666)) < 0)
  {
    perror("open");
    exit(1);
  }

  lseek(fd, id * sizeof(Comandos), SEEK_SET);
  lseek(fd, 400 + 2 * sizeof(int), SEEK_CUR);
  read(fd, &ini, sizeof(int));
  read(fd, &fim, sizeof(int));

  close(fd);

  if ((fd = open("log.txt", O_RDWR, 0666)) < 0)
  {
    perror("open");
    exit(1);
  }

  int fdout = open(pipeC, O_WRONLY);

  lseek(fd, ini, SEEK_SET);
  char a[fim - ini];
  int i = ini;
  int rea = 0;
  while ((rea = read(fd, a, (fim - ini))) > 0 && ini < fim)
  {
    write(fdout, a, rea);
    ini += rea;
  }
  close(fd);
  close(fdout);
}

/**
 * Devolve uma string com todas opções de tarefas.
 */
char *ajuda()
{
  char *buffer = "tempo-inatividade segs\ntempo-execução1 segs\nexecutar p1 | p2 ... | pn\nlistar\nterminar nTarefa\nhistorico\nsair\n";
  return buffer;
}


/**
 * Capta um sinal alarme e mata o processo com um determinado pid.
*/
void sigalarmExec_handler(int signum)
{
  kill(pid2, SIGKILL);
  exit(0);
}

/**
 * Devolve todos os comandos que já foram executados.
 */
char *historico()
{
  char *final = (char *)malloc(4 * sizeof(char));
  int fd;
  int aux = 0;
  if ((fd = open("historico.txt", O_RDONLY)) < 0)
  {
    perror("open");
    exit(1);
  }

  Comandos c;
  char *hist = (char *)malloc(4 * sizeof(char));
  int bytes_read = 0;
  while ((bytes_read = read(fd, &c, sizeof(Comandos))) > 0)
  {
    if (strcmp(c.status, "Running"))
    {
      asprintf(&hist, "# %d  ,   %s : %s\n", c.id, c.status, c.comando);

      strcat(final, hist);

      aux++;
    }
  }
  if (aux == 0)
  {
    return "Historico Vazio!\n";
  }
  close(fd);
  return final;
}

/**
 * Lista todos os processos em execução.
 */
char *listar()
{
  int fd;
  if ((fd = open("historico.txt", O_RDONLY)) < 0)
  {
    perror("open");
    exit(1);
  }

  Comandos c;
  char *hist = (char *)malloc(4 * sizeof(char));

  char *listaa = (char *)malloc(4 * sizeof(char));

  int bytes_read = 0;

  int aux = 0;

  while ((bytes_read = read(fd, &c, sizeof(Comandos))) > 0)
  {
    if (!strcmp(c.status, "Running"))
    {
      asprintf(&hist, "# %d  ,   %s : %s\n", c.id, c.status, c.comando);
      strcat(listaa, hist);
      aux++;
    }
  }

  if (aux == 0)
  {
    return "Nenhum processo a correr\n";
  }
  close(fd);
  return listaa;
}

/**
 * Termina um determinado processo de um comando com o índice i.
 */
void terminar(int i)
{
  int fd;
  if ((fd = open("historico.txt", O_RDONLY)) < 0)
  {
    perror("open");
    exit(1);
  }

  Comandos c;
  char hist[1024];

  int bytes_read = 0;
  while ((bytes_read = read(fd, &c, sizeof(Comandos))) > 0)
  {

    if (c.id == i)
    {
      pid_t p = c.pid;
      char *pid = malloc(10);
      sprintf(pid, "%d", p);
      unlink(pid);
      kill(p, SIGQUIT);
    }
  }

  close(fd);
}

/**
 * Abre o pipe do cliente.
 */
int escreveCliente(char *token)
{
  char *pipeC;
  asprintf(&pipeC, "client@%s", token);
  int fdout = open(pipeC, O_WRONLY);
  return fdout;
}

/**
 * Servidor que comunica com os clientes através de pipes com nome.
 * Para cada pedido de um cliente cria um fork
 * É identificado o pedido:
 * tempo de inatividade,
 * tempo de execução,
 * executar,
 * listar,
 * terminar,
 * histórico,
 * ajuda,
 * output (adicional).
 */
int main(int argc, char const *argv[])
{

  if (signal(SIGINT, sigint_handler) == SIG_ERR)
  {
    perror("signal signint");
    exit(1);
  }

  int piddi;

  char *inpipe = "inpipe";
  char *outpipe = "outpipe";

  int fdin = -1;
  int fdout = -1;

  int pidCliente = -1;
  int fd;
  if ((fd = open("historico.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666)) < 0)
  {
    perror("open");
    exit(1);
  }

  if (mkfifo(inpipe, 0666) < 0)
  {
    perror("mkfifo");
  }

  fdin = open("inpipe", O_RDONLY);
  int bytes_read = 0;

  while (1)
  {

    char buff[4096];

    while ((bytes_read = read(fdin, buff, 4096) > 0))
    {

      if ((piddi = fork()) == 0)
      {

        int n = -1;
        char *suc;
        char *command = NULL;
        char *buffer = "argus$ ";

        command = (char *)malloc(4 * sizeof(char));

        strcpy(command, buff);

        char *token = NULL;
        char *token1 = NULL;
        char *token2 = NULL;

        char *commandA = (char *)malloc(4 * sizeof(char));

        strcpy(commandA, command);

        token = strdup(strtok(buff, " "));
        token1 = strdup(strtok(NULL, " "));
        char *tok2 = strtok(NULL, "\n");
        if (tok2)
          token2 = strdup(tok2);

        if (strcmp(token1, "tempo-execucao") == 0 || strcmp(token1, "-m") == 0)
        {
          int fdout = escreveCliente(token);
          tExecucao = atoi(token2);

          write(fdout, "tempo alterado\n", sizeof("tempo alterado\n"));
          close(fdout);
        }

        if (strcmp(token1, "tempo-inatividade") == 0 || strcmp(token1, "-i") == 0)
        {
          int fdout = escreveCliente(token);

          tInatividade = atoi(token2);

          write(fdout, "tempo alterado\n", sizeof("tempo alterado\n"));
          close(fdout);
        }

        else if (strcmp(token1, "historico") == 0 || strcmp(token1, "-r") == 0)
        {
          char *savee = (char *)malloc(4 * sizeof(char));

          savee = historico();

          int fdout = escreveCliente(token);

          write(fdout, savee, strlen(savee));
          close(fdout);
        }

        else if (strcmp(token1, "ajuda") == 0 || strcmp(token1, "-h") == 0)
        {

          int fdout = escreveCliente(token);
          char *aj = ajuda();
          write(fdout, aj, strlen(aj) + 1);

          close(fdout);
        }

        else if (strcmp(token1, "executar") == 0 || strcmp(token1, "-e") == 0)
        {
          // nComandos++;
          int newfd = -1;

          if ((newfd = open("historico.txt", O_RDONLY, 0666)) < 0)
          {
            perror("open historico");
            exit(1);
          }

          nComandos = lseek(newfd, sizeof(struct Comandos), SEEK_END) / (sizeof(struct Comandos)) - 1;

          close(newfd);

          inde = nComandos;
          int pid;

          int fdout = escreveCliente(token);

          if ((pid = fork()) == 0)
          {
            char *noo = (char *)malloc(4 * sizeof(char));
            sprintf(noo, "client@%s", token);

            executar(token2, noo, inde);

            if (signal(SIGALRM, sigalarmExec_handler) == SIG_ERR)
            {
              perror("alarm");
              exit(1);
            }
            alarm(tExecucao);
            wait(NULL);

            _exit(1);
          }
          else
          {
            int status;
            int w;

            w = waitpid(pid2, &status, 0);

            if (w == -1)
            {
              perror("waitpid");
              exit(EXIT_FAILURE);
            }

            if (WIFEXITED(status))
            {

              if (WEXITSTATUS(status) == 0)
              {
                //printf("vou mudar o indice %d\n", inde);
                statusUpdate(inde, "max execução");
              }
              else if (WEXITSTATUS(status) == 1)
              {
                //printf("vou mudar o indice %d\n", inde);
                statusUpdate(inde, "concluida");
              }
            }
          }
          write(fdout, "Comando executado\n", sizeof("Comando executado\n"));
          close(fdout);
        }
        else if (strcmp(token1, "listar") == 0 || strcmp(token1, "-l") == 0)
        {
          char *list = (char *)malloc(4 * sizeof(char));
          list = listar();

          int fdout = escreveCliente(token);
          write(fdout, list, strlen(list));
          close(fdout);
        }
        else if (strcmp(token1, "terminar") == 0 || strcmp(token1, "-t") == 0)
        {
          terminar(atoi(token2));
          int fdout = escreveCliente(token);

          statusUpdate(atoi(token2), "Processo terminado");

          write(fdout, "Processo Terminado\n", strlen("Processo Terminado\n"));
          close(fdout);
        }
        else if (strcmp(token1, "output") == 0 || strcmp(token1, "-o") == 0)
        {

          char *pipeC = (char *)malloc(4 * sizeof(char));

          sprintf(pipeC, "client@%s", token);
          output(atoi(token2), pipeC);
        }
        else
        {
          write(fdout, "", sizeof(""));
        }
      }
    }
    close(fdout);
    //unlink(outpipe);
  }

  close(fdin);
  //unlink(inpipe);

  return 0;
}