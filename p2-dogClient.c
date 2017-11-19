#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

struct dogType {
	char nombre[32];
	char tipo[32];
	int edad;
	char raza[16];
	int estatura;
	float peso;
	char sexo[1];
};

# define PORT 3535
int sd,r,seleccion;
char buffer[16];
float bufferFloat;
int bufferInt;
struct sockaddr_in client;
socklen_t tama;


/*void *soloRecibirChar(int sd){
	  recv(sd,buffer,266,0);
	  return(buffer);
}*/

void imprimirRegistro(struct dogType *mascota,int index){ //imprime la mascota si se le pasa la mascota 

	printf("Indice: %d\n",index+1);
	printf("Nombre: %s \n",mascota->nombre);
	printf("Tipo: %s\n",mascota->tipo);	
	printf("Edad: %i\n",mascota->edad);
	printf("Raza: %s\n",mascota->raza);
	printf("Estatura: %i\n",mascota->estatura);
	printf("Peso: %f\n",mascota->peso);
	printf("Sexo: %s\n",mascota->sexo);
	printf("\n---------------------------------------------\n");		
}

float soloRecibirfloat(int sd){
      recv(sd,&bufferFloat,sizeof(float),0);
	  return(bufferFloat);
}

int soloRecibirInt(int sd){
      recv(sd,&bufferInt,sizeof(int),0);
	  return(bufferInt);
}

void recibirImprimirChar(int sd){
      recv(sd,buffer,266,0);
	  printf("\n%s\n" , buffer);
}

void recibirImprimirFloat(int sd){
      recv(sd,&bufferFloat,sizeof(float),0);
	  printf("\n%f\n" , bufferFloat);
}

void recibirImprimirInt(int sd){
      recv(sd,&bufferInt,sizeof(int),0);
	  printf("\n%d\n" , bufferInt);
}

void recibir_ContestarChar(int sd, int tam){
      char respuesta[tam];
      recv(sd,buffer,266,0);
	  printf("\n%s\n" , buffer);
	  scanf("%s",respuesta);
	  send(sd,respuesta,sizeof(respuesta),0);
      strcpy(respuesta,"");
}

float recibir_Contestarfloat(int sd){
      float respuesta;
      recv(sd,buffer,266,0);
	  printf("\n%s\n" , buffer);
	  scanf("%f", &respuesta);
	  send(sd,&respuesta,sizeof(float),0);
	  return respuesta;
}

int recibir_ContestarInt(int sd){
      int respuesta;
      recv(sd,buffer,266,0);
	  printf("\n%s\n" , buffer);
	  scanf("%d", &respuesta);
	  send(sd,&respuesta,sizeof(int),0);
	  return respuesta;
}

void esperar(){ //Función que solicita presionar enter para continuar
	printf("\nPresione ENTER para continuar...\n");

	char prev = 0;
	while(1)
	{
		//Toma de entrada cualquier cosa para continuar
		char c = getchar();
		if(c == '\n' && prev == c)
		{
			break;
		}
		prev = c;
	}
}

void ingresar(int sd){
   
    char nombre[32];
    char tipo[32];
    int edad;
    char raza[16];
    int estatura;
    float peso;
    char sexo[1];
    recibirImprimirChar(sd);
	struct dogType *p =(struct dogType*) malloc(sizeof(struct  dogType));//Reserva espacio para estructura
	printf("Nombre: ");//Solicita los datos de la mascota
	scanf("%s", p->nombre);
	printf("Tipo: ");
	scanf("%s",p -> tipo);
	printf("Edad: ");
	scanf("%i",&p -> edad);
	printf("Raza: ");
	scanf("%s",p -> raza);
	printf("Estatura: ");
	scanf("%i",&p -> estatura);
	printf("Peso: ");
	scanf("%f",&p -> peso);
	printf("Sexo: ");
	scanf("%s",p -> sexo);
	send(sd,p,sizeof(struct dogType),0);
	recibirImprimirChar(sd);
	free(p);
}

