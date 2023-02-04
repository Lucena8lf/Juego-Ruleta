#ifndef RULETA_H
#define RULETA_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include<signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>

#define MSG_SIZE 350
#define REFRAN_SIZE 250
#define MAX_CLIENTS 30
#define MAX_GAMES 10
#define MAX_REFRANES 5 //A modo de ejemplo el programa contará con 5 refranes


// char ejemplo[NUM_STRINGS][MAX_LENGTH]

typedef struct Usuario {

	/* Un usuario podrá tener diferentes estados:
		- 1: El usuario está conectado al servidor
		- 2: Ha introducido un usuario válido
		- 3: Ha introducido la contraseña correcta para ese usuario. El usuario se encuentra con una sesión iniciada en el sistema
		- 4: El usuario ha introducido "INICIAR-PARTIDA". Aquí pueden ocurrir 2 cosas:
			4.1 Hay otro jugador disponible para jugar que haya introducido "INICIAR-PARTIDA" y se encuentre en la cola (Se pasaría al estado 5)
			4.2 No hay otro usuario para jugar y se queda a la espera
			Por lo tanto, el estado 4 representa a un jugador en la cola
		-5: El usuario se encuentra en una partida.
	*/
	int estado;

	char nombreUsuario[MSG_SIZE];

	char passwordUsuario[MSG_SIZE];

	int descriptor; //Descriptor del usuario

	int partida; //Clave foránea a la posición en el array de una partida. Si no esta en ninguna partida: -1

	int oponente; //Posicion en el array del cliente contra el que esta jugando el jugador. Si no esta en ninguna partida: -1

	int puntuacion;

} Usuario;

typedef struct Partida {

	int turno;

	int sockets[2]; //Este array contiene los descriptores de los jugadores que estan jugando

	char refran_juego[REFRAN_SIZE];

	char refran_resuelto[REFRAN_SIZE];

	
} Partida;


void salirCliente(int socket, fd_set * readfds, int * numClientes, Usuario arrayClientes[]);
bool compruebaUsuario(char* nombreUsuario);
bool compruebaPassword(char* nombreUsuario, char* password);
int buscaCliente(int descriptor, int *numClientes, Usuario arrayClientes[]);
void escribeUsuario(char* nombreUsuario, char* password);
int jugadorDisponible(int socket,int *numClientes,Usuario arrayClientes[]); //Devuelve '-1' si no hay jugadores disponibles para jugar o la posicion del jugador disponible en el array
char *convierteRefranes(char *refran_resuelto);
int esVocal(char letra);
int cuentaLetra(char *consonante, char *refran_resuelto);
char *actualizaRefran(char *letra, char *refran_juego, char *refran_resuelto);
void salirPartida(int jugador, int *numPartidas, Partida arrayPartidas[]); //Le pasamos los sockets de los dos jugadores de esa partida
bool usuarioRepetido(char *dato, int *numClientes, Usuario arrayClientes[]);

#endif
