#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "hashTableIndex.c"

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

void buscar(){	
	char name[32];
	printf("Inserte el nombre del animal a buscar: ");//Solicita nombre a buscar
	scanf("%s",name);//Recibe nombre por teclado
	int actual = buscarRegistroHash(name,hashTable);
	int checker = 0;
	int count;
	printf("%d",actual);
	ap=fopen("dataDogs.dat","rb+");//Abre el archivo en modo lectura binaria
	fseek(ap, 0, SEEK_END);//Señal al final del archivo
	count = (ftell(ap)/(sizeof(struct dogType)+1));//Contador de cantidad de registros en el archivo
	fclose(ap);//Cierra archivo
	if(actual != 0){
		while(checker==0){
		struct dogType *registro;
		registro = malloc(sizeof(struct dogType));//Se reserva espacio para la estructura
			ap=fopen("dataDogs.dat","rb");//Abre archivo
			fseek(ap, ((actual-1)*(sizeof(struct dogType)+1)), SEEK_SET);//Pone puntero en registro seleccionado
			fread(registro,sizeof(struct dogType),1,ap);//Lee registro seleccionado
			fclose(ap);
			printf("\n---------------------------------------------\n");
			if(strcmp(lowerCaseParse(name),lowerCaseParse(registro -> nombre))==0){//Revisa si coincide
				printf("\nRegistro #%d:\n\n", actual);//Imprime el índice del registro
				imprimirRegistro(registro);//Imprime el registro coincidente
				if(actual < count){
					actual = actual + 1;
				}else{
					checker =1;
				}
			}else{
				checker=1;
			}
			free(registro);
		}
	}else{
		printf("No existe una mascota con ese nombre");
	}
}

void borrar(){
	int count;
	int selected;
	char nombreActual[32];
	char comando[55];
	ap=fopen("dataDogs.dat","rb+");//Abre el archivo en modo lectura binaria
	fseek(ap, 0, SEEK_END);//Señal al final del archivo
	count = (ftell(ap)/(sizeof(struct dogType)+1));//Contador de cantidad de registros en el archivo
	if(count == 0){
	  	printf("No hay registros");
	}else{
		struct dogType *p = malloc(sizeof(struct dogType));
		struct dogType *past = malloc(sizeof(struct dogType));
        printf("%d registros existentes\n", count);//Muestra la cantidad de registros existentes
        fclose(ap);
		printf("Ingrese el índice de la mascota que desea eliminar: ");
        scanf("%d", &selected);//Solicita que se ingrese el indice del registro a borrar
		if(selected<=count){
	        ap=fopen("dataDogs.dat","rb+");//Abre el archivo en lectura binaria
			fseek(ap, ((selected-1)*(sizeof(struct dogType)+1)), SEEK_SET);//Ubica el apuntador en el índice seleccionado
	        fread(past,(sizeof(struct dogType)+1),1,ap);//Lee el registro en el índice
            snprintf(comando, sizeof(comando), "sed -i '%dd' dataDogs.dat",selected);//Borra línea en el archivo .dat correspondiente al indice seleccionado
	        system(comando);//Envia comando a sistema		
			fclose(ap);//Cierra archivo
			//Si era el primero reviso si hay más  
			int estaAca = (hashTable[getHashValue(lowerCaseParse(past -> nombre))] -> index);
			strcpy(nombreActual, past -> nombre);
			if((estaAca)==selected){
                ap=fopen("dataDogs.dat","rb+");//Abre el archivo en lectura binaria
				fseek(ap, ((selected-1)*(sizeof(struct dogType)+1)), SEEK_SET);//Ubica el apuntador en el índice seleccionado
	        	fread(p,(sizeof(struct dogType)+1),1,ap);//Lee el registro en el índice
				if(strcmp(lowerCaseParse(nombreActual),lowerCaseParse(p -> nombre))!=0){
					hashTable[getHashValue(nombreActual)]=NULL;
				}
            }
			moverArriba(selected,hashTable);
	        printf("Registro con nombre %s borrado correctamente",nombreActual);//Confirma
		}else{
			printf("Selección inválida, vuelva a intentarlo.");
		}	
		free(p);
		free(past);
	}
}

