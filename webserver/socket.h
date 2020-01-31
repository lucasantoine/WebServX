#ifndef __SOCKET_H__
#define __SOCKET_H__
/* * Cr é e une socket serveur qui é coute sur toute les interfaces IPv4
de la machine sur le port pass é en param è tre . La socket retourn é e
doit pouvoir ê tre utilis é e directement par un appel à accept .
La fonction retourne -1 en cas d ’ erreur ou le descripteur de la
socket cr é é e . */
int creer_serveur(int port);
# endif