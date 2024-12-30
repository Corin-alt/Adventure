/**
 * @file client.c
 * @brief Gestion du client du jeu
 * @author Dupont Corentin & Lacroix Owen
 * @date 2022-03-30
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <locale.h>
#include <pthread.h>
#include "../utils/ncurses_utils.h"
#include "game_int.h"
#include "../utils/threads_utils.h"
#include "../utils/sockets_utils.h"
#include "../utils/files_utils.h"
#include "../utils/other_utils.h"
#include "../utils/include.h"
#include "../elements/hero.h"

/**
 * @brief Routine de thread qui permet de rafraichier l'interface (map, hero etc...)
 * 
 * @param arg les arguments passés au thread
 * @return void* 
 */

void * interface_routine(void * arg) {

    pthread_testcancel();

    int socket_client_fd = * (int * ) arg;
    int flag;

    map_t * map = (map_t * ) malloc_check(sizeof(map_t));
    hero_t * hero = (hero_t * ) malloc_check(sizeof(hero_t));
    interface_t * inter = NULL;

    char * msg;

    while (1) {
        read_int(socket_client_fd, & flag);

        if (inter != NULL) {
            if (flag == FLAG_SUCCESS_MESSAGE || flag == FLAG_TAKE_ARTEFACT) {
                read_string(socket_client_fd, & msg);
                fenetre_printw_col(inter -> win_infos, GREEN, msg);
                fenetre_refresh(inter -> win_infos);
            } else if (flag == FLAG_WARNING_MESSAGE) {
                read_string(socket_client_fd, & msg);
                fenetre_printw_col(inter -> win_infos, YELLOW, msg);
                fenetre_refresh(inter -> win_infos);
            } else if (flag == FLAG_ERROR_MESSAGE) {
                read_string(socket_client_fd, & msg);
                fenetre_printw_col(inter -> win_infos, RED, msg);
                fenetre_refresh(inter -> win_infos);
            }
        }

        lock_map(map);
        read_map(socket_client_fd, map);
        unlock_map(map);

        read_hero(socket_client_fd, hero);

        if (inter == NULL) {
            inter = game_interface_creer(map, hero);
            print_hero_stat(hero, inter -> win_hero);
            fenetre_refresh(inter -> win_hero);
        } else {
            inter -> map = map;
            inter -> hero = hero;
            print_hero_stat(hero, inter -> win_hero);
            refresh_map_in_interface(inter, map);
            fenetre_refresh(inter -> win_hero);
            fenetre_refresh(inter -> win_infos);
        }

        usleep(convert_theoric_move_speed_for_thread(hero -> move_speed));
        //sleep(1);
    }

    free(arg);

    return NULL;
}

int main(int argc, char * argv[]) {
    struct sockaddr_in adresse;
    pthread_t * map_refresh_thread;
    sigset_t sigs_new, sigs_old;
    int sock_client_fd, ch, stop = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage : ./client <server_address> <server_port>\n");
        fprintf(stderr, "With :\n");
        fprintf(stderr, "  server_address : server IPv4 address\n");
        fprintf(stderr, "  server_port    : server port number\n");
        exit(EXIT_FAILURE);
    }

    // Initialisation de ncurses
    setlocale(LC_ALL, "en_US.UTF-8");
    ncurses_init();
    ncurses_initsouris();
    ncurses_couleurs();
    game_palette();
    clear();
    refresh();

    //désactivation du signale SIGINT
    sigemptyset( & sigs_new);
    sigaddset( & sigs_new, SIGINT);
    if (sigprocmask(SIG_BLOCK, & sigs_new, & sigs_old) == -1) {
        perror("Erreur lors du blocage des signaux ");
        exit(EXIT_FAILURE);
    }

    sock_client_fd = create_ipv4_tcp_socket(); //création de la socket
    init_address( & adresse, argv[1], atoi(argv[2])); //initialisation de l'adrresse (remplissage de la structure)
    connect_socket(sock_client_fd, & adresse); //connexion au serveur

    map_refresh_thread = (pthread_t * ) malloc_check(sizeof(pthread_t));
    create_thread_check(map_refresh_thread, interface_routine, & sock_client_fd);

    while (stop != 1) {
        ch = getch();

        if ((ch == 'Q') || (ch == 'q')) {
            write_int(sock_client_fd, KEY_q);
            stop = 1;
        } else if (ch == KEY_UP) {
            write_int(sock_client_fd, KEY_UP);
        } else if (ch == KEY_DOWN) {
            write_int(sock_client_fd, KEY_DOWN);
        } else if (ch == KEY_LEFT) {
            write_int(sock_client_fd, KEY_LEFT);
        } else if (ch == KEY_RIGHT) {
            write_int(sock_client_fd, KEY_RIGHT);
        } else if (ch == KEY_SPACE) {
            write_int(sock_client_fd, KEY_SPACE);
        }

    }

    // Arrêt de ncurses
    ncurses_stop();

    close_in_write(sock_client_fd); //fermeture de la socket
    pthread_cancel( * map_refresh_thread);
    if (pthread_join( * map_refresh_thread, NULL) == 0) {
        printf("[Log - Client] - Le client s'est correctement arrêté.\n");
    }

    //réactivation du signale SIGINT
    if (sigprocmask(SIG_SETMASK, & sigs_old, NULL) == -1) {
        perror("Erreur lors du repositionnement ");
        exit(EXIT_FAILURE);
    }

    free(map_refresh_thread);

    return EXIT_SUCCESS;
}