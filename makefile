practica3: generador p3-dogServer p3-dogClient
	
generador: generador.c 
	gcc -o generadorExe generador.c -I.
p3-dogServer: p3-dogServer.c 
	gcc -o p3-dogServer p3-dogServer.c -lpthread -I.
p3-dogClient: p3-dogClient.c 
	gcc -o p3-dogClient p3-dogClient.c -I.


