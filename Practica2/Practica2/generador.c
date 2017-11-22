#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

//Crea la estructura dogType
struct dogType {
	char nombre[32];
	char tipo[32];
	int edad;
	char raza[16];
	int estatura;
	float peso;
	char sexo[1];
};

//Genera un aleatorio de tipo entero entre a y b
int aleatorioEntero(int a, int b)
{
	int x=(a + (rand() % (int)(b - a + 1)));
	if(x != 10 && x != 13){
		return x;
	}else{
		return aleatorioEntero(a,b);
	}
}

//Genera un aleatorio de tipo real entre a y b
float aleatorioReal(int a, int b)
{
    	int x = ((float)aleatorioEntero(a, b) + ((float) rand() / (RAND_MAX)) + 1);
	if(x != 63.760475){
		return x;
	}else{
		return aleatorioReal(a,b);
	}
}


int main(){

	//Crea el archivo donde se guardan las mascotas
    
	FILE *ap = fopen("dataDogs.dat", "w");
	fwrite("",0,0,ap);

	//Abre los archivos donde están almacenados los nombres y las razas
	FILE *nombres = fopen("nombresMascotas.txt","r");
	FILE *razas = fopen("razas.txt","r");
	
	//Variable del iterador
	int i;
	
	//Variables señaladoras para saber si los archivos llegan al final.
	int c1=0;
	int c2=0;
	
	fclose(ap);

	for(i=0;i<17000;i++){//1716
		
		//Separa espacio de memoria para la estructura
		struct dogType *data = malloc(sizeof(struct dogType));


		//Se lee siguiente nombre y raza
		c1=fscanf(nombres," %[^\n]",data -> nombre);
		c2=fscanf(razas," %[^\n]",data -> raza);

		//Genera datos aleatorios para edad, estatura y peso
		data -> edad = aleatorioEntero(1,20);
		data -> estatura = aleatorioEntero(5,100);
		data -> peso = aleatorioReal(3,100);
		
		//Si el iterador es divisible entre 3, será macho. De lo contrario, será hembra
		if(i%3 == 0){
			strcpy(data -> sexo, "M");
       		}else{
			strcpy(data -> sexo, "H");
       		}
		
		//Los nombres en posición par son perros, en posición impar son gatos
		if(i%2 == 0){
			strcpy(data -> tipo, "Perro");
        	}else{
			strcpy(data -> tipo, "Gato");
        	}
			
		//Si se llega al final del archivo con los nombres o las razas, manda el apuntador al principio del mismo
	    	if(c1==EOF){
	       		rewind(nombres);
	    	}
	    	if(c2==EOF){
			rewind(razas);
    		}
			
		//Abre el archivo e inserta la estructura del perro creado
		ap = fopen("dataDogs.dat", "ab");
		fwrite(data, sizeof(struct dogType), 1, ap);
		//Inserta un salto de línea y cierra el archivo
		fwrite("\n",1,1,ap);
		fclose(ap);
		
	}			
}
