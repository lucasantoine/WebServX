#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "socket.h"


void initialiser_signaux(void){
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) { 
		perror("signal"); 
	}
}

int main (/*int argc , char ** argv*/){
   
	initialiser_signaux();

    int socket_serveur = creer_serveur(8080);
    int socket_client ;
	char client_message[80];
	int size = 0;
	while(1){ 
	    socket_client = accept(socket_serveur, NULL, NULL);
		if(socket_client == -1){
		    perror("accept");
		    return -1;
		    /* traitement d ’ erreur */
		}
		if(fork()){
			/* On peut maintenant dialoguer avec le client */
			const char *message_bienvenue[15] = {
				"#===========================================================================#\n", 
				"|                                                                           |\n",
				"|                                  WebServ'X                                |\n",
				"|                                                                           |\n",
				"|                     Bonjour et bienvenue sur WebServX !                   |\n", 
				"|        Il s'agit de la page par défaut de WebServX, si vous la voyez      |\n", 
				"|                     cela signifie que tout fonctionne.                    |\n", 
				"|       Pour la changer, nous vous invitons a regarder la documentation.    |\n", 
				"|          WebServX est un serveur web open source crée dans le cadre       |\n", 
				"|         du cours de Programmation Système Avancé à l'IUT A de Lille.      |\n", 
				"|          Ce projet a été réalisé par ANTOINE Lucas et POMIER Mathys       |\n", 
				"|      et supervisé par HAUSPIE Michael, PLACE Jean-Marie, RIQUET Damien    |\n", 
				"|                 et BEAUFILS Bruno. Projet lancé le 31/01/2020.            |\n", 
				"|                                                                           |\n",
				"#===========================================================================#\n"};
			sleep(1);
			for(int i = 0; i < 15; i++){
				write(socket_client , message_bienvenue[i] , strlen(message_bienvenue[i]));
			}
			while((size = read(socket_client, client_message, 80)) != -1){
				write(socket_client, client_message, size);
			}
		}
		close(socket_client);
	}
    return 0;
}