void ver(int sd){
	char nombre[32];
	float peso;
	//recibirImprimirChar(sd);
	float count =soloRecibirInt(sd);
	if (count == 0){
		recibirImprimirChar(sd);
	}else{
		recibirImprimirInt(sd);
		recibirImprimirChar(sd);
		int selected;
		scanf("%d",&selected);
		send(sd,&selected,sizeof(int),0);
		if(selected<=count){

			recv(sd,buffer,sizeof(nombre),0);
			recv(sd,&peso,sizeof(float),0);	
	
	  		strcpy(nombre,buffer);
			char ruta[45];
			sprintf(ruta, "%s%f.txt",nombre, peso);
			FILE *archivoHistoria;
			archivoHistoria = fopen(ruta,"w+");
			int tam;
			recv(sd,&tam,sizeof(int),0);
			printf("%d",tam);
			int i = 0;

			for (i = 0; i < tam; ++i)
			{
				char aux;
				recv(sd,&aux,sizeof(char),0);
				fputc(aux, archivoHistoria);
			}
			fclose(archivoHistoria);

			
			char nano[80];
			sprintf(nano, "nano %s", ruta);
			system(nano);

			archivoHistoria = fopen(ruta,"r+");
			fseek(archivoHistoria, 0, SEEK_END);
			tam = ftell(archivoHistoria);
			send(sd, &tam,sizeof(int),0);
			fseek(archivoHistoria, 0, SEEK_SET);

			for (i = 0; i < tam; ++i)
			{
				char aux;
				aux = fgetc(archivoHistoria);
				send(sd, &aux,sizeof(char),0);
			}
			
			remove(ruta);

			recibirImprimirChar(sd);
		}else{
			recibirImprimirChar(sd);
		}

	}

}

void borrar(int sd){
	//recibirImprimirChar(sd);
	int count =soloRecibirInt(sd);
	char c;
	if (count == 0){
		recibirImprimirChar(sd);
	}else{
		recibirImprimirChar(sd);
		int selected=recibir_ContestarInt(sd);
		if(selected<=count){
			recibirImprimirChar(sd);
		}else{
			recibirImprimirChar(sd);
		}
	}
}

void buscar(int sd){

	char selected[32];
	int count = soloRecibirInt(sd);
	printf("count: %d",count);
	recibirImprimirChar(sd);
	scanf("%s",selected);
	send(sd, selected, sizeof(selected),0);
	int actual = 0;
	printf("\n---------------------------------------------\n");
    for(actual=0;actual<count;actual++){
		int a = soloRecibirInt(sd);
		if(a==1){
			struct dogType *registro = malloc(sizeof(struct dogType));
			recv(sd,registro,sizeof(struct dogType),0);
			imprimirRegistro(registro,actual);
			free(registro);
		}
	}
}

int main(int argc, char *argv[]){
	sd = socket(AF_INET,SOCK_STREAM,0);
	if(sd < 0){
        perror("\n-->Error en socket():");
        exit(-1);
    }
	client.sin_family = AF_INET;
	client.sin_port = htons(PORT);
	client.sin_addr.s_addr = inet_addr(argv[1]);
  	bzero(client.sin_zero, 8);
	tama = sizeof(struct sockaddr_in);
	r = connect(sd, (struct sockaddr*)& client,tama);
	if(r < 0){
        perror("\n-->Error en connect(): ");
        exit(-1);
    }
    float seleccion = 0;
    while(seleccion != 5){
    	printf("Para seleccionar una de las siguientes opciones, pulse el número correspondiente seguido de la tecla enter:\n\
			\n1. Ingresar registro\
			\n2. Ver registro\
			\n3. Borrar registro\
			\n4. Buscar registro\
			\n5. Salir\n\n");
	   	scanf("%f", &seleccion);
	  	send(sd,&seleccion,sizeof(float),0);
		switch ((int)seleccion){
	    case 1: //Ingresar
	        esperar();
	        ingresar(sd);
	        esperar();
	        break; 
	    case 2://Ver
	        esperar();
	        ver(sd);
	        esperar();
	        break; 
	    case 3://Borrar
	        esperar();
			borrar(sd);
			esperar();
	        break;  
	    case 4://Buscar
	        esperar();
			buscar(sd);
			esperar();
	        break;  
	    case 5://Salir
	        send(sd,"Salir",30,0);
	        break;  
	    default://oops
	        send(sd,"Mal",30,0);
	  }

	}
	close(sd);

}