void ver(){
	char nombre[32];
	char comando[45];
	int count;
	int selected;
	struct dogType *registro;
	registro = malloc(sizeof(struct dogType));//Se reserva espacio para la estructura
	ap=fopen("dataDogs.dat","rb+");//Abre archivo
	fseek(ap, 0, SEEK_END);//Señal al final del archivo
	count = (ftell(ap)/(sizeof(struct dogType)+1));//Contador de cantidad de registros en el archivo
	if(count == 0){
		printf("No hay registros");
	}else{
		printf("%d\n", count);//Muestra cantidad de registros
		fclose(ap);
		printf("Ingrese el índice de la mascota que desea ver: ");
		scanf("%d", &selected);//Solicita que se ingrese el indice del registro a borrar
		if(selected<=count){
			ap=fopen("dataDogs.dat","rb");//Abre archivo
			fseek(ap, ((selected-1)*(sizeof(struct dogType)+1)), SEEK_SET);//Pone puntero en registro seleccionado
			fread(registro,sizeof(struct dogType),1,ap);//Lee registro seleccionado
			printf("El nombre de la mascota es: %s \n" ,registro->nombre); 
			//imprimirRegistro(registro); //imprime en pantalla luego de buscar en la hash
			snprintf(comando, sizeof(comando), "nano HC/%s%f.txt",registro->nombre, registro->peso);//Crea comando a enviar a consola
			system(comando);//Envía a consola comando para abrir historia clínica
			fclose(ap);
			printf("Historia Clinica abierta correctamente");
		}else{
			printf("Selección inválida, vuelva a intentarlo.\n");
		}
	}
}

void ingresar(){
	char comando[45];
	char *name;
	struct dogType *p =(struct dogType*) malloc(sizeof(struct dogType));//Reserva espacio para estructura
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
	
	int count;
	ap=fopen("dataDogs.dat","rb+");//Abre el archivo en modo lectura binaria
	fseek(ap, 0, SEEK_END);//Señal al final del archivo
	count = (ftell(ap)/(sizeof(struct dogType)+1));//Contador de cantidad de registros en el archivo
	fclose(ap);
	int hash = ingresarHash(p->nombre,hashTable,(count));
	//printf("hash: %i",hash);
	if(hash==0){
		ap=fopen("dataDogs.dat","ab+");//Abre archivo .dat
		//printf("No hay");
		fwrite(p,sizeof(struct dogType),1,ap);//Escribe la estructura en el dat
		fwrite("\n",1,1,ap);//Hace salto de línea
		fclose(ap);
	}else{
		//printf("\nEstá en el %i\n\n", ingresarHash(p->nombre,hashTable,(count)));
		ap=fopen("dataDogs.dat","ab+");//Abre archivo .dat
		fwrite(p,sizeof(struct dogType),1,ap);//Escribe la estructura en el dat
		fwrite("\n",1,1,ap);//Hace salto de línea
		fclose(ap);
		bajar(p,hashTable,ingresarHash(p->nombre,hashTable,(count)));
	}
	printf("Mascota ingresada correctamente");
}

int main(){
	int checker = 1;
	subirArchivo(hashTable);
	while(checker == 1){
		int seleccion;
		system("clear");//Se solicita seleccionar una opción (Ingresar, ver, borrar, etc.)
		printf("\nPara seleccionar una de las siguientes opciones, pulse el número correspondiente seguido de la tecla enter: \n");
		printf("\n1. Ingresar registro\n");
		printf("2. Ver registro\n");
		printf("3. Borrar registro\n");
		printf("4. Buscar registro\n");
		printf("5. Salir\n\n");
		scanf("%i", &seleccion);//Recibe la seleccion
		//Realiza una acción dependiendo de la selección
		if(seleccion == 5){
			esperar();
			exit(0);
		}else if(seleccion == 4){
			esperar();
			buscar();
			esperar();
		}else if(seleccion == 3){
			esperar();
	        borrar();
			esperar();
		}else if(seleccion == 2){
     		esperar();
        	ver();
			esperar();
		}else if(seleccion == 1){
			esperar();
			ingresar();
			esperar();
		};
	};
};

