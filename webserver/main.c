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

const int BUFFER_SIZE = 128;

void traitement_signal(int sig) { 
	waitpid(sig, NULL, WNOHANG);
}


void initialiser_signaux(void){
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) { 
		perror("signal"); 
	}
	struct sigaction sa;
	//sa.sa_handler = traitement_signal; 
	sigemptyset(&sa.sa_mask); 
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = traitement_signal;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) { 
		perror("sigaction(SIGCHLD)"); 
	}
}

int main (/*int argc , char ** argv*/){
   
	initialiser_signaux();

    int socket_serveur = creer_serveur(8080);
    int socket_client;
	char client_message[BUFFER_SIZE];
	while(1){ 
	    socket_client = accept(socket_serveur, NULL, NULL);
		if(socket_client == -1){
		    perror("accept");
		    return -1;
		    /* traitement d ’ erreur */
		}
		int pidFork = fork();
		if(pidFork == 0){
			int cpt = 1;
			FILE * file = fdopen(socket_client, "w+");
			if(file == NULL){
		    	perror("fdopen");
		    	return -1;
		    	/* traitement d ’ erreur */
			}
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
				"|          WebServX est un serveur web open source créé dans le cadre       |\n", 
				"|         du cours de Programmation Système Avancé à l'IUT A de Lille.      |\n", 
				"|          Ce projet a été réalisé par ANTOINE Lucas et POMIER Mathys       |\n", 
				"|      et supervisé par HAUSPIE Michael, PLACE Jean-Marie, RIQUET Damien    |\n", 
				"|                 et BEAUFILS Bruno. Projet lancé le 31/01/2020.            |\n", 
				"|                                                                           |\n",
				"#===========================================================================#\n"};

			while(fgets(client_message, BUFFER_SIZE, file) != NULL) {
				
				printf(client_message);
				if(cpt == 1) {
					int res = 0;
					/*if(strcmp(client_message, "GET / HTTP/1.1\r\n") != 0){
						fprintf(file, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 17\r\n\r\n400 Bad Request\r\n");
					}else */if((res = strcmp(client_message, "GET /inexistant HTTP/1.1\r\n")) == 0){
						//fprintf(file, "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Length: 15\r\n\r\n404 Not Found\r\n");
						fprintf(file, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", 1182);
						for(int i = 0; i < 15; i++){
							fprintf(file, "%s", message_bienvenue[i]);
						}
						printf("HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Length: 100\r\n\r\n404 Not Found\r\n");
					}
				} else if(cpt > 1 && strcmp(client_message, "\r\n") == 0) {
					fprintf(file, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", 1182);
					for(int i = 0; i < 15; i++){
						fprintf(file, "%s", message_bienvenue[i]);
					}
				}
				cpt++;
			}
			return 0;
		}
		close(socket_client);
	}
    return 0;
}

