#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <ctype.h>
#include "dogType.h"
#include <pthread.h>
#include <time.h>

#define PORT 3535
#define BACKserverLog 2
#define NUM_HILOS 32

FILE *ap;
FILE *serverLog;
int numRegistros;
pthread_mutex_t mut;
sem_t *semaforo;
int pipefd[2];
int r;
char buf[11];

struct cliente{
	int socket;
	struct sockaddr_in dir;	
};
//-------------------------------------------------HACER serverLog DEL SERVER--------------------------------------------------

void hacerserverLog(struct cliente cli, int opcion, int *numFun, char *nombre){
	char num[16];
	serverLog = fopen("serverDogs.log","a+");
	time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char output[128];
    strftime(output, 128, "%d/%m/%y %H:%M:%S", tlocal);
    fputs("Fecha y hora: ", serverLog);
    fputs(output , serverLog);
	fputs("   Cliente(IP: ", serverLog);
	fputs(inet_ntoa(cli.dir.sin_addr), serverLog);
	switch (opcion){
	    case 1: //Ingresar
	    fputs(") Registro ingresado, # de registros = ", serverLog);
	    sprintf(num, "%d", numRegistros);
	    fputs(num, serverLog);
	    break; 
	    case 2:
	   	fputs(") Registro # ", serverLog);
	    sprintf(num, "%d", *numFun);
	    fputs(num, serverLog);
	    fputs(" visto" , serverLog);
	    break; 
	    case 3:
	    fputs(") Registro #", serverLog);
	    sprintf(num, "%d", *numFun);
	    fputs(num, serverLog);
	    fputs(" borrado",serverLog);
	    break;  
	    case 4:
	    fputs(") Registro con nombre ", serverLog);
	    fputs(nombre, serverLog);
	    fputs(" buscado",serverLog);
	    break; 
	}
	fputs("\n", serverLog);
	fclose(serverLog);  

}
//----------------------------------------------------------BUSCAR----------------------------------------------------------

char *lowerCaseParse(char *try){ //pasa los strings a minuscula
	int i;	
	for(i = 0; try[i]; i++){
		try[i] = tolower(try[i]);
	}	
	return try;
}

void buscar(struct cliente cli){
	int sdcli = cli.socket; //NO NECESITA SER BLOQUEADO
	char name[32];
	int count;
	int confirm;
	pthread_mutex_lock(&mut);
	printf("Inicio Mutex en buscar\n");
	sem_wait(semaforo);
	printf("Inicio Semaforo en buscar\n");
	r = read(pipefd[0], buf, 10);
	printf("Inicio Tuberia en buscar\n");    
	ap=fopen("dataDogs.dat","rb+");//Abre el archivo en modo lectura binaria
    fseek(ap, 0, SEEK_END);//Señal al final del archivo
    count = (ftell(ap)/(sizeof(struct dogType)+1));//Contador de cantidad de registros en el archivo
    fclose(ap);
    send(sdcli,&count,sizeof(int),0);
    send(sdcli,"Inserte el nombre del animal a buscar: ",266,0);//Solicita nombre a buscar
    recv(sdcli,name,sizeof(name),0);
    int actual = 0;
    for(actual=0;actual<count;actual++){
        struct dogType *registro = malloc(sizeof(struct dogType));//Se reserva espacio para la estructura
        ap=fopen("dataDogs.dat","rb");//Abre archivo
        fseek(ap, ((actual)*(sizeof(struct dogType)+1)), SEEK_SET);//Pone puntero en registro seleccionado
        fread(registro,sizeof(struct dogType),1,ap);//Lee registro seleccionado
        fclose(ap);
        if(strcmp(lowerCaseParse(name),lowerCaseParse(registro -> nombre))==0){
        	confirm=1;            
        	send(sdcli,&confirm,sizeof(int),0);
        	send(sdcli,registro,sizeof(struct dogType),0);
        }else{
        	confirm=0;
        	send(sdcli,&confirm,sizeof(int),0);
        }
        free(registro);
    }
   	pthread_mutex_unlock(&mut);
   	printf("Fin Mutex en buscar\n");
   	sem_post(semaforo);
   	printf("Fin semaforo en buscar\n");
   	write(pipefd[1], "T", 1);
   	printf("Fin tuberia en buscar\n");
    hacerserverLog(cli, 4, NULL, name);
}

