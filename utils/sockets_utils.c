/**
 * @file threads_utils.c
 * @brief Fonctions utiles aux sockets
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include "sockets_utils.h"
#include "ncurses_utils.h"
#include "ncurses.h"

#define MAX_RETRIES 3
#define RETRY_DELAY 100000 // 100ms

int create_ipv4_tcp_socket() {
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        ncurses_stop();
        perror("Création socket TCP IPv4");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int create_ipv4_udp_socket() {
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd == -1) {
        ncurses_stop();
        perror("Création socket UDP IPv4");
        exit(EXIT_FAILURE);
    }
    return fd;
}

void close_socket(int sock_fd) {
    if (close(sock_fd) == -1) {
        ncurses_stop();
        perror("Fermeture socket");
        exit(EXIT_FAILURE);
    }
}

void listen_socket(int sock_fd, int size_queue) {
    if (listen(sock_fd, size_queue) == -1) {
        ncurses_stop();
        perror("Erreur listen");
        exit(EXIT_FAILURE);
    }
}

void close_in_write(int sock_fd) {
    if (shutdown(sock_fd, SHUT_WR) == -1) {
        ncurses_stop();
        perror("Erreur shutdown write");
        exit(EXIT_FAILURE);
    }
}

void close_in_read(int sock_fd) {
    if (shutdown(sock_fd, SHUT_RD) == -1) {
        ncurses_stop();
        perror("Erreur shutdown read");
        exit(EXIT_FAILURE);
    }
}

void close_in_read_write(int sock_fd) {
    if (shutdown(sock_fd, SHUT_RDWR) == -1) {
        ncurses_stop();
        perror("Erreur shutdown read/write");
        exit(EXIT_FAILURE);
    }
}

void connect_socket(int sock_fd, const void *addr) {
    int retries = 0;
    while (retries < MAX_RETRIES) {
        if (connect(sock_fd, (struct sockaddr*)addr, sizeof(struct sockaddr_in)) == 0) {
            return;
        }
        if (errno != ECONNREFUSED && errno != EINTR) {
            break;
        }
        usleep(RETRY_DELAY);
        retries++;
    }
    ncurses_stop();
    perror("Connexion échouée");
    exit(EXIT_FAILURE);
}

int accept_socket(int sock_fd) {
    int new_sock;
    do {
        new_sock = accept(sock_fd, NULL, NULL);
    } while (new_sock == -1 && errno == EINTR);
    return new_sock;
}

void init_address(struct sockaddr_in *addr, char* ip_addr, int port) {
    if (!addr || !ip_addr) return;

    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons((uint16_t)port);
    str_addr_to_network(AF_INET, ip_addr, &addr->sin_addr);
}

void init_any_ipv4_address(struct sockaddr_in *addr, int port) {
    if (!addr) return;

    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons((uint16_t)port);
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
}

void bind_address(int sock_fd, const void *addr) {
    if (bind(sock_fd, (struct sockaddr*)addr, sizeof(struct sockaddr_in)) == -1) {
        ncurses_stop();
        perror("Bind échoué");
        exit(EXIT_FAILURE);
    }
}

ssize_t send_udp(int sock_fd, const void *msg, size_t msg_length, const void *dest_addr, socklen_t addr_length) {
    ssize_t sent;
    int retries = 0;

    while (retries < MAX_RETRIES) {
        sent = sendto(sock_fd, msg, msg_length, 0, (struct sockaddr*)dest_addr, addr_length);
        if (sent != -1 || errno != EINTR) {
            break;
        }
        retries++;
    }

    if (sent == -1) {
        ncurses_stop();
        perror("Envoi UDP échoué");
        exit(EXIT_FAILURE);
    }
    return sent;
}

ssize_t receive_udp(int sock_fd, void *msg, size_t msg_length, int flags, void *src_addr, socklen_t *addr_length) {
    ssize_t received;
    do {
        received = recvfrom(sock_fd, msg, msg_length, flags, (struct sockaddr*)src_addr, addr_length);
    } while (received == -1 && errno == EINTR);

    if (received == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
        ncurses_stop();
        perror("Réception UDP échouée");
        exit(EXIT_FAILURE);
    }
    return received;
}

void str_addr_to_network(int family, const char *addr, void *addr_buffer) {
    if (!addr || !addr_buffer) return;

    if (inet_pton(family, addr, addr_buffer) != 1) {
        ncurses_stop();
        fprintf(stderr, "Conversion adresse échouée\n");
        exit(EXIT_FAILURE);
    }
}

void network_addr_to_str(int family, const void *addr, char *addr_buffer, socklen_t len) {
    if (!addr || !addr_buffer) return;

    if (inet_ntop(family, addr, addr_buffer, len) == NULL) {
        ncurses_stop();
        fprintf(stderr, "Conversion adresse réseau échouée\n");
        exit(EXIT_FAILURE);
    }
}