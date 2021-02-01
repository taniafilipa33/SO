/**
 * Estrutura utilizada para armazenar um comando.
 */
typedef struct Comandos
{
	char comando[200];
	char status[200];
	int id;
	int pid;
	int inicio; //extra
	int fim;   //extra
} Comandos;

/**
 * Funções definidas no argus.c
 */
int executar(char *cmd, char *nome,int id);
int setIndices(int ini,int fim,int id);
void output(int id,char* pipeC);
int conexao(char *comando);
int statusUpdate(int index, char *status);
char *ajuda();
void tempoInatividade(int i);
void tempoExecucao(int i);
char *historico();
char *listar();
void terminar(int i);
ssize_t readln(int fildes, char *buf, size_t nbyte);
int execute(char *cmd[]);
int split(char *original, int offset, char **s1, char **s2);
int main(int argc, char const *argv[]);