//----------------------------------------------------------BORRAR----------------------------------------------------------

void borrar(struct cliente cli){
	int sdcli = cli.socket;
	int count;
	int selected;
	char nombreActual[32];
	char comando[266];
	char existen[266];
	char toSend[266];
	char existen1[266];
	pthread_mutex_lock(&mut);
	printf("Inicio Mutex en borrar\n");
	sem_wait(semaforo);
	printf("Inicio Semaforo en borrar\n");
	r = read(pipefd[0], buf, 10);
	printf("Inicio Tuberia en borrar\n");
    ap=fopen("dataDogs.dat","rb+");//Abre el archivo en modo lectura binaria
    fseek(ap, 0, SEEK_END);//Señal al final del archivo
    count = (ftell(ap)/(sizeof(struct dogType)+1));//Contador de cantidad de registros en el archivo
    fclose(ap);    
    send(sdcli,&count,sizeof(int),0);
    if(count == 0){
    	send(sdcli,"No hay registros",30,0);
    }else{
    	struct dogType *p = malloc(sizeof(struct dogType));
        sprintf(existen,"%d registros existentes\n", count);//Muestra la cantidad de registros existentes
        send(sdcli,existen,sizeof(existen),0);
        sprintf(existen1,"Seleccione el registro a borrar: ");//Muestra la cantidad de registros existentes
        send(sdcli,existen1,sizeof(existen),0);
        recv(sdcli, &selected, sizeof(int),0);//Solicita que se ingrese el indice del registro a borrar
        if(selected<=count){
            ap=fopen("dataDogs.dat","rb+");//Abre el archivo en lectura binaria
            fseek(ap, ((selected-1)*(sizeof(struct dogType)+1)), SEEK_SET);//Ubica el apuntador en el índice seleccionado
            fread(p,(sizeof(struct dogType)+1),1,ap);
            snprintf(comando, sizeof(comando), "sed -i '%dd' dataDogs.dat",selected);//Borra línea en el archivo .dat correspondiente al indice seleccionado
            system(comando);//Envia comando a sistema   
            fclose(ap);//Cierra archivo
            strcpy(p->nombre,nombreActual);
            numRegistros--;
            sprintf(toSend,"Registro borrado correctamente");//Confirma
            send(sdcli,toSend,sizeof(toSend),0);
           	hacerserverLog(cli, 3, &selected, NULL);
        }else{
        	send(sdcli,"Selección inválida, vuelva a intentarlo.",45,0);
        }

        free(p);
    }
   	pthread_mutex_unlock(&mut);
   	printf("Fin Mutex en borrar\n");
   	sem_post(semaforo);
   	printf("Fin semaforo en borrar\n");
   	write(pipefd[1], "T", 1);
   	printf("Fin tuberia en borrar\n");//VUELVE A ESCRIBIR EN LA TUBERIA
}

//----------------------------------------------------------VER----------------------------------------------------------

