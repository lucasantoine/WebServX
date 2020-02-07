#include <stdio.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "socket.h"


int creer_serveur(int port){
    int socket_serveur;
    socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_serveur == -1){
        /* traitement de l ’ erreur */
        perror ("socket_serveur");
        return -1;
    }

    int optval = 1;
    if (setsockopt(socket_serveur, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
        perror("Can not set SO_REUSEADDR option"); 
    }



    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET; /* Socket ipv4 */
    saddr.sin_port = htons(port); /* Port d ’e coute */
    saddr.sin_addr.s_addr = INADDR_ANY; /* e coute sur toutes les interfaces */

    /* Utilisation de la socket serveur */
    if (bind(socket_serveur, (struct sockaddr *)& saddr, sizeof(saddr)) == -1){
        perror("bind socker_serveur");
        return -1;
        /* traitement de l ’ erreur */
    }
    if (listen(socket_serveur, 10) == -1){
        perror("listen socket_serveur");
        return -1;
        /* traitement d ’ erreur */
    }
	return socket_serveur;
}