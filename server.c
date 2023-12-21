#include<stdio.h> 
#include<stdlib.h>
#include<ctype.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<netinet/in.h> 
#include<signal.h> 
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#define SIZE sizeof(struct sockaddr_in) 

void check (char tabuleiro[][3]);
void catcher(int sig); 
int newsockfd[2]; 
int pid[2];
int porta = 8080;

int main(int argc, char *argv[]) { 
	int sockfd[2]; 
	char serverRead[1];
	char ans[1];
	int usr=0;
	int ctr=1;
	int linha = 0;
	int coluna = 0;
	int escolha=0;
	char x[4];
	char a[2][40];
    char tabuleiro [3][3] =   {	
		{' ',' ',' '},
		{' ',' ',' '},
		{' ',' ',' '}
	};

	if (argc > 1) {
        porta = atoi(argv[1]);
    }

	struct sockaddr_in server = {AF_INET, porta, INADDR_ANY}; 
	strcpy(ans,"");
	static struct sigaction act,act2; 
	act.sa_handler = catcher; 
	sigfillset(&(act.sa_mask)); 
	sigaction(SIGPIPE, &act, 0); 
	sigaction(SIGTSTP, &act, 0);
	sigaction(SIGINT, &act, 0);
	
	if ((sockfd[0] = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{ 
		perror("Socket Call Failed"); 
		exit(1); 
	} 
	
	if ( bind(sockfd[0], (struct sockaddr *)&server, SIZE) == -1) 
	{ 
		perror("Bind Call Failed"); 
		exit(1); 
	}
	
	printf("Esperando os jogadores se conectarem... \n");
	
	strcpy(a[0],"Jogador 1 conectado!\nEsperando...\n");
	
	while(usr<2)
	{
		if ( listen(sockfd[0], 5) == -1 ) 
		{ 
			perror("Listen Call Failed\n"); 
			exit(1) ; 
		}
		newsockfd[usr] = accept(sockfd[0], 0, 0);
		usr++;
		if (usr==1)
		{	
			strcpy(a[1],"0");
			write(newsockfd[0],a,sizeof(a));
			read(newsockfd[0],x,sizeof(x));
			pid[0]=atoi(x);
		}
		
		printf("Nº de jogadores conectados: %d\n",usr);
		
		if (usr==2)
		{
			strcpy(a[0],"Começando o jogo!!");
			strcpy(a[1],"1");
			write(newsockfd[0],a,sizeof(a));
			strcpy(a[1],"2");
			write(newsockfd[1],a,sizeof(a));
			read(newsockfd[1],x,sizeof(x));
			pid[1]=atoi(x);	
		} 	
	}	
	if ( fork() == 0) 
	{
		int count=0; 		
		while (count==0) 
		{ 	
			read(newsockfd[ctr], serverRead, sizeof(serverRead));			
			escolha = atoi(serverRead);
			printf("O servidor recebeu: %d\n",escolha);
			linha = --escolha/3;
			coluna = escolha%3;
			tabuleiro[linha][coluna] = (ctr==0)?'X':'O';
			if(ctr == 1)
			ctr = 0;
			else
			ctr = 1;
			write(newsockfd[ctr],tabuleiro,sizeof(tabuleiro)); 
			check(tabuleiro);					
		}
		close(newsockfd[0]);
		exit (0);
	} 	
	//wait();
	close(newsockfd[1]);
}

void check (char tabuleiro[][3])
{
	int i;
	char key = ' ';
	for (i=0; i<3;i++)
	if (tabuleiro [i][0] == tabuleiro [i][1] && tabuleiro [i][0] == tabuleiro [i][2] && tabuleiro [i][0] != ' ') 
	key = tabuleiro [i][0];	
	for (i=0; i<3;i++)
	if (tabuleiro [0][i] == tabuleiro [1][i] && tabuleiro [0][i] == tabuleiro [2][i] && tabuleiro [0][i] != ' ') 
	key = tabuleiro [0][i];
	if (tabuleiro [0][0] == tabuleiro [1][1] && tabuleiro [1][1] == tabuleiro [2][2] && tabuleiro [1][1] != ' ') 
	key = tabuleiro [1][1];
	if (tabuleiro [0][2] == tabuleiro [1][1] && tabuleiro [1][1] == tabuleiro [2][0] && tabuleiro [1][1] != ' ') 
	key = tabuleiro [1][1];
	
	if (key == 'X')
	{
		printf("Jogador 1 venceu\n\n");
		kill(pid[0], SIGUSR1);
		kill(pid[1], SIGUSR1);
		exit (0); 
	}
	
	if (key == 'O')
	{
		printf("Jogador 2 venceu\n\n");
		kill(pid[0], SIGUSR2);
		kill(pid[1], SIGUSR2);
		exit (0);
	}
	int j,k,c=0;
	for(j=0;j<3;j++){
		for(k=0;k<3;k++){
			if(tabuleiro [j][k] != ' '){
				c++;
			}
		}
	}
	if(c==9){
		printf("Empate\n");
		kill(pid[0], SIG_IGN);
		kill(pid[1], SIG_IGN);
		exit (0);
	}
}

void catcher(int sig) 
{ 
	printf("Saindo...\n");
}