void ver(struct cliente cli){
	int sdcli = cli.socket;
	char nombre[32];
	char ruta[77];
	int count;
	int selected;
	struct dogType *registro;
	//send(sdcli,"Permiso concedido\n",100,0);
    registro = malloc(sizeof(struct dogType));//Se reserva espacio para la estructura
    ap=fopen("dataDogs.dat","rb+");//Abre archivo
    fseek(ap, 0, SEEK_END);//Señal al final del archivo
    count = (ftell(ap)/(sizeof(struct dogType)+1));//Contador de cantidad de registros en el archivo
    send(sdcli,&count,sizeof(int),0);
    if(count == 0){
    	send(sdcli,"No hay registros",30,0);
    }else{
        send(sdcli,&count,sizeof(int),0);//Muestra cantidad de registros
        fclose(ap);
        pthread_mutex_lock(&mut);
        printf("Inicio Mutex en ver\n");
		sem_wait(semaforo);
		printf("Inicio Semaforo en ver\n");
		r = read(pipefd[0], buf, 10); // LEE DE LA TUBERIA
		printf("Inicio Tuberia en ver\n");
        send(sdcli,"Ingrese el índice de la mascota que desea ver: ",100,0);
        recv(sdcli,&selected,sizeof(int),0);//Solicita que se ingrese el indice del registro a ver
        if(selected<=count){
            ap=fopen("dataDogs.dat","rb");//Abre archivo
            fseek(ap, ((selected-1)*(sizeof(struct dogType)+1)), SEEK_SET);//Pone puntero en registro seleccionado
            fread(registro,sizeof(struct dogType),1,ap);//Lee registro seleccionado

            fclose(ap);

            send(sdcli,registro->nombre,sizeof(registro -> nombre),0);
            send(sdcli,&registro->peso,sizeof(float),0);

            FILE *archivoHistoria;
            sprintf(ruta, "HC/%s%f.txt", registro->nombre, registro->peso);//Ruta del archivo   
            archivoHistoria = fopen(ruta,"r+");
            if(archivoHistoria == NULL){
            	archivoHistoria = fopen(ruta,"w+");
            	if( archivoHistoria == NULL){
            		perror("Error al crear el archivo");
            		exit(-1);
            	}
            }
            fclose(archivoHistoria);

            archivoHistoria=fopen(ruta,"r+");
            fseek(archivoHistoria, 0, SEEK_END);
            int tam = ftell(archivoHistoria);
            send(sdcli, &tam, sizeof(int), 0);
            fseek(archivoHistoria, 0, SEEK_SET);
            int i=0;

            for(i = 0; i < tam; ++i)
            {
            	char aux;
            	aux = fgetc(archivoHistoria);
            	send(sdcli, &aux, sizeof(char),0);
            }

            fclose(archivoHistoria);
            remove(ruta);
            archivoHistoria = fopen(ruta,"a+t");
            recv(sdcli,&tam,sizeof(int),0);
            for(i = 0; i < tam; ++i)
            {
            	char aux;
            	recv(sdcli, &aux, sizeof(char),0);
            	fputc(aux, archivoHistoria);
            }

            fclose(ap);
            free(registro);
             hacerserverLog(cli, 2, &selected, NULL);
            send(sdcli,"Historia Clinica abierta correctamente",100,0);


        }else{
        	send(sdcli,"Selección inválida, vuelva a intentarlo.\n",100,0);
        }
    }
    pthread_mutex_unlock(&mut);
   	printf("Fin Mutex en ver\n");
   	sem_post(semaforo);
   	printf("Fin semaforo en ver\n");
   	write(pipefd[1], "T", 1);
   	printf("Fin tuberia en ver\n");//VUELVE A ESCRIBIR EN LA TUBERIA
}

//----------------------------------------------------------INGRESAR----------------------------------------------------------

