#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#include <semaphore.h>
#include <fcntl.h>

int main()
{
	int in = 0, out = 0;
	
	//pega nÃºmero do processo e grava no arquivo para writer poder acordar o printer
	pid_t pid = getpid();
	FILE* pidfile = fopen("pidprinter.txt", "w");
	fprintf(pidfile, "%d", pid);
	fclose(pidfile); 

	//cria semÃ¡foro, inicializando como verde (1)
    sem_t *sem = sem_open("/sem_printer", O_CREAT | O_EXCL, 0644, 1);

	while(1)
	{
		//Espera o semÃ¡foro ficar verde para continuar. Antes de entrar na regiÃ£o crÃ­tica, coloca vermelho (0)
		sem_wait(sem);
		
		//***InÃ­cio da regiÃ£o crÃ­tica***, pois manipula arquivo compartilhado com a printer
		
		//abre arquivo com informaÃ§Ãµes do spooler, lendo variÃ¡veis in (Ã­ndice do arquivo a ser gravado) e out (Ã­ndice do arquivo a ser impresso)
		FILE* spool;
	    while(!(spool = fopen("spool.txt","r+")));		
	    fscanf(spool, "%d %d\n", &in, &out);

		//incrementa out para ter Ã­ndice do arquivo lido (o out sempre aponta para o Ã­ndice "anterior", de tal forma que, se ele Ã© negativo, a impressora sabe que jÃ¡ imprimiu tudo)
		out++;
		
		//se o out for vÃ¡lido, entra na rotina de impressÃ£o
	    if(out < in && out >= 0)
	    {
	    	//gera o nome do arquivo a imprimir
			char filename[20];	
			sprintf(filename, "%d.txt", out);
	
			//abre arquivo a imprimir
            FILE *fout;
            while(!(fout = fopen(filename, "r")));
		
			//imprime arquivo
			char content[100];
			fscanf(fout, "%s", content);
			printf("%s\n", content);

			//fecha arquivo e remove
			fclose(fout);
			remove(filename);
		}else{
			if(in == out)
			    in = 0;
			out = -1;
			
		}

		//atualiza spool com novos in e out (out incrementado para apontar para o prÃ³ximo arquivo a ser impresso)
		rewind(spool);
    	fprintf(spool, "%d %d\n", in, out);		
		fclose(spool);
		
		//*** Fim da regiÃ£o crÃ­tica ***
		//passa semÃ¡foro para verde (1) para que outro processo que tambÃ©m utiliza esse sinal possa entrar na sua regiÃ£o crÃ­tica
		sem_post(sem);
		
		//Se imprimiu tudo (out = -1), durma
		if(out == -1) raise(SIGSTOP);
	}
	return 0;
}
