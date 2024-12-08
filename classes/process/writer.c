#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <semaphore.h>
#include <fcntl.h>

int main()
{

	int in = 0, out = 0;
    FILE* spool;
    
    //Abre semÃ¡foro criado pelo printer
    sem_t *sem = sem_open("/sem_printer", O_RDWR);

	//laÃ§o infinito para pegar texto e mandar para impressÃ£o
	while(1)
	{
		//Espera usuÃ¡rio digitar texto e o lÃª
		char texto[100];
		scanf("%s", texto);
		
		//Espera o semÃ¡foro ficar verde para continuar. Antes de entrar na regiÃ£o crÃ­tica, coloca vermelho (0)
		sem_wait(sem);

		//***InÃ­cio da regiÃ£o crÃ­tica***, pois manipula arquivo compartilhado com a printer
		
		//abre arquivo com informaÃ§Ãµes do spooler, lendo variÃ¡veis in (Ã­ndice do arquivo a ser gravado) e out (Ã­ndice do arquivo a ser impresso)
		while(!(spool = fopen("spool.txt","r+")));		
		fscanf(spool, "%d %d\n", &in, &out);
		
		//cria arquivo de impressÃ£o com a string digitada pelo usuÃ¡rio. O nome do arquivo Ã© igual ao Ã­ndice de entrada do spooler
		char filename[20];
		sprintf(filename, "%d.txt", in);
		
		FILE* fout = fopen(filename, "w");
		
		fprintf(fout, "%s", texto);
		
		fclose(fout);
	
		//atualiza spooler com o Ã­ndice da prÃ³xima posiÃ§Ã£o livre
		rewind(spool);
    	fprintf(spool, "%d %d\n", (in+1), out);
		fclose(spool);
    	
    	//acorda processo printer lendo o seu nÃºmero no arquivo
    	int pid;
    	FILE* pidf = fopen("pidprinter.txt", "r");
    	fscanf(pidf, "%d", &pid);
    	kill(pid, SIGCONT);
    	fclose(pidf);
		
		//*** Fim da regiÃ£o crÃ­tica ***
		//passa semÃ¡foro para verde (1) para que outro processo que tambÃ©m utiliza esse sinal possa entrar na sua regiÃ£o crÃ­tica
		sem_post(sem);
	}
	
	return 0;
}
