## Introducción

Este es un proyecto de la asignatura de Redes. Consiste en simular el juego de la ruleta mediante sockets utilizando C como lenguaje.

## Descripción del proyecto

El proyecto consiste en el diseño e implementación del juego de la ruleta de la suerte permitiendo jugar de manera individual o bien en grupos. La finalidad del juego es averiguar la frase o palabra secreta que se muestra en cada partida.

#### Objetivos del juego de la ruleta

Para jugar a la ruleta de la suerte es necesario un tablón donde aparecen marcadas las letras de las que consta la frase a adivinar. El objetivo del juego es adivinar la frase consiguiendo el mayor número de puntos. Se jugará compitiendo con otros usuarios. Gana el jugador que consigue resolver satisfactoriamente la frase.

Todas las partidas versarán sobre la categoría de refranes.

#### Especificación del juego implementado

La comunicación entre los clientes del juego de la ruleta de la suerte se realizará bajo el protocolo de transporte TCP. La práctica que se propone consiste en la realización de una aplicación cliente/servidor que implemente el juego de la ruleta de la suerte con algunas restricciones. En el juego considerado los jugadores (los clientes) se conectan al servicio (el
servidor). Solamente se admitirán partidas con dos jugadores. Se admiten hasta 10 partidas simultáneas, y hasta 30 jugadores conectados simultáneamente en el servidor.

### Funcionalidades del programa

Considerando la práctica completa, vamos a considerar los siguientes tipos de mensajes con el siguiente formato cada uno:

- USUARIO usuario: mensaje para introducir el usuario que desea.
- PASSWORD contraseña: mensaje para introducir la contraseña asociada al usuario.
- REGISTER –u usuario –p password: mensaje mediante el cual el usuario solicita registrarse para acceder al juego de la ruleta que escucha en el puerto TCP 2050.
- INICIAR_PARTIDA: mensaje para solicitar jugar una partida de la ruleta de la suerte en grupo.
- CONSONANTE letra, donde letra indica una consonante que se piensa que puede estar en la frase.
- VOCAL letra, para poder mandar este mensaje necesitas tener al menos 50 puntos, que se restarán por cada vocal que solicites, con independencia del número de veces que aparezca.
- RESOLVER frase, donde frase representará una cadena que contiene el refrán que queremos resolver.
- PUNTUACION, solicitar la puntuación en el juego.
- SALIR: mensaje para solicitar salir del juego.

Cualquier otra tipo de mensaje que se envíe al servidor, no será reconocida por el protocolo como un mensaje válido y generará su correspondiente“-Err.” por parte del servidor.

## Ejecución

Para compilar el servidor será necesario incluir el fichero "util.c":
gcc util.c ruletaServidor.c -o ruletaCliente.exe
Para compilar el cliente:
gcc ruletaCliente.c -o ruletaCliente.exe

"ruleta.txt" será la base de datos de nuestra aplicación y tendrá el siguiente formato:
usuario1-contraseña1-
usuario2-contraseña2-
usuario3-contraseña3-
...
usuarioX-contraseñaX-