void ingresar(struct cliente cli){
	int sdcli = cli.socket;//NO NECESITA SER BLOQUEADO
	char comando[45];
	char *name;
	pthread_mutex_lock(&mut);
	printf("Inicio Mutex en borrar\n");
	sem_wait(semaforo);
	printf("Inicio Semaforo en borrar\n");
	r = read(pipefd[0], buf, 10);
	printf("Inicio Tuberia en borrar\n");
	send(sdcli,"Permiso concedido\n",100,0);
    struct dogType *p =(struct dogType*) malloc(sizeof(struct dogType));//Reserva espacio para estructura

    recv(sdcli,p,sizeof(struct dogType),0);

    int count;
    ap=fopen("dataDogs.dat","rb+");//Abre el archivo en modo lectura binaria
    fseek(ap, 0, SEEK_END);//Señal al final del archivo
    count = (ftell(ap)/(sizeof(struct dogType)+1));//Contador de cantidad de registros en el archivo
    fclose(ap);
    ap=fopen("dataDogs.dat","ab+");//Abre archivo .dat
    fwrite(p,sizeof(struct dogType),1,ap);//Escribe la estructura en el dat
    fwrite("\n",1,1,ap);//Hace salto de línea
    fclose(ap);
    free(p);
    numRegistros++;
    hacerserverLog(cli, 1, NULL, NULL);
   	pthread_mutex_unlock(&mut);
   	printf("Fin Mutex en ingresar\n");
   	sem_post(semaforo);
   	printf("Fin semaforo en ingresar\n");
   	write(pipefd[1], "T", 1);
   	printf("Fin tuberia en ingresar\n");
    send(sdcli,"Mascota ingresada correctamente",100,0);
}


//----------------------------------------------------------INICIAR Y MENU----------------------------------------------------------

void menu(struct cliente cli){
	float bufferFloat = 0;
	int r;
	int sdcli = cli.socket;

	while(bufferFloat!=5){
		r = recv(sdcli,&bufferFloat,sizeof(float),0);
		switch ((int)bufferFloat){
		    case 1: //Ingresar
		    ingresar(cli);
		    break; 
		    case 2:
		    ver(cli);
		    break; 
		    case 3:
		    borrar(cli);
		    break;  
		    case 4:
		    buscar(cli);
		    break;  
		    case 5:
		    send(sdcli,"Salir",30,0);
		    break;  
		    default:
		    send(sdcli,"Elija de nuevo",30,0);
		}
	}
}


void iniciar(void *cli){
	struct cliente idCliente;
	idCliente = *(struct cliente *)cli;
	menu(idCliente);
	close(idCliente.socket);
}

//----------------------------------------------------------MAIN----------------------------------------------------------


int main(){
	pthread_mutex_init(&mut, NULL); //INICIO MUTEX
	semaforo= sem_open("semaforo_name", O_CREAT, 0700, 1);//INICIO SEMAFORO
	 r = pipe(pipefd); //INICIO
    if(pipefd < 0){ 
        perror("Error pipe() "); 
        exit(-1);
    }
    write(pipefd[1], "T", 1); //ESCRIBE EN EL PIPE

	ap=fopen("dataDogs.dat","rb+");//Abre el archivo en modo lectura binaria
    fseek(ap, 0, SEEK_END);//Señal al final del archivo
    numRegistros = (ftell(ap)/(sizeof(struct dogType)+1));//Contador de cantidad de registros en el archivo
    fclose(ap);    
	int sd, sdcli, r,opt = 1;
	int checker = 1;
	pthread_t idhilo[NUM_HILOS];
	struct sockaddr_in server, client;
	socklen_t client_t;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		perror("Error socket");
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(server.sin_zero, 8);

	setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int));

	r = bind(sd, (struct sockaddr*)&server, sizeof(struct sockaddr));
	if (r == -1) {
		perror("Error bind");
		exit(-1);
	}

	r = listen(sd, BACKserverLog);
	if(r == -1){
		perror("Error listen");
		exit(-1);
	}


	int i = 0;
	for(i = 0;i < NUM_HILOS;i++){

		sdcli = accept(sd, (struct sockaddr*)&client, &client_t);
		//printf("entra hilo # %d", i);

		if(sdcli == -1){
			perror("Error accept");
			exit(-1);
		}

		struct cliente cli;
		cli.socket = sdcli;
		cli.dir = client;

		r = pthread_create(&idhilo[i],NULL, (void *)iniciar, (void *)(&cli));
		if(r != 0){
			perror("Error create thread");
			exit (-1);
		}
	}
	close(sd);   
	pthread_mutex_destroy(&mut); //FIN MUTEX
	sem_close(semaforo);
	sem_unlink("semaforo_name"); // FIN SEMAFORO
	return 0;
}
