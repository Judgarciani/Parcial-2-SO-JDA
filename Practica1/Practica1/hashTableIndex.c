#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dogType.h"

#define SIZE 50000
#define BASE (256)

FILE *ap;

struct Node {
   int index;
};

struct Node *hashTable[SIZE]; 

unsigned long getHashValue(const char *s){ //obtiene el hash de un string
	unsigned long m = 49999;
	unsigned long h;
	unsigned const char *us;
	us = (unsigned const char *) s;
	h=0;
	while(*us != '\0'){
		h = (h * BASE + *us) %m;
		us++;
	}
	return h;
}


char *lowerCaseParse(char *try){ //pasa los strings a minuscula
	for(int i = 0; try[i]; i++){
		try[i] = tolower(try[i]);
	}	
	return try;
}

int ingresarHash(char nombre[], struct Node **hashTable,int index){
	unsigned long hashIndex = getHashValue(lowerCaseParse(nombre));//Obtiene hash
	struct Node *registro = (struct Node*) malloc(sizeof(struct Node));//Reserva espacio
	registro->index = index + 1;

	if(hashTable[hashIndex]==NULL){ //Unico en la lista
		hashTable[hashIndex]=(struct Node*) malloc(sizeof(struct Node));
		hashTable[hashIndex] = registro;
		return 0;
	}else{//Hay uno o mas con el mismo nombre
		return hashTable[hashIndex] -> index;
	}
	free(registro);
}

void bajar(struct dogType *p, struct Node **hashTable,int index){
	struct dogType *registro = malloc(sizeof(struct dogType));//Se reserva espacio para la estructura
	int checker = 0;
	int actual = index;
	int count;
	int recorredor = index;
	char comando[45];
	ap=fopen("dataDogs.dat","rb+");//Abre el archivo en modo lectura binaria
	fseek(ap, 0, SEEK_END);//Señal al final del archivo
	count = (ftell(ap)/(sizeof(struct dogType)+1));//Contador de cantidad de registros en el archivo
	fclose(ap);
	while(checker == 0){
		ap=fopen("dataDogs.dat","rb");//Abre archivo
		fseek(ap, ((recorredor-1)*(sizeof(struct dogType)+1)), SEEK_SET);//Pone puntero en registro seleccionado
		fread(registro,sizeof(struct dogType),1,ap);//Lee registro seleccionado
		fclose(ap);//Cierra el archivo
		if(strcmp(lowerCaseParse(p -> nombre),lowerCaseParse(registro -> nombre)) == 0 && actual < count){//Compara
			ap=fopen("dataDogs.dat","ab");//Abre archivo
			fwrite(registro,sizeof(struct dogType),1,ap);//Escribe la estructura en el dat
			fwrite("\n",1,1,ap);//Hace salto de línea
			fclose(ap);//Cierra archivo
			actual = actual + 1;
			recorredor = recorredor + 1;
		}else{
			int i;
			for(i=0;i<=49999;i++){//Actualiza la hash de los registros afectados
				if(hashTable[i]!=NULL){
					if(hashTable[i] -> index >= actual){
						int act = hashTable[i] -> index;
						hashTable[i] -> index = act - (actual-index);
					}
				}
			}
			hashTable[getHashValue(p -> nombre)]->index = count - (actual-index);//Actualiza la hash del nombre recibido
			snprintf(comando, sizeof(comando), "sed -i '%d,%dd' dataDogs.dat",index,(actual-1));//Borra línea en el archivo .dat correspondiente al indice seleccionado
	        system(comando);//Envia comando a sistema		
			checker = 1;
		}
	}
}

void imprimirRegistro(struct dogType *registro){
	//Imprime un registro campo a campo
	printf("Nombre: %s \n" ,registro->nombre);
    printf("Tipo: %s\n",registro->tipo);
	printf("Edad: %i\n",registro->edad);
    printf("Raza: %s\n",registro->raza);
    printf("Estatura: %i\n",registro->estatura);
	printf("Peso: %f\n",registro->peso);
	printf("Sexo: %s\n",registro->sexo);
	printf("\n---------------------------------------------\n");
}

int buscarRegistroHash(char nombre[], struct Node**hashTable){
	unsigned long hashIndex = getHashValue(lowerCaseParse(nombre));//Obtiene hash
	int encontrados = 0;
	if(hashTable[hashIndex]==NULL){
		return 0;//Si no existe devuelve 0
	}else{
		return hashTable[hashIndex] -> index;//Si existe devuelve el índice
	}
}

void moverArriba(int index, struct Node**hashTable){
	int i;
	for(i=0;i<=49999;i++){//Actualiza todos los hash afectados
		if(hashTable[i]!=NULL){
			if(hashTable[i] -> index > index){
				int act = hashTable[i] -> index;
				hashTable[i] -> index = act-1;
			}
		}
	}
}

int subirArchivo(struct Node **hashTable){

	ap=fopen("dataDogs.dat","rb+");
	fseek(ap, 0, SEEK_END);//Señal al final del archivo
	int cant = (ftell(ap)/(sizeof(struct dogType)+1));//Contador de cantidad de registros en el archivo
	printf("%d ",cant);
	int i;
	fclose(ap);

	for(i=0;i<(cant);i++){ //Se cargan los datos del .dat a la hash.	
		struct dogType *perro = (struct dogType*) malloc(sizeof(struct dogType));
		ap=fopen("dataDogs.dat","rb+");
		fseek(ap,(i*(sizeof(struct dogType)+1)),SEEK_SET);
		fread(perro, sizeof(struct dogType),1,ap);
		fclose(ap);
		ingresarHash(perro -> nombre, hashTable, i);
		free(perro);
	}
}
