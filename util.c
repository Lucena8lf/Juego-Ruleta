#include "util.h"

void salirCliente(int socket, fd_set * readfds, int * numClientes, Usuario arrayClientes[]){
  
    char buffer[250];
    int j;

    close(socket);
    FD_CLR(socket,readfds);
    
    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j].descriptor == socket) //Posicionamos la 'j' en la posicion del socket que quiere salir
            break;
    for (; j < (*numClientes) - 1; j++)
        (arrayClientes[j] = arrayClientes[j+1]);
    
    (*numClientes)--;

}

bool compruebaUsuario(char* nombreUsuario){

	bool encontrado = false;
	char temp[MSG_SIZE], *aux;
	FILE *f;

	if((f = fopen("ruleta.txt","r"))==NULL){

		printf("Error: No se pudo acceder a la base de datos de la ruleta.\n");
		exit(-1);
	}

	while((fgets(temp,256,f))!=NULL){ //Ya hemos leido el primero

		//temp tendra la linea con el formato: usuario-contraseña
		aux = strtok(temp,"-"); //Ahora la variable temp tendra el nombre de usuario

		//OJO!!! nombreUsuario viene con un \n
		for(int i=0; i<MSG_SIZE; i++){
		if(nombreUsuario[i] == '\n')
			nombreUsuario[i] = '\0';
	}

		if(strcmp(aux,nombreUsuario) == 0)
			encontrado = true;

	}

	fclose(f);

	if(encontrado)
		return true;
	else
		return false;

}

bool compruebaPassword(char* nombreUsuario, char* password){

	bool encontrado = false;
	char temp[MSG_SIZE], *aux, trashTemp[MSG_SIZE],passwordAux[MSG_SIZE];
	FILE *f;

	if((f = fopen("ruleta.txt","r"))==NULL){

		printf("Error: No se pudo acceder a la base de datos de la ruleta.\n");
		exit(-1);
	}

	while((fgets(temp,256,f))!=NULL){ //Ya hemos leido el primero

		//temp tendra la linea con el formato: usuario-contraseña
		strcpy(trashTemp,temp);
		aux = strtok(trashTemp,"-"); //Ahora la variable temp tendra el nombre de usuario

		//OJO!!! nombreUsuario viene con un \n
		for(int i=0; i<MSG_SIZE; i++){
			if(nombreUsuario[i] == '\n')
				nombreUsuario[i] = '\0';
		}

		if(strcmp(aux,nombreUsuario) == 0){ //Hemos encontrado el usuario
			
			aux = strtok(temp,"-");
			for(int i=0; i<1; i++)
				aux = strtok(NULL,"-");

			// OJO!!!
			strcat(aux,"\n\0");
			
			if(strcmp(aux,password) == 0)
				encontrado = true;

		}

	}

	fclose(f);

	if(encontrado)
		return true;
	else
		return false;

}

int buscaCliente(int descriptor, int *numClientes, Usuario arrayClientes[]){

	/* Esta funcion queremos que a partir de un descriptor dado nos devuelva la posición en la que se encuentra el cliente
	   con ese decriptor en el array 'arrayClientes'
	*/

	int pos;

	for(int i=0; i<(*numClientes); i++){

		if(arrayClientes[i].descriptor == descriptor){
			pos = i;
			break;
		}
	}

	return pos;

}

void escribeUsuario(char* nombreUsuario, char* password){

	FILE *f;

	if((f = fopen("ruleta.txt","a"))==NULL){

		printf("Error: No se pudo abrir el fichero.\n");
		exit(-1);
	}

	//Eliminamos el '\n' de password
	for(int i=0; i<MSG_SIZE; i++){
		if(password[i] == '\n')
			password[i] = '\0';
	}
	//Añadimos el nombre de usuario y la contraseña con ese formato
	fprintf(f,"%s-",nombreUsuario);
	fprintf(f,"%s-\n",password);

	fclose(f);

}

