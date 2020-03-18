#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include "socket.h"
#include "http_parse.h"
#include "stats.h"


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

void send_response ( FILE * client , int code , const char * reason_phrase , const char * content_type, const int length, const char * message_body ){
	send_status(client, code, reason_phrase);
	fprintf(client, "Content-Type: %s\r\nContent-Length: %d\r\n\r\n%s", content_type,length, message_body);
}

char * rewrite_target(char * target){
	if(strcmp(target, "/") == 0) target = "/index.html";
	int i = strchr(target, '?') - target;
	if(i < 0) return target;
	char * rewrite = malloc(sizeof(char)*i);
	strncpy(rewrite, target, i);
	return rewrite;
}

FILE * check_and_open(const char * target, const char * document_root){
	char * pathname = malloc(strlen(document_root)+strlen(target));
	strcat(pathname, document_root);
	strcat(pathname, target);
	struct stat * stats = malloc(sizeof(struct stat));
	if(stat(pathname, stats) == -1){
		perror("stat");
		return NULL;
	}
	if(S_ISREG(stats->st_mode)){
		FILE * file = fopen(pathname, "r");
		return file;
	}
	return NULL;
}

int get_file_size(int fd){
	struct stat * stats = malloc(sizeof(struct stat));
	if(fstat(fd, stats) == -1){
		perror("stat");
		return 0;
	}
	return stats->st_size;
}

int copy(FILE * in, FILE * out){
	char b;
	while(fread(&b, 1, 1, in) == 1){
		if(fwrite(&b, 1, 1, out) == 0){
			return -1;
		}
	}
	return 0;
}

void send_stats(FILE *client){
	skip_headers(client);
	send_status(client, 200, "OK");
	fprintf(client, "Content-Length: 500\r\nContent-Type: text/html\r\n\r\n");
	fprintf(client, "Stats : \n\tNombre de connexions : %d\n\tNombre de requêtes : %d\n\tNombre de code de retour 200 : %d\n\tNombre d'erreurs 400 : %d\n\tNombre d'erreurs 403 : %d\n\tNombre d'erreurs 404 : %d\n", get_stats()->served_connections, get_stats()->served_requests, get_stats()->ok_200, get_stats()->ko_400, get_stats()->ko_403, get_stats()->ko_404);
}

int main (int argc , char ** argv){

	if(argc <= 1 || argc > 2){
		perror("Invalid number of arguments");
		exit(1);
	}
	
	DIR * dirroot;
	dirroot = opendir(argv[1]);
	if(dirroot == NULL){
		perror("opendir");
		exit(1);
	}

	if(chdir(argv[1]) < 0){
		perror("chdir");
		exit(1);
	}

	char * document_root = argv[1];
	
	initialiser_signaux();
	init_stats();

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
			FILE * client = fdopen(socket_client, "w+");
			http_request request;
			if(client == NULL){
		    	perror("fdopen");
		    	return -1;
		    	/* traitement d ’ erreur */
			}

			get_stats()->served_connections++;

			char * response_message = "";
			
			while(fgets_or_exit(client_message, BUFFER_SIZE, client)) {
				get_stats()->served_requests++;
				if (parse_http_request(client_message, &request) == 0){
					response_message = "Bad Request\r\n";
					send_response(client, 400, "Bad Request", "text/html", strlen(response_message),response_message);
					get_stats()->ko_400++;
				}else if (request.method == HTTP_UNSUPPORTED){
					response_message = "Method Not Allowed\r\n";
					send_response(client, 405 ,"Method Not Allowed", "text/html", strlen(response_message),response_message);
				}else{
					char * rewritetarget = rewrite_target(request.target);
					if(strcmp(rewritetarget, "/stats") == 0){
						get_stats()->ok_200++;
						send_stats(client);
					}else{
						FILE * file = check_and_open(rewritetarget, document_root);
						if(file == NULL){
							response_message = "Not Found\r\n";
							send_response(client, 404, "Not Found", "text/html", strlen(response_message),response_message);
							get_stats()->ko_404++;
						}else if(strstr(rewritetarget, "..") != NULL){
							response_message = "Forbidden\r\n";
							send_response(client, 403, "Forbiden", "text/html", strlen(response_message),response_message);
							get_stats()->ko_403++;
						}else{
							get_stats()->ok_200++;
							skip_headers(client);
							int fd = fileno(file);
							char * contenttype = "text/html";
							if(strstr(rewritetarget, ".jpg")) contenttype = "image/jpeg";
							send_response(client, 200, "OK", contenttype, get_file_size(fd), "");
							copy(file, client);
						}
					}
				}

			}
			close(socket_client);
		}
	}
	return 0;
}

