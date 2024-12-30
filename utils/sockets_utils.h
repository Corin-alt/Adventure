/**
 * @file sockets_utils.h
 * @brief Protoypes des fonctions utiles aux sockets
 * @author Dupont Corentin & Lacroix Owen
 * @date 2022-03-14
 */

#ifndef H_SOCKETS_UTILS
#define H_SOCKETS_UTILS

#include <arpa/inet.h>
#include <unistd.h>

/**
 * @brief Permet de créer une socket tcp IPV4
 * 
 * @return int le descripteur de fichier/communication
 */
int create_ipv4_tcp_socket();

/**
 * @brief Permet de créer une socket udp IPV4
 * 
 * @return int le descripteur de fichier/communication
 */
int create_ipv4_udp_socket();

/**
 * @brief Permet de fermer une socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 */
void close_socket(int sock_fd);

/**
 * @brief Permet d'écouter une socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 * @param size_queue la taille de la file d'attente
 */
void listen_socket(int sock_fd, int size_queue);

/**
 * @brief Permet de fermer l'ecriture de la socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 */
void close_in_write(int sock_fd);

/**
 * @brief Permet de fermer la lecture de la socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 */
void close_in_read(int sock_fd);

/**
 * @brief Permet de fermer la lecture et l'ecriture de la socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 */
void close_in_read_write(int sock_fd);

/**
 * @brief Permet de se connecter à une socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 * @param addr l'adresse
 */
void connect_socket(int sock_fd, const void *addr);

/**
 * @brief Permet d'accpeter une socket
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 * @return int 
 */
int accept_socket(int sock_fd);
/**
 * @brief Permet d'initialiser une adresse avec une ip
 * 
 * @param address l'adresse
 * @param port le port
 * @param ip_addr l'adresse ip
 */
void init_address(struct sockaddr_in *addr, char* ip_addr, int port);

/**
 * @brief  Permet d'initialiser une adresse avec une ip aléatoire
 * 
 * @param address l'adresse
 * @param port le port
 */
void init_any_ipv4_address(struct sockaddr_in *addr, int port);

/**
 * @brief Permet d'attcher une socket à une adresse
 * 
 * @param sock_fd le descripteur de fichier correspondant à la socket
 * @param addr l'adresse à attribuer à la socket
 */
void bind_address(int sock_fd, const void *addr);

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
ssize_t send_udp(int sock_fd, const void *msg, size_t msg_lenght, const void *dest_addr, socklen_t addr_length);

/**
 * 
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
ssize_t receive_udp(int sock_fd, void *msg, size_t msg_length, int flags, void *src_addr, socklen_t *addr_length);

/**
 * @brief Convertit une chaine de caractère en adreese réseau
 * 
 * @param family IPV4 ou IPV6
 * @param addr l'adresse au format str
 * @param addrBuffer la structure de l'adresse in_addr pour IPV4 ou in6_addr pour IPV6
 */
void str_addr_to_network(int family, const char *addr, void *addr_buffer);

/**
 * @brief Convertit une adresse en chaine de caractère
 * 
 * @param family IPV4 ou IPV6
 * @param addr l'adresse au format réseau
 * @param addrBuffer l'adresse au format str
 * @param len la taille de l'adresse au format str
 */
void network_addr_to_str(int family, const void *addr, char *addr_buffer, socklen_t len);

#endif