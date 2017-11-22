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

//Funciones de envio y recepcion de datos segun tipo de respuesta

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
	//Solicita los datos de la mascota
	printf("Nombre: ");
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
	send(sd,p,sizeof(struct dogType),0);//Envía la estructura
	recibirImprimirChar(sd);//Recibe e imprime confirmación
	free(p);//Libera espacio
}

void ver(int sd){
	//Declaración de variables
	char nombre[32];
	float peso;
	float count =soloRecibirInt(sd);//Recibe tamaño
	if (count == 0){
		recibirImprimirChar(sd);//Si el tamaño es 0 recibe mensaje de error del servidor
	}else{
		recibirImprimirInt(sd);//Recibe e imprime cantidad de registros
		recibirImprimirChar(sd);//Recibe e imprime petición de ingresar índice
		int selected;
		scanf("%d",&selected);//Recibe de teclado índice deseado por usuario
		send(sd,&selected,sizeof(int),0);//Envía índice a ver
		if(selected<=count){
			//Recibe datos necesarios
			recv(sd,buffer,sizeof(nombre),0);
			recv(sd,&peso,sizeof(float),0);	
	
	  		strcpy(nombre,buffer);
			char ruta[45];
			sprintf(ruta, "%s%f.txt",nombre, peso);
			FILE *archivoHistoria;
			archivoHistoria = fopen(ruta,"w+");//Crea archivo temporal de historia clínica
			int tam;
			recv(sd,&tam,sizeof(int),0);//Recibe el tamaño del archivo original
			printf("%d",tam);//Imprime el tamaño
			int i = 0;
			//Función para recibir el archivo del servidor según su tamaño (parte a parte)
			for (i = 0; i < tam; ++i)
			{
				char aux;
				recv(sd,&aux,sizeof(char),0);
				fputc(aux, archivoHistoria);
			}
			fclose(archivoHistoria);//Cierra archivo

			
			char nano[80];
			sprintf(nano, "nano %s", ruta);
			system(nano);//Abre archivo en nano para editarlo

			archivoHistoria = fopen(ruta,"r+");//Abre archivo temporal
			fseek(archivoHistoria, 0, SEEK_END);//Puntero al final
			tam = ftell(archivoHistoria);//Tamaño
			send(sd, &tam,sizeof(int),0);//Envía tamaño
			fseek(archivoHistoria, 0, SEEK_SET);//Devuelve puntero
			//Función para enviar el archivo al cliente según su tamaño (parte a parte)
			for (i = 0; i < tam; ++i)
			{
				char aux;
				aux = fgetc(archivoHistoria);
				send(sd, &aux,sizeof(char),0);
			}
			
			remove(ruta);//Elimina archivo temporal

			recibirImprimirChar(sd);//Recibe confirmación
		}else{
			recibirImprimirChar(sd);//Recibe error
		}

	}

}

void borrar(int sd){
	int count =soloRecibirInt(sd);//Recibe del servidor la cantidad de registros existentes
	char c;
	if (count == 0){
		recibirImprimirChar(sd);//Si no hay registros, recibe este mensaje
	}else{
		recibirImprimirChar(sd);//Imprime la cantidad de registros existentes
		int selected=recibir_ContestarInt(sd);//Recibe mensaje que solicita índice, y lo responde con el índice ingresado por teclado
		if(selected<=count){
			recibirImprimirChar(sd);//Recibe confirmación
		}else{
			recibirImprimirChar(sd);//Recibe error
		}
	}
}

void buscar(int sd){

	char selected[32];
	int count = soloRecibirInt(sd);//Recibe tamaño
	printf("count: %d",count);//Imprime tamaño
	recibirImprimirChar(sd);//Recibe solicitud de insertar nombre
	scanf("%s",selected);//Recibe nombre por teclado
	send(sd, selected, sizeof(selected),0);//Envía nombre al servidor
	int actual = 0;
	printf("\n---------------------------------------------\n");
    	for(actual=0;actual<count;actual++){
		int a = soloRecibirInt(sd);//Recibe dígito de confirmación
		if(a==1){//Si el dígito de confirmación es afirmativo
			struct dogType *registro = malloc(sizeof(struct dogType));//Reserva espacio para la estructura
			recv(sd,registro,sizeof(struct dogType),0);//Recibe estructura
			imprimirRegistro(registro,actual);//Imprime registro de estructura recibida
			free(registro);//Libera espacio en memoria
		}
	}
}

int main(int argc, char *argv[]){
	sd = socket(AF_INET,SOCK_STREAM,0);
	if(sd < 0){//Si hay error en el socket
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
    	while(seleccion != 5){//Menu principal
    		printf("Para seleccionar una de las siguientes opciones, pulse el número correspondiente seguido de la tecla enter:\n\
			\n1. Ingresar registro\
			\n2. Ver registro\
			\n3. Borrar registro\
			\n4. Buscar registro\
			\n5. Salir\n\n");
	   	scanf("%f", &seleccion);//Recibe seleccion de opcion
	  	send(sd,&seleccion,sizeof(float),0);//Envia seleccion al servidor
		switch ((int)seleccion){//Dependiendo de la selección, ejecuta una de las siguientes opciones
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
	    		default://Seleccion incorrecta
	        		send(sd,"Mal",30,0);
	  	}

	}
	close(sd);

}