int jugadorDisponible(int socket,int *numClientes,Usuario arrayClientes[]){

	int pos = -1;

	for(int i=0; i<(*numClientes); i++){

		/* Si un cliente se encuentra en el estado 4 significa que esta en la cola buscando partida */
		if(arrayClientes[i].estado == 4 && arrayClientes[i].descriptor != socket){
			pos = i;
			break;
		}
	}

	return pos;
}

char *convierteRefranes(char *refran_resuelto){

	//char refran_juego[REFRAN_SIZE];
	char *refran_juego = malloc (sizeof (char) * REFRAN_SIZE);

	strcpy(refran_juego,refran_resuelto);
	
	for(int i=0; i<strlen(refran_resuelto); i++){
		
		if( isalpha(refran_resuelto[i]) )
			refran_juego[i] = '_';
		else if(refran_resuelto[i] == ' ')
			refran_juego[i] = ' ';
	}

	return refran_juego;
}

int esVocal(char letra) {

    // Convertir a mayúscula para evitar hacer más comparaciones
    char letraEnMayuscula = (char) toupper(letra);

    return letraEnMayuscula == 'A' ||
           letraEnMayuscula == 'E' ||
           letraEnMayuscula == 'I' ||
           letraEnMayuscula == 'O' ||
           letraEnMayuscula == 'U';
}

int cuentaLetra(char *consonante, char *refran_resuelto){

	int nVeces = 0;
	//Devuelve el numero de veces que aparece esa consonante en el refran

	if(!esVocal(consonante[0])){
		for(int i=0; i<strlen(refran_resuelto); i++){

			if( isalpha(refran_resuelto[i]) && !esVocal(refran_resuelto[i]) && (char )toupper(refran_resuelto[i]) == (char)toupper(consonante[0]) )
				nVeces++;
		}

	} else{
		for(int i=0; i<strlen(refran_resuelto); i++){

			if( isalpha(refran_resuelto[i]) && esVocal(refran_resuelto[i]) && (char) toupper(refran_resuelto[i]) == (char) toupper(consonante[0]) )
				nVeces++;
		}

	}

	return nVeces;
}

char *actualizaRefran(char *letra,char *refran_juego, char *refran_resuelto){

	char *refranNuevo = malloc (sizeof (char) * REFRAN_SIZE);

	strcpy(refranNuevo,refran_resuelto);
	
	for(int i=0; i<strlen(refran_resuelto); i++){
		
		if( isalpha(refran_resuelto[i]) && toupper(refran_resuelto[i]) == toupper(letra[0]) )
			refranNuevo[i] = letra[0];
		else if(refran_resuelto[i] == ' ')
			refranNuevo[i] = ' ';
		else if( isalpha(refran_resuelto[i]) && refran_juego[i] == '_' )
			refranNuevo[i] = '_';
	}

	return refranNuevo;
}

void salirPartida(int jugador, int *numPartidas, Partida arrayPartidas[]){

	int j;

	//Re-estructuramos el array de partidas
	 for (j = 0; j < (*numPartidas) - 1; j++)
        if (arrayPartidas[j].turno == jugador) //Posicionamos la 'j' en la posicion de la partida que ha terminado
            break;
    for (; j < (*numPartidas) - 1; j++)
        (arrayPartidas[j] = arrayPartidas[j+1]);
    
    (*numPartidas)--;

    /*
    printf("Partida acabada\n");
    printf("Array de partidas ahora:\n");
    for(int i=0; i<(*numPartidas); i++){
    	printf("Partida <%d>\n",i);
    }
    */
}

bool usuarioRepetido(char *dato, int *numClientes, Usuario arrayClientes[]){

	//Eliminamos \n del dato que ha introducido el cliente
	if(dato[ strlen(dato) -1 ] == '\n')
		dato[ strlen(dato) -1 ] = '\0';

	for(int i=0; i<(*numClientes); i++){

		if( strcmp(arrayClientes[i].nombreUsuario, dato) == 0)
			return true;
	}

	return false;
}