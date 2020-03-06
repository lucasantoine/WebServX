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
#include "http_parse.h"


const int BUFFER_SIZE = 128;
const char *message_bienvenue =  "#===========================================================================#\n|                                                                           |\n|                                  WebServ'X                                |\n|                                                                           |\n|                     Bonjour et bienvenue sur WebServX !                   |\n|        Il s'agit de la page par défaut de WebServX, si vous la voyez      |\n|                     cela signifie que tout fonctionne.                    |\n|       Pour la changer, nous vous invitons a regarder la documentation.    |\n|          WebServX est un serveur web open source créé dans le cadre       |\n|         du cours de Programmation Système Avancé à l'IUT A de Lille.      |\n|          Ce projet a été réalisé par ANTOINE Lucas et POMIER Mathys       |\n|      et supervisé par HAUSPIE Michael, PLACE Jean-Marie, RIQUET Damien    |\n|                 et BEAUFILS Bruno. Projet lancé le 31/01/2020.            |\n|                                                                           |\n#===========================================================================#\n";

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

char * fgets_or_exit ( char * buffer , int size , FILE * stream ){
	if(fgets(buffer, size, stream) == NULL) {
		exit(0);
	}
	return buffer;
}

void skip_headers(FILE *client){
	char buffer[BUFFER_SIZE];
	while(strcmp(buffer, "\r\n") != 0 && strcmp(buffer, "\n") != 0) { 
		fgets_or_exit(buffer, BUFFER_SIZE, client);
	}
}

void send_status ( FILE * client , int code , const char * reason_phrase ){
	fprintf(client, "HTTP/1.1 %d %s\r\n", code, reason_phrase);
}

void send_response ( FILE * client , int code , const char * reason_phrase , const char * message_body ){
	send_status(client, code, reason_phrase);
	fprintf(client, "Content-Length: %d\r\n\r\n%s", (int) strlen(message_body), message_body);
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
			//int cpt = 1;
			FILE * client = fdopen(socket_client, "w+");
			http_request request;
			if(client == NULL){
		    	perror("fdopen");
		    	return -1;
		    	/* traitement d ’ erreur */
			}
			while(fgets_or_exit(client_message, BUFFER_SIZE, client)) {
				if (parse_http_request(client_message, &request) == 0){
					send_response(client, 400, "Bad Request", "Bad request\r\n");
				}else if (request.method == HTTP_UNSUPPORTED){
					send_response(client, 405 ,"Method Not Allowed", "Method Not Allowed\r\n");
				}else if (strcmp(request.target ,"/") == 0){
					send_response(client, 200, "OK", message_bienvenue);
				}else{
					send_response(client, 404, "Not Found", "Not Found\r\n");
				}
			}
			close(socket_client);
		}
	}
	return 0;
}

