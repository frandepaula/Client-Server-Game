#include <sys/socket.h>
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>


#define SIZE sizeof(struct sockaddr_in)

int play(int porta);
int menu(void);
int te = 0;
void catcher(int sig);
void mostrar_logo();
void desenha_tabuleiro();
void win_handler(int signum);
int sockfd;

int main(int argc, char *argv[]) {
    int porta = 8080; // Porta padrão
    if (argc > 1) {
        porta = atoi(argv[1]); // Verifica se uma porta foi passada como argumento
    }

    while (1) {
        switch (menu()) {
            case 1: {
                printf("\nVocê selecionou a opção de jogar\n\n");
                play(porta);
                break;
            }
            case 2: {
                printf("\nSair\n\n");
                if (sockfd != -1) {
                    close(sockfd); // Fecha o socket do servidor, se estiver aberto
                }
                exit(0);
                break;
            }
            default: {
                printf("\nEntrada inválida\n\n");
                break;
            }
        }
    }
}

int play(int porta) {
    void result(char[], char[]);
    static struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigfillset(&(act.sa_mask));
    sigaction(SIGTSTP, &act, 0);
    signal(SIGUSR1, win_handler);
    signal(SIGUSR2, win_handler);
    struct sockaddr_in server = {AF_INET, porta};
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//int sockfd;
	int i,row,column,choice;
	int iclientRead; 		
	char input; 
	char a[2][40];
	char pid[4];
	char clientRead[3][3];
	char clientWrite[1];
	
	char tabuleiro_numerado[3][3] = {							
		{'1','2','3'},
		{'4','5','6'},
		{'7','8','9'}
	}; 
	
	char tabuleiro[3][3] =   {						
		{' ',' ',' '},
		{' ',' ',' '},
		{' ',' ',' '}
	};
	
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Chamada de Socket Falhou");
		exit(1);
	}
	
	if ( connect (sockfd, (struct sockaddr *)&server, SIZE) == -1) 
	{ 
		perror("Chamada de Conexão Falhou");
		exit(1); 
	} 
	
	read(sockfd,a,sizeof(a));
	printf("%s\n",a[0]);
	
	if(strcmp(a[1],"0")==0)
	{
		int num1 = getpid();
		sprintf(pid,"%d",num1);
		write(sockfd, pid, sizeof(pid));	
		read(sockfd,a,sizeof(a));
		printf("%s\n",a[0]);
		mostrar_logo();
	}
	
	if(strcmp(a[1],"2")==0)
	{
		int num2 = getpid();
		sprintf(pid,"%d",num2);
		write(sockfd, pid, sizeof(pid));	
	}
	
	if (strcmp(a[1],"1")!=0)
	{
		mostrar_logo();		
		desenha_tabuleiro(tabuleiro_numerado);	
		for(;;)
		{
			printf("\nJogador %d, por favor, insira o número do tabuleiro onde você deseja colocar o seu '%c': \n",(strcmp(a[1], "1")==0)?1:2,(strcmp(a[1], "1")==0)?'X':'O');
			scanf("%s",clientWrite);
			choice = atoi(clientWrite);
			row = --choice/3;
			column = choice%3;
			if(choice<0 || choice>9 || tabuleiro[row][column]>'9'|| tabuleiro[row][column]=='X' || tabuleiro[row][column]=='O')
			printf("Entrada invalida, Escolha outra.\n\n");
			else
			{
				tabuleiro[row][column] = (strcmp(a[1], "1")==0)?'X':'O';					
				break;
			}
		}	
		write(sockfd, clientWrite, sizeof(clientWrite));
		//mostrar_logo();
		desenha_tabuleiro(tabuleiro);
		printf("\nTabuleiro atual\n\n");
	}
	for(input = 'x';;) 
	{ 	
        int b[2][40];
		if (input == '\n') 
		{	if (te==0)
			{
				desenha_tabuleiro(tabuleiro_numerado);	
			}					
			for(;;)
			{	
				
				if (te==0)
				{
					printf("\nJogador %d, Sua vez .. Digite o nº do tabuleiro que deseja marcar com '%c': \n",(strcmp(a[1], "1")==0)?1:2,(strcmp(a[1], "1")==0)?'X':'O');
					scanf("%s",clientWrite);
					choice = atoi(clientWrite);
					row = --choice/3;
					column = choice%3;
					if(choice<0 || choice>9 || tabuleiro[row][column]>'9'|| tabuleiro[row][column]=='X' || tabuleiro[row][column]=='O')
					printf("Entrada invalida. Digite novamente.\n\n"); 
					else
					{
						tabuleiro[row][column] = (strcmp(a[1], "1")==0)?'X':'O';
						break;
					}
				}
			}	
			write(sockfd, clientWrite, sizeof(clientWrite));
			mostrar_logo();
			desenha_tabuleiro(tabuleiro);
			printf("\nTabuleiro atual\n\n");
			if (te==1)
			{
				printf("JOGADOR 1 GANHOU!!\n");
				exit(0);
			}
			if (te==2)
			{
				printf("JOGADOR 2 GANHOU!!\n");
				exit(0);
			}
			if (te==3)
			{
				printf("EMPATE!\n");
				exit(0);
			}
		}
		if (read(sockfd, clientRead, sizeof(clientRead)) >0) 
		{
			mostrar_logo();
			memcpy(tabuleiro, clientRead, sizeof(tabuleiro));
			desenha_tabuleiro(tabuleiro);
			printf("\nTabuleiro atual\n\n");
			input = '\n';
			if (te==1)
			{
				printf("JOGADOR 1 GANHOU!!\n");
				exit(0);
			}
			if (te==2)
			{
				printf("JOGADOR 2 GANHOU!!\n");
				exit(0);
			}
			if (te==3)
			{
				printf("EMPATE!\n");
				exit(0);
			}
		}	
		else {
		    printf("Voce venceu!!\n");
			close(sockfd);
			exit(1); 
		}
		
	}
}

void mostrar_logo() 
{
	printf("\033[1;33m"); 
    printf("                ┌────────────────────────────┐\n");
    printf("                │       JOGO DA VELHA        │\n");
    printf("                └────────────────────────────┘\n");
    printf("\033[0m"); 
}

void desenha_tabuleiro(char tabuleiro[][3]){

	printf("\033[1;36m");
	printf("                ╭───────┬───────┬───────╮\n");
    printf("                │       │       │       │\n");
    printf("                │   %c   │   %c   │   %c   │\n", tabuleiro[0][0], tabuleiro[0][1], tabuleiro[0][2]);
    printf("                ├───────┼───────┼───────┤\n");
    printf("                │       │       │       │\n");
    printf("                │   %c   │   %c   │   %c   │\n", tabuleiro[1][0], tabuleiro[1][1], tabuleiro[1][2]);
    printf("                ├───────┼───────┼───────┤\n");
    printf("                │       │       │       │\n");
    printf("                │   %c   │   %c   │   %c   │\n",  tabuleiro[2][0], tabuleiro[2][1], tabuleiro[2][2]);
    printf("                ╰───────┴───────┴───────╯\n");

	printf("\033[0m"); 
}

void catcher(int sig)
{
	printf("Desculpe...você só pode sair após o término da sua vez!\n");
}

int menu(void)
{
	int reply;
	mostrar_logo();
	printf("Digite 1 para Jogar.\n\n");
	printf("Digite 2 para Sair.\n\n");

	scanf("%d", &reply);
	return reply;
}

void win_handler(int signum)
{
	if (signum == SIGUSR1)
	{
		te=1;
	}
	if (signum == SIGUSR2)
	{
		te=2;
	}
	if (signum == SIG_IGN)
	{
		te=3;
	}
}
