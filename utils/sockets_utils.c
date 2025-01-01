/**
 * @file threads_utils.c
 * @brief Implémentation des fonctions utiles aux sockets
 * @author Dupont Corentin & Lacroix Owen
 * @date 2022-03-14
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include "sockets_utils.h"
#include "ncurses_utils.h"
#include "ncurses.h"

/**
 * @brief Permet de créer une socket tcp IPV4
 * 
 * @return int le descripteur de fichier/communication
 */
int create_ipv4_tcp_socket(){
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        ncurses_stop();
        perror("An error occurred while creating the ipv4 tcp socket");
        exit(EXIT_FAILURE);
    }
    return fd;
}

/**
 * @brief Permet de créer une socket udp IPV4
 * 
 * @return int le descripteur de fichier/communication
 */
int create_ipv4_udp_socket(){
    int fd;
    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        ncurses_stop();
        perror("An error occurred while creating the ipv4 udp socket ");
        exit(EXIT_FAILURE);
    }
    return fd;
}


/**
 * @brief Permet de fermer une socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 */
void close_socket(int sock_fd){
    if(close(sock_fd) == -1) {
        ncurses_stop();
        perror("An error occurred while closing teh socket ");
        exit(EXIT_FAILURE);
    }
}


/**
 * @brief Permet d'écouter une socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 * @param size_queue la taille de la file d'attente
 */
void listen_socket(int sock_fd, int size_queue){
     if (listen(sock_fd, size_queue) == -1) {
        ncurses_stop(); 
        perror("An error occurred while marking the socket as a passive socket");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet de fermer l'ecriture de la socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 */
void close_in_write(int sock_fd){
     if (shutdown(sock_fd, SHUT_WR) == -1) {
        ncurses_stop();
        perror("Error during socket write closure ");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet de fermer la lecture de la socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 */
void close_in_read(int sock_fd){
     if (shutdown(sock_fd, SHUT_RD) == -1) {
        ncurses_stop(); 
        perror("Error during socket read closure ");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet de fermer la lecture et l'ecriture de la socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 */
void close_in_read_write(int sock_fd){
     if (shutdown(sock_fd, SHUT_RDWR) == -1) {
        ncurses_stop(); 
        perror("Error during socket read/write closure ");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet de se connecter à une socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 * @param addr l'adresse
 */
void connect_socket(int sock_fd, const void *addr){
    if (connect(sock_fd, (struct sockaddr*) addr, sizeof(struct sockaddr_in)) == -1) {
        ncurses_stop();
        perror("An error occurred while connecting to an established socket");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet d'accpeter une socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 * @return int 
 */
int accept_socket(int sock_fd){
    /**
    int new_sock;
    if ((new_sock = accept(sock_fd, NULL, NULL)) == -1) {
        ncurses_stop();
        perror("An error occurred while creating a client socket");
        exit(EXIT_FAILURE);
    }
    */
    return accept(sock_fd, NULL, NULL);
}

/**
 * @brief Permet d'initialiser une adresse avec une ip
 * 
 * @param address l'adresse
 * @param port le port
 * @param ip_addr l'adresse ip
 */
void init_address(struct sockaddr_in *addr, char* ip_addr, int port){
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons((uint16_t) port);

    str_addr_to_network(AF_INET, ip_addr, &addr->sin_addr);
}

/**
 * @brief  Permet d'initialiser une adresse avec une ip aléatoire
 * 
 * @param address l'adresse
 * @param port le port
 */
void init_any_ipv4_address(struct sockaddr_in *addr, int port){
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons((uint16_t) port);
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
}

/**
 * @brief Permet d'attcher une socket à une adresse
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 * @param addr l'adresse à attribuer à la socket
 */
void bind_address(int sock_fd, const void *addr){
    if (bind(sock_fd, (struct sockaddr*) addr, sizeof(struct sockaddr_in)) == -1) {
        ncurses_stop();
        perror("An error occurred while binding the address to the socket");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet d'envoyer un message en UDP
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 * @param msg le message
 * @param msg_lenght la taille du message
 * @param dest_addr l'adresse de destination
 * @param addr_length la taille de l'adresse
 * @return ssize_t la taille de ce qui a été recu
 */
ssize_t send_udp(int sock_fd, const void *msg, size_t msg_lenght, const void *dest_addr, socklen_t addr_length){
    ssize_t sent;
    if ((sent = sendto(sock_fd, msg, msg_lenght, 0, (struct sockaddr*) dest_addr, addr_length)) == -1) {
        ncurses_stop();
        perror("An error occurred while sending the message through the socket");
        exit(EXIT_FAILURE);
    }
    return sent;
}

/**
 * @brief Permet de recevoir d'un message en UDP
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 * @param msg le message
 * @param msg_length la taille du message
 * @param flags 
 * @param src_addr l'adresse de départ
 * @param addr_length la taille de l'adresse
 * @return ssize_t la taille de ce qui a été envoyé
 */
ssize_t receive_udp(int sock_fd, void *msg, size_t msg_length, int flags, void *src_addr, socklen_t *addr_length){
    ssize_t receveid;
    if ((receveid = recvfrom(sock_fd, msg, msg_length, flags, (struct sockaddr*) src_addr, addr_length)) == -1) {
        if (errno != EINTR) {
            ncurses_stop();
            perror("An error occurred while receiving the message via the socket");
            exit(EXIT_FAILURE);
        }
    }
    return receveid;
}

/**
 * @brief Convertit une chaine de caractère en adreese réseau
 * 
 * @param family IPV4 ou IPV6
 * @param addr l'adresse au format str
 * @param addrBuffer la structure de l'adresse in_addr pour IPV4 ou in6_addr pour IPV6
 */
void str_addr_to_network(int family, const char *addr, void *addr_buffer){
    if (inet_pton(family, addr, addr_buffer) != 1) {
        ncurses_stop();
        fprintf(stderr, "An error occurred while converting the string address to a network address\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Convertit une adresse en chaine de caractère
 * 
 * @param family IPV4 ou IPV6
 * @param addr l'adresse au format réseau
 * @param addrBuffer l'adresse au format str
 * @param len la taille de l'adresse au format str
 */
void network_addr_to_str(int family, const void *addr, char *addr_buffer, socklen_t len){
    if (inet_ntop(family, addr, addr_buffer, len) == NULL) {
        ncurses_stop();
        fprintf(stderr, "An error occurred while converting the network address to a string address\n");
        exit(EXIT_FAILURE);
    }
}