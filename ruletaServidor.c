#include "util.h"

//char refranes[MAX_REFRANES][REFRAN_SIZE] = { {"A quien madruga, Dios le ayuda"}, {"No hay mal que por bien no venga"}, {"En casa del herrero cuchara de palo"},
//{"Ojo por ojo, diente por diente"}, {"El que no corre, vuela"} };
char refranes[MAX_REFRANES][REFRAN_SIZE] = { "de tal palo, tal astilla", "no hay mal que por bien no venga", "en casa del herrero cuchara de palo",
"ojo por ojo, diente por diente", "el que no corre, vuela" };

/*
Pasos a seguir en el servidor:
1. Abrimos el socket
2. Asignamos las propiedades correspondientes a la estructura 'sockname' y llamamos a bind() para asociar nuestro socket al puerto 2050
3. Ponemos en escucha al socket del servidor
4. El servidor acepta peticiones
5. Utilizamos la funcion select() para la concurrencia de clientes
6. Recorremos todos los posibles descriptores y en funcion de los que nos encontremos en el conjunto 'auxfds' realizamos una accion u otra:

	6.1 Si el descriptor encontrado == nuestro descriptor (sd), significa que un nuevo cliente se ha conectado al servidor. En este caso,
	si el cliente es aceptado (puede ser que sobrepase el numero maximo de clientes permitidos) añadiremos al 'arrayClientes' y al conjunto
	'readfds' el nuevo descriptor que representa a ese cliente.

	6.2 Si el descriptor encontrado == 0, significa que desde el servidor se ha introducido algo por teclado. En este caso, solo
	comprobaremos que si desde el servidor se introduce "SALIR" se cerrarán todos los sockets y terminará el servidor.

	6.3 Si el descriptor encontrado no es ni 0 ni nuestro descriptor, significa que algún cliente le está mandando algo al servidor.
*/


int main(){

	srand(time(NULL)); //Generamos semilla

	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd, new_sd;
	struct sockaddr_in sockname, from;
	/* Necesario reservar el buffer a 800 bytes para evitar warnings al compilar */
	char buffer[800], *dato, *nuevoUsuario, *nuevaPassword, bufferTemp[MSG_SIZE], *oculto_aux, comienzaTurno[800],
	 	 cons[MSG_SIZE];
	socklen_t from_len;
    fd_set readfds, auxfds;
   	int salida;
   	Usuario arrayClientes[MAX_CLIENTS];
   	Partida arrayPartidas[MAX_GAMES]; //Array con las partidas que se estan jugando
    int numClientes = 0, numPartidas=0, clienteX, oponente, nConsonantes, nVocales;

    //contadores
    int i,j,k;
	int recibidos;
   	int on,ret;




    /* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}


	// Activaremos una propiedad del socket para permitir· que otros
    	// sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    	// Esto permite· en protocolos como el TCP, poder ejecutar un
    	// mismo programa varias veces seguidas y enlazarlo siempre al
   	 	// mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    	// quedase disponible (TIME_WAIT en el caso de TCP)
    	on=1;
    	ret = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));



	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2050);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}


	/*---------------------------------------------------------------------
		Del las peticiones que vamos a aceptar sólo necesitamos el 
		tamaño de su estructura, el resto de información (familia, puerto, 
		ip), nos la proporcionará el método que recibe las peticiones.
   	----------------------------------------------------------------------*/
	from_len = sizeof (from);


	if(listen(sd,30) == -1){
		perror("Error en la operación de listen");
		exit(1);
	}


	//Inicializar los conjuntos fd_set
    	FD_ZERO(&readfds);
    	FD_ZERO(&auxfds);
    	FD_SET(sd,&readfds);
    	FD_SET(0,&readfds); //Añadimos el descriptor '0' al conjunto para saber el momento en el que se escribe en el servidor



    /*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
		while(1){
            

            //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)
            
            auxfds = readfds;
            
            salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);
            
            if(salida > 0){ //Un nuevo cliente ha activado al servidor

            	//Recorremos todos los posibles descriptores
            	for(i=0; i<FD_SETSIZE; i++){
                    //Buscamos si el socket 'i' se encuentra en el conjunto 'auxfds'
                    if(FD_ISSET(i, &auxfds)) {

                    	if( i == sd){
                            
                            if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                                perror("Error aceptando peticiones");
                            }
                            else
                            {
                                if(numClientes < MAX_CLIENTS){
                                    arrayClientes[numClientes].descriptor = new_sd;

                                    //Una vez que el usuario ha sido conectado cambiamos su estado a 1
                                    arrayClientes[numClientes].estado = 1;

                                    numClientes++;
                                    FD_SET(new_sd,&readfds);
                                
                                	bzero(buffer,sizeof(buffer));
                                    strcpy(buffer, "+OK. Usuario conectado\n");
                                
                                    send(new_sd,buffer,sizeof(buffer),0);

                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,"-ERR. Demasiados clientes conectados\n");
                                    send(new_sd,buffer,sizeof(buffer),0);
                                    close(new_sd);
                                }
                            }
                                
                            }
                            else if( i == 0){

                            	//Se ha introducido información de teclado
                            	bzero(buffer, sizeof(buffer));
                            	fgets(buffer, sizeof(buffer),stdin);
                            
	                            //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
	                            if(strcmp(buffer,"SALIR\n") == 0){
	                             
	                                for (j = 0; j < numClientes; j++){
							        	bzero(buffer, sizeof(buffer));
							        	strcpy(buffer,"Desconexión servidor\n"); 
	                                	send(arrayClientes[j].descriptor,buffer , sizeof(buffer),0);
	                                	close(arrayClientes[j].descriptor);
	                                    FD_CLR(arrayClientes[j].descriptor,&readfds);
	                                }
	                                    close(sd);
	                                    exit(-1);
	                                
	                                
	                            }
	                            //Mensajes que se quieran mandar a los clientes (implementar)
                            

                            }
                            else{ //Mensaje de otro cliente

	                            bzero(buffer,sizeof(buffer));
	                            
	                            recibidos = recv(i,buffer,sizeof(buffer),0);
	                            
	                            if(recibidos > 0){
	                                
	                                if(strcmp(buffer,"SALIR\n") == 0){
	                                    

									    bzero(comienzaTurno,sizeof(comienzaTurno));
									    sprintf(comienzaTurno,"+OK. Desconexion procesada\n");
									    
									  	//El mensaje se le manda al propio cliente que se desconecta
									  	send(i,comienzaTurno,sizeof(comienzaTurno),0);

	                                    clienteX = buscaCliente(i,&numClientes,arrayClientes);

	                                    //Estudiamos todas las opciones posibles
	                                    if(arrayClientes[clienteX].estado == 5){ //Se encuentra en una partida

	                                    	bzero(buffer,sizeof(buffer));
										    sprintf(buffer,"+OK. Ha salido el otro jugador. Finaliza la partida\n");
										  	send(arrayClientes[ arrayClientes[clienteX].oponente ].descriptor,buffer,sizeof(buffer),0);

		                          			/* Al salir pondremos el 'oponente' y 'partida' de cada jugador = -1. También deberemos
		                           			   llamar a la función "salirPartida" para que libere esa partida del array y cambiar el estado
		                            		   de cada uno = 3 (como si se hubieran acabado de loguear)
		                            		*/
											arrayClientes[ arrayClientes[clienteX].oponente ].oponente = -1;
							        		arrayClientes[ arrayClientes[clienteX].oponente ].partida = -1;
							        		arrayClientes[ arrayClientes[clienteX].oponente ].estado = 3;

                               				arrayClientes[clienteX].oponente = -1;
				                   			arrayClientes[clienteX].partida = -1;
				                   			arrayClientes[clienteX].estado = 3;

				                   			salirPartida(i,&numPartidas,arrayPartidas);
	                                    }

	                                    salirCliente(i,&readfds,&numClientes,arrayClientes);
	                                    
	                                }
	                                else{
	                                    
	                                    //Compararemos i(descriptor del cliente) con el cliente del array que tenga su mismo descriptor
	                                    //Ahora 'clienteX' tendra la posicion del cliente con ese descriptor
	                                	clienteX = buscaCliente(i,&numClientes,arrayClientes);
	                                    
	                                    /*En buffer estara el mensaje del cliente.
	                                      Utilizaremos strstr() para detectar la opción del cliente
	                                    */
	                                   	if(strstr(buffer, "USUARIO") != NULL){ //Opción USUARIO

	                                   		/*Usaremos strtok() para delimitar cadenas: strtok(cadena, delimitador) */
	                                   		dato = strtok(buffer," ");
	                                   		for(j=0; j<1; j++)
	                                   			dato = strtok(NULL," ");
	                                   	
	                                   		
	                                   		/*Debemos comprobar que ese usuario se encuentra en nuestro fichero de usuarios*/
	                                   		if(arrayClientes[clienteX].estado != 1){
					                   		bzero(buffer,sizeof(buffer));
		                                  	strcpy(buffer, "-ERR. Ya has iniciado sesion con un usuario\n");
	                                    	
	                                    	send(i,buffer,sizeof(buffer),0);
                            				}
                            				else if(usuarioRepetido(dato,&numClientes,arrayClientes)){
                            					bzero(buffer,sizeof(buffer));
	                                   			strcpy(buffer, "-ERR. Este usuario ya se encuentra logueado en el sistema\n");
	                                   			send(i,buffer,sizeof(buffer),0);
                            				}
	                                   		else if(compruebaUsuario(dato)){
	                                   			strcpy(arrayClientes[clienteX].nombreUsuario, dato);
	                                   			
	                                   			bzero(buffer,sizeof(buffer));
	                                   			strcpy(buffer, "+OK. Usuario correcto\n");
                                
                                				arrayClientes[clienteX].estado = 2;
                                				
                                    			send(i,buffer,sizeof(buffer),0);
                                    		}
                                    		else{

                                    			bzero(buffer,sizeof(buffer));
	                                   			strcpy(buffer, "-ERR. Usuario incorrecto\n");

	                                   			send(i,buffer,sizeof(buffer),0);

                                    		}
                                    	}
                                    	else if(strstr(buffer, "PASSWORD") != NULL){

				                       		dato = strtok(buffer," ");
					                      	for(j=0; j<1; j++)
					                   			dato = strtok(NULL," ");
					                              	
					                                   		
					                       	/*Debemos comprobar que esa contraseña es de ese usuario*/
					                       	if( (compruebaPassword(arrayClientes[clienteX].nombreUsuario,dato)) && arrayClientes[clienteX].estado == 2){
					                      		strcpy(arrayClientes[clienteX].passwordUsuario,dato);

					                       		bzero(buffer,sizeof(buffer));
	                       						strcpy(buffer, "+OK. Usuario validado\n");
                                
                       			 				arrayClientes[clienteX].estado = 3;

                   				 				send(i,buffer,sizeof(buffer),0);

                            				}
                            				else if(arrayClientes[clienteX].estado != 2){
                            					bzero(buffer,sizeof(buffer));

                            					if(arrayClientes[clienteX].estado < 2)
		                                   			strcpy(buffer, "-ERR. Debe introducir un usuario valido antes de introducir una contrasenia\n");
				                                else
				                                	strcpy(buffer,"-ERR. Ya has iniciado sesion con tu usuario\n");

	                                    		send(i,buffer,sizeof(buffer),0);
                            				}                          			
                            				else{
		                                   		bzero(buffer,sizeof(buffer));
		                                   		strcpy(buffer, "-ERR. Error en la validacion\n");
				                                
	                                    		send(i,buffer,sizeof(buffer),0);
	                                  		}

	                                   	} else if(strstr(buffer, "REGISTRO") != NULL){ //Opción REGISTRO

	                                   		/* La sintaxis de REGISTRO es: REGISTRO -u usuario -p password */
	                                   		//Nos interesara la 3 y 5 posicion del string que nos pasen (usuario y password)
	                                   		strcpy(bufferTemp,buffer);
	                                   		nuevoUsuario = strtok(bufferTemp," ");
					                      	for(j=0; j<2; j++)
					                   			nuevoUsuario = strtok(NULL," ");
					                   		
					                   		nuevaPassword = strtok(buffer," ");
					                   		for(j=0; j<4; j++)
					                   			nuevaPassword = strtok(NULL," ");

											
					                   		if(compruebaUsuario(nuevoUsuario)){
					                   			bzero(buffer,sizeof(buffer));
					                   			strcpy(buffer,"-ERR. El nombre de usuario elegido no se encuetra disponible. Por favor, introduzca otro nombre de usuario\n");

					                   			send(i,buffer,sizeof(buffer),0);
					                   		}
					                   		else if(arrayClientes[clienteX].estado != 1){
					                   			bzero(buffer,sizeof(buffer));
		                                   		strcpy(buffer, "-ERR. Ya has iniciado sesion con un usuario\n");

	                                    		send(i,buffer,sizeof(buffer),0);
                            				}                          			
					                   		else{

					                   			/* Se introduce el usuario y la contrasenia en el fichero */
					                   			escribeUsuario(nuevoUsuario,nuevaPassword);

					                   			/* Modificamos los valores de ese cliente */
					                   			arrayClientes[clienteX].estado = 3;
					                   			strcpy(arrayClientes[clienteX].nombreUsuario, nuevoUsuario);
					                   			strcpy(arrayClientes[clienteX].passwordUsuario, nuevaPassword);

					                   			bzero(buffer,sizeof(buffer));
					                   			strcpy(buffer,"+OK. Nuevo usuario registrado\n");

					                   			send(i,buffer,sizeof(buffer),0);
					                   		}

	                                   	} else if(strstr(buffer, "INICIAR-PARTIDA") != NULL){
	                                   		
	                                   		if(arrayClientes[clienteX].estado != 3){
	                                   			bzero(buffer,sizeof(buffer));

	                                   			if(arrayClientes[clienteX].estado < 3)
	                                   				strcpy(buffer, "-ERR. Para iniciar una partida debes antes iniciar sesion\n");
	                                   			else if(arrayClientes[clienteX].estado == 4)
	                                   				strcpy(buffer, "-ERR. Ya te encuentras en la cola de espera. Por favor, espera mientras se encuentran a mas jugadores\n");
	                                   			else
	                                   				strcpy(buffer, "-ERR. Ya te encuentras jugando una partida\n");

	                                    		send(i,buffer,sizeof(buffer),0);

	                                   		}
	                                   		else{
												
		                                   		/* Tenemos 2 opciones: se inicie partida o el cliente vaya a la cola de espera */
		                                   		arrayClientes[clienteX].estado = 4;

		                                   		oponente = jugadorDisponible(i,&numClientes,arrayClientes);
		                                   		if(oponente == -1){
		                                   			bzero(buffer,sizeof(buffer));
						                   			strcpy(buffer,"+OK. Peticion recibida. Quedamos a la espera de mas jugadores\n");

						                   			send(i,buffer,sizeof(buffer),0);

		                                   		} else{ //Hay otro jugador para jugar

		                                   			if(numPartidas < MAX_GAMES){

		                                   				/*-----------------------------------------------------------------------
															Rellenamos datos de la partida
														------------------------------------------------------------------------ */
		                                   				//Asignamos al oponente primero ya que el turno va en funcion del orden de conexion
		                                   				arrayPartidas[numPartidas].sockets[0] = arrayClientes[oponente].descriptor;
		                                   				arrayPartidas[numPartidas].sockets[1] = i;
		                                   				arrayPartidas[numPartidas].turno = arrayClientes[oponente].descriptor;
		                                   				strcpy(arrayPartidas[numPartidas].refran_resuelto,refranes[rand() % MAX_REFRANES]);
		                                   				
		                                   				strcpy(arrayPartidas[numPartidas].refran_juego,convierteRefranes(arrayPartidas[numPartidas].refran_resuelto));
		                                   				

		                                   				/* ----------------------------------------------------------------------- */

		                                   				oculto_aux = arrayPartidas[numPartidas].refran_juego;
		                                   				numPartidas++;
			                                   			arrayClientes[clienteX].estado = 5;
			                                   			arrayClientes[oponente].estado = 5;

			                                   			bzero(buffer,sizeof(buffer));
			                                   			bzero(comienzaTurno, sizeof(comienzaTurno));
			                                   			sprintf(buffer,"+OK. Empieza la partida. FRASE: %s\n\n+OK. Turno del otro jugador\n",oculto_aux);
			                                   			sprintf(comienzaTurno,"+OK. Empieza la partida. FRASE: %s\n\n+OK. Turno de partida\n",oculto_aux);

							                   			send(i,buffer,sizeof(buffer),0);
							                   			send(arrayClientes[oponente].descriptor,comienzaTurno,sizeof(comienzaTurno),0);

							                   			/* Antes de salir asignaremos a cada jugador la posicion en el array de su oponente
														   y el numero de partida. Tambien inicializaremos la puntuacion de cada uno a 0
							                   			*/
							                   			arrayClientes[clienteX].oponente = oponente;
							                   			arrayClientes[clienteX].partida = numPartidas - 1;
							                   			arrayClientes[clienteX].puntuacion = 0;

							                   			arrayClientes[oponente].oponente = clienteX;
							                   			arrayClientes[oponente].partida = numPartidas - 1;
							                   			arrayClientes[oponente].puntuacion = 0;

						                   			}

		                                   		}
		                                   	}

	                                   	} else if( strstr(buffer,"CONSONANTE") != NULL || strstr(buffer,"VOCAL") != NULL || 
	                                   			   strstr(buffer,"PUNTUACION") != NULL || strstr(buffer,"RESOLVER") != NULL){ //Opciones dentro de una partida

	                                   		if(arrayClientes[clienteX].estado != 5 || arrayClientes[clienteX].partida == -1){
	                                   			bzero(buffer,sizeof(buffer));
		                                   		strcpy(buffer, "-ERR. No estas en una partida\n");

	                                    		send(i,buffer,sizeof(buffer),0);
	                                   		}
	                                   		else if(arrayPartidas[ arrayClientes[clienteX].partida ].turno != arrayClientes[clienteX].descriptor){ //Comprobamos si es su turno
	                                   			bzero(buffer,sizeof(buffer));
		                                   		strcpy(buffer, "-ERR. Debe esperar su turno\n");

	                                    		send(i,buffer,sizeof(buffer),0);

	                                    	} else{

	                                   			if(strstr(buffer,"CONSONANTE") != NULL){ //Opcion consonante
	                                   				
	                                   				/* Si un jugador pide una consonante deberemos buscar en el refran resuelto el
	                                   				   número de veces que aparece esa letra y enviarselo a ambos jugadores, dando 50
	                                   				   puntos por cada aparicion al jugador que lo pidió
	                                   				*/

	                                   				dato = strtok(buffer," ");
	                                   				for(j=0; j<1; j++)
	                                   					dato = strtok(NULL," ");

	                                   				if(esVocal(dato[0])){
	                                   					bzero(buffer,sizeof(buffer));
	                                    			
	                                    				sprintf(buffer,"-ERR. No has introducido una consonante\n");
	                                    				send(i,buffer,sizeof(buffer),0);
	                                   				}
	                                   				else{
	                                   				
		                                   				//OJO!!! la letra viene con un \n
														for(int i=0; i<sizeof(dato); i++){
															if(dato[i] == '\n')
																dato[i] = '\0';
														}
														
														
		                                   				nConsonantes = cuentaLetra(dato,arrayPartidas[ arrayClientes[clienteX].partida ].refran_resuelto);

		                                   				//Ahora debemos cambiar el refran_juego para que aparezca las que ha adivinado
		                                   				if(nConsonantes > 0)
		                                   					strcpy(arrayPartidas[ arrayClientes[clienteX].partida ].refran_juego, actualizaRefran(dato, arrayPartidas[ arrayClientes[clienteX].partida ].refran_juego,arrayPartidas[ arrayClientes[clienteX].partida ].refran_resuelto));
		                                   				

		                                   				//Actualizamos puntos de ese jugador
		                                   				arrayClientes[clienteX].puntuacion += nConsonantes * 50;

		                                   				//Debemos comprobar que no se haya completado ya todo el refrán sin haber indicado la opción "RESOLVER"
		                                   				if(strcmp(arrayPartidas[ arrayClientes[clienteX].partida ].refran_juego, arrayPartidas[ arrayClientes[clienteX].partida ].refran_resuelto) == 0){

		                                   					bzero(buffer,sizeof(buffer));
			                                   				sprintf(buffer,"+OK. Partida finalizada. FRASE: %s. Ha ganado el jugador %s con %d puntos\n"
			                                   						,arrayPartidas[ arrayClientes[clienteX].partida ].refran_resuelto, arrayClientes[clienteX].nombreUsuario, arrayClientes[clienteX].puntuacion);
			                                   				send(i,buffer,sizeof(buffer),0);
			                                   				send(arrayClientes[ arrayClientes[clienteX].oponente ].descriptor,buffer,sizeof(buffer),0);
			                                   				
			                                   				/* Al salir pondremos el 'oponente' y 'partida' de cada jugador = -1. También deberemos
			                                   				   llamar a la función "salirPartida" para que libere esa partida del array y cambiar el estado
			                                   				   de cada uno = 3 (como si se hubieran acabado de loguear)
			                                   				*/
															arrayClientes[ arrayClientes[clienteX].oponente ].oponente = -1;
								                   			arrayClientes[ arrayClientes[clienteX].oponente ].partida = -1;
								                   			arrayClientes[ arrayClientes[clienteX].oponente ].estado = 3;

			                                   				arrayClientes[clienteX].oponente = -1;
								                   			arrayClientes[clienteX].partida = -1;
								                   			arrayClientes[clienteX].estado = 3;

								                   			salirPartida(i,&numPartidas,arrayPartidas);
		                                   				} else{

			                                   				/* Por último, indicamos al nuevo jugador que es su turno y cambiamos la variable turno
			                                    			   de esa partida
			                                    			*/
			                                    			arrayPartidas[ arrayClientes[clienteX].partida ].turno = arrayClientes[ arrayClientes[clienteX].oponente ].descriptor;

			                                    			sprintf(cons,"+OK. %s",dato);

															bzero(buffer,sizeof(buffer));
															bzero(comienzaTurno,sizeof(comienzaTurno));
															sprintf(buffer,"%s aparece %d veces. FRASE: %s\n\n.+OK Turno del otro jugador",cons,nConsonantes,arrayPartidas[ arrayClientes[clienteX].partida ].refran_juego);
															sprintf(comienzaTurno,"%s aparece %d veces. FRASE: %s\n\n+OK. Turno de partida\n",cons,nConsonantes,arrayPartidas[ arrayClientes[clienteX].partida ].refran_juego);
			                                    			send(i,buffer,sizeof(buffer),0);
			                                    			send(arrayClientes[ arrayClientes[clienteX].oponente ].descriptor,comienzaTurno,sizeof(comienzaTurno),0);

			                                    		}
		                                    		}

	                                   			}
	                                   			else if(strstr(buffer,"VOCAL")){

	                                   				dato = strtok(buffer," ");
		                                   			for(j=0; j<1; j++)
		                                   				dato = strtok(NULL," ");

	                                   				if(arrayClientes[clienteX].puntuacion < 50){
	                                   					bzero(buffer,sizeof(buffer));
	                                    			
	                                    				sprintf(buffer,"+OK. No tienes puntuacion suficiente\n");
	                                    				send(i,buffer,sizeof(buffer),0);
	                                   				}
	                                   				else if(!esVocal(dato[0])){
	                                   					bzero(buffer,sizeof(buffer));
	                                    			
	                                    				sprintf(buffer,"-ERR. No has introducido una vocal.\n");
	                                    				send(i,buffer,sizeof(buffer),0);
	                                   				}
	                                   				else{

		                                   				//OJO!!! la letra viene con un \n
														for(int i=0; i<sizeof(dato); i++){
															if(dato[i] == '\n')
																dato[i] = '\0';
														}

														nConsonantes = cuentaLetra(dato,arrayPartidas[ arrayClientes[clienteX].partida ].refran_resuelto);

		                                   				//Ahora debemos cambiar el refran_juego para que aparezca las que ha adivinado
		                                   				if(nConsonantes > 0){

		                                   					strcpy(arrayPartidas[ arrayClientes[clienteX].partida ].refran_juego, actualizaRefran(dato, arrayPartidas[ arrayClientes[clienteX].partida ].refran_juego,arrayPartidas[ arrayClientes[clienteX].partida ].refran_resuelto));
		                                   				}

		                                   				//Actualizamos puntos de ese jugador
	                                   					arrayClientes[clienteX].puntuacion -= 50;

	                                   					if(strcmp(arrayPartidas[ arrayClientes[clienteX].partida ].refran_juego, arrayPartidas[ arrayClientes[clienteX].partida ].refran_resuelto) == 0){

		                                   					bzero(buffer,sizeof(buffer));
			                                   				sprintf(buffer,"+OK. Partida finalizada. FRASE: %s. Ha ganado el jugador %s con %d puntos\n"
			                                   						,arrayPartidas[ arrayClientes[clienteX].partida ].refran_resuelto, arrayClientes[clienteX].nombreUsuario, arrayClientes[clienteX].puntuacion);
			                                   				send(i,buffer,sizeof(buffer),0);
			                                   				send(arrayClientes[ arrayClientes[clienteX].oponente ].descriptor,buffer,sizeof(buffer),0);
			                                   				
			                                   				/* Al salir pondremos el 'oponente' y 'partida' de cada jugador = -1. También deberemos
			                                   				   llamar a la función "salirPartida" para que libere esa partida del array y cambiar el estado
			                                   				   de cada uno = 3 (como si se hubieran acabado de loguear)
			                                   				*/
															arrayClientes[ arrayClientes[clienteX].oponente ].oponente = -1;
								                   			arrayClientes[ arrayClientes[clienteX].oponente ].partida = -1;
								                   			arrayClientes[ arrayClientes[clienteX].oponente ].estado = 3;

			                                   				arrayClientes[clienteX].oponente = -1;
								                   			arrayClientes[clienteX].partida = -1;
								                   			arrayClientes[clienteX].estado = 3;

								                   			salirPartida(i,&numPartidas,arrayPartidas);

		                                   				} else{

			                                    			/* Por último, indicamos al nuevo jugador que es su turno y cambiamos la variable turno
		                                    			   	   de esa partida
			                                    			*/
			                                    			arrayPartidas[ arrayClientes[clienteX].partida ].turno = arrayClientes[ arrayClientes[clienteX].oponente ].descriptor;

			                                    			sprintf(cons,"+OK. %s",dato);

			                                    			bzero(buffer,sizeof(buffer));
															bzero(comienzaTurno,sizeof(comienzaTurno));
															sprintf(buffer,"%s aparece %d veces. FRASE: %s\n\n.+OK Turno del otro jugador\n",cons,nConsonantes,arrayPartidas[ arrayClientes[clienteX].partida ].refran_juego);
															sprintf(comienzaTurno,"%s aparece %d veces. FRASE: %s\n\n+OK. Turno de partida\n",cons,nConsonantes,arrayPartidas[ arrayClientes[clienteX].partida ].refran_juego);
			                                    			send(i,buffer,sizeof(buffer),0);
			                                    			send(arrayClientes[ arrayClientes[clienteX].oponente ].descriptor,comienzaTurno,sizeof(comienzaTurno),0);


			                                    		}

													}

	                                   			} 
	                                   			else if(strstr(buffer,"PUNTUACION")){

	                                   				bzero(buffer,sizeof(buffer));
	                                   				sprintf(buffer,"+OK. Tu puntuacion es: %d\n",arrayClientes[clienteX].puntuacion);
	                                   				send(i,buffer,sizeof(buffer),0);
	                                   			}
	                                   			else if(strstr(buffer,"RESOLVER")){

	                                   				//INICIAR-PARTIDA

	                                   				dato = strtok(buffer," ");
	                                   				dato = strtok(NULL,"\n");
	                                   				

	                                   				//OJO!!! la respuesta viene con un \n
													for(int i=0; i<sizeof(dato); i++){
														if(dato[i] == '\n')
															dato[i] = '\0';
													}
													
													if(strcmp( arrayPartidas[ arrayClientes[clienteX].partida ].refran_resuelto , dato ) == 0){
														
														bzero(buffer,sizeof(buffer));
		                                   				sprintf(buffer,"+OK. Partida finalizada. FRASE: %s. Ha ganado el jugador %s con %d puntos\n"
		                                   						,arrayPartidas[ arrayClientes[clienteX].partida ].refran_resuelto, arrayClientes[clienteX].nombreUsuario, arrayClientes[clienteX].puntuacion);
		                                   				send(i,buffer,sizeof(buffer),0);
		                                   				send(arrayClientes[ arrayClientes[clienteX].oponente ].descriptor,buffer,sizeof(buffer),0);
		                                   				
		                                   				/* Al salir pondremos el 'oponente' y 'partida' de cada jugador = -1. También deberemos
		                                   				   llamar a la función "salirPartida" para que libere esa partida del array y cambiar el estado
		                                   				   de cada uno = 3 (como si se hubieran acabado de loguear)
		                                   				*/
														arrayClientes[ arrayClientes[clienteX].oponente ].oponente = -1;
							                   			arrayClientes[ arrayClientes[clienteX].oponente ].partida = -1;
							                   			arrayClientes[ arrayClientes[clienteX].oponente ].estado = 3;

		                                   				arrayClientes[clienteX].oponente = -1;
							                   			arrayClientes[clienteX].partida = -1;
							                   			arrayClientes[clienteX].estado = 3;

							                   			salirPartida(i,&numPartidas,arrayPartidas);

							                   			//ESTADO
							                   			

													} else{

														bzero(buffer,sizeof(buffer));
		                                   				sprintf(buffer,"+OK. Partida finalizada. FRASE: %s. No se ha acertado la frase\n"
		                                   						,arrayPartidas[ arrayClientes[clienteX].partida ].refran_resuelto);
		                                   				send(i,buffer,sizeof(buffer),0);
		                                   				send(arrayClientes[ arrayClientes[clienteX].oponente ].descriptor,buffer,sizeof(buffer),0);

		                                   				//Al igual que antes al terminar la partida debemos modificar variables de los jugadores
		                                   				arrayClientes[ arrayClientes[clienteX].oponente ].oponente = -1;
							                   			arrayClientes[ arrayClientes[clienteX].oponente ].partida = -1;
							                   			arrayClientes[ arrayClientes[clienteX].oponente ].estado = 3;

		                                   				arrayClientes[clienteX].oponente = -1;
							                   			arrayClientes[clienteX].partida = -1;
							                   			arrayClientes[clienteX].estado = 3;

							                   			salirPartida(i,&numPartidas,arrayPartidas);

													}
	                                   			}
	                                   		}
	                                   	} 

	                               }
	                                                          
	                                
	                            }

	                        }
	                    }
	                }
	            }
	        }



    close(sd);
	return 0;
}
