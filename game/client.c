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

typedef struct {
    int socket_fd;
    volatile int *stop;
} interface_data_t;

void *interface_routine(void *arg) {
    pthread_testcancel();

    interface_data_t *data = (interface_data_t *)arg;
    if (!data) return NULL;

    int socket_client_fd = data->socket_fd;
    int flag;
    char *msg = NULL;

    map_t *map = malloc_check(sizeof(map_t));
    hero_t *hero = malloc_check(sizeof(hero_t));
    interface_t *inter = NULL;

    if (!map || !hero) {
        free(map);
        free(hero);
        free(data);
        return NULL;
    }

    while (!*(data->stop)) {
        read_int(socket_client_fd, &flag);

        if (inter && flag >= FLAG_SUCCESS_MESSAGE && flag <= FLAG_TAKE_ARTEFACT) {
            read_string(socket_client_fd, &msg);

            int color = (flag == FLAG_SUCCESS_MESSAGE) ? GREEN :
                       (flag == FLAG_WARNING_MESSAGE) ? YELLOW : RED;

            fenetre_printw_col(inter->win_infos, color, msg);
            fenetre_refresh(inter->win_infos);
            free(msg);
        }

        lock_map(map);
        read_map(socket_client_fd, map);
        unlock_map(map);
        read_hero(socket_client_fd, hero);

        if (!inter) {
            inter = game_interface_creer(map, hero);
            if (!inter) break;

            print_hero_stat(hero, inter->win_hero);
            fenetre_refresh(inter->win_hero);
        } else {
            inter->map = map;
            inter->hero = hero;
            print_hero_stat(hero, inter->win_hero);
            refresh_map_in_interface(inter, map);
            fenetre_refresh(inter->win_hero);
            fenetre_refresh(inter->win_infos);
        }

        usleep(convert_theoric_move_speed_for_thread(hero->move_speed));
    }

    if (inter) game_interface_supprimer(&inter);
    free(map);
    free(hero);
    free(data);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_address> <server_port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Init ncurses
    setlocale(LC_ALL, "en_US.UTF-8");
    ncurses_init();
    ncurses_initsouris();
    ncurses_couleurs();
    game_palette();
    clear();
    refresh();

    // Block SIGINT
    sigset_t sigs_block, sigs_old;
    sigemptyset(&sigs_block);
    sigaddset(&sigs_block, SIGINT);
    if (sigprocmask(SIG_BLOCK, &sigs_block, &sigs_old) == -1) {
        perror("sigprocmask");
        ncurses_stop();
        return EXIT_FAILURE;
    }

    // Setup socket connection
    int sock_client_fd = create_ipv4_tcp_socket();
    struct sockaddr_in server_addr = {0};
    init_address(&server_addr, argv[1], atoi(argv[2]));
    connect_socket(sock_client_fd, &server_addr);

    // Setup interface thread
    pthread_t interface_thread;
    volatile int stop = 0;
    interface_data_t *data = malloc_check(sizeof(interface_data_t));
    if (!data) {
        ncurses_stop();
        close_socket(sock_client_fd);
        return EXIT_FAILURE;
    }

    data->socket_fd = sock_client_fd;
    data->stop = &stop;

    create_thread_check(&interface_thread, interface_routine, data);

    // Main input loop
    int ch;
    while (!stop) {
        ch = getch();

        if (ch == 'q' || ch == 'Q') {
            write_int(sock_client_fd, KEY_q);
            stop = 1;
        }
        else if (ch == KEY_UP || ch == KEY_DOWN ||
                 ch == KEY_LEFT || ch == KEY_RIGHT ||
                 ch == KEY_SPACE) {
            write_int(sock_client_fd, ch);
        }
    }

    // Cleanup
    ncurses_stop();
    close_in_write(sock_client_fd);
    pthread_cancel(interface_thread);
    pthread_join(interface_thread, NULL);

    // Restore signal mask
    if (sigprocmask(SIG_SETMASK, &sigs_old, NULL) == -1) {
        perror("sigprocmask restore");
        return EXIT_FAILURE;
    }

    printf("Client terminé.\n");
    return EXIT_SUCCESS;
}