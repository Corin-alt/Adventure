#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <ncurses.h>
#include <errno.h>
#include <sys/wait.h>
#include "../data_structure/map_path.h"
#include "../utils/sockets_utils.h"
#include "../utils/files_utils.h"
#include "../utils/other_utils.h"
#include "../utils/threads_utils.h"
#include "../utils/ncurses_utils.h"
#include "../utils/include.h"

// Global variables with mutexes for thread safety
static volatile int stop = 0;
static volatile int count_client = 0;
static pthread_mutex_t MUTEX_NB_CLIENT = PTHREAD_MUTEX_INITIALIZER;

static volatile int GRAND_TOUT_LVL_1 = FALSE;
static volatile int GRAND_TOUT_LVL_2 = FALSE;
static volatile int GRAND_TOUT_LVL_3 = FALSE;
static pthread_mutex_t MUTEX_GRAND_TOUT = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t COND_GRAND_TOUT_LVL2 = PTHREAD_COND_INITIALIZER;
static pthread_cond_t COND_GRAND_TOUT_LVL3 = PTHREAD_COND_INITIALIZER;

static int nb_monster_thread = 0;
static pthread_t *monster_threads = NULL;
static pthread_mutex_t MUTEX_MONSTER_THREAD = PTHREAD_MUTEX_INITIALIZER;

static int nb_treasure_thread = 0;
static pthread_t *treasure_threads = NULL;
static pthread_mutex_t MUTEX_TREASURE_THREAD = PTHREAD_MUTEX_INITIALIZER;

static int nb_maps = 0;
static map_t *all_loaded_maps = NULL;
static map_path_t *map_path = NULL;
static pthread_mutex_t MUTEX_MAP_PATH = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int id_thread;
    int socket_fd_client;
} data_client_routine_t;

typedef struct {
    int id_thread;
    entity_t *monster;
    map_t *map;
} data_monster_routine_t;

typedef struct {
    int id_thread;
    map_t *map;
} data_treasure_routine_t;

static void handler(int signum) {
    if (signum == SIGINT) {
        printf("\n[Log - Serveur] - Signal d'arrêt reçu. Arrêt en attente des clients...\n");
        stop = 1;
    }
    else if (signum == SIGALRM) {
        pthread_mutex_lock(&MUTEX_GRAND_TOUT);
        if (GRAND_TOUT_LVL_1) {
            GRAND_TOUT_LVL_1 = FALSE;
        }
        else if (GRAND_TOUT_LVL_2) {
            pthread_cond_broadcast(&COND_GRAND_TOUT_LVL2);
            GRAND_TOUT_LVL_2 = FALSE;
        }
        else if (GRAND_TOUT_LVL_3) {
            pthread_cond_broadcast(&COND_GRAND_TOUT_LVL3);
            GRAND_TOUT_LVL_3 = FALSE;
        }
        pthread_mutex_unlock(&MUTEX_GRAND_TOUT);
    }
}

static void cleanup_server(void) {
    // Free monster threads
    pthread_mutex_lock(&MUTEX_MONSTER_THREAD);
    free(monster_threads);
    monster_threads = NULL;
    pthread_mutex_unlock(&MUTEX_MONSTER_THREAD);

    // Free treasure threads
    pthread_mutex_lock(&MUTEX_TREASURE_THREAD);
    free(treasure_threads);
    treasure_threads = NULL;
    pthread_mutex_unlock(&MUTEX_TREASURE_THREAD);

    // Free maps
    if (all_loaded_maps) {
        for (int i = 0; i < nb_maps; i++) {
            if (all_loaded_maps[i].entities) {
                free(all_loaded_maps[i].entities);
            }
        }
        free(all_loaded_maps);
        all_loaded_maps = NULL;
    }

    // Free map path
    if (map_path) {
        free_map_path(&map_path);
    }
}

static void *monster_routine(void *arg) {
    if (!arg) return NULL;

    data_monster_routine_t *data = (data_monster_routine_t *)arg;
    entity_t *monster = data->monster;
    map_t *map = data->map;
    free(arg);  // Free the data structure

    if (!monster || !map) return NULL;

    // Set cancelation type
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    unsigned int sleep_val = convert_theoric_move_speed_for_thread(monster->move_speed);
    int copy_x, copy_y, direction;

    while (1) {
        pthread_testcancel();

        // Handle GRAND_TOUT effects
        pthread_mutex_lock(&MUTEX_GRAND_TOUT);
        if (GRAND_TOUT_LVL_1) {
            sleep_val = 3;
        }
        else if (GRAND_TOUT_LVL_2) {
            monster->can_attack = FALSE;
            while (GRAND_TOUT_LVL_2) {
                pthread_cond_wait(&COND_GRAND_TOUT_LVL2, &MUTEX_GRAND_TOUT);
            }
            monster->can_attack = TRUE;
        }
        else if (GRAND_TOUT_LVL_3) {
            while (GRAND_TOUT_LVL_3) {
                pthread_cond_wait(&COND_GRAND_TOUT_LVL3, &MUTEX_GRAND_TOUT);
            }
            monster->health /= 2;
        }
        pthread_mutex_unlock(&MUTEX_GRAND_TOUT);

        // Monster movement logic
        copy_x = monster->coord_x;
        copy_y = monster->coord_y;
        direction = rand() % 4;

        switch (direction) {
            case 0: monster->coord_x--; break;
            case 1: monster->coord_x++; break;
            case 2: monster->coord_y--; break;
            case 3: monster->coord_y++; break;
        }

        if (!is_out_of_map(monster->coord_x, monster->coord_y)) {
            pthread_mutex_t *mutex_current = get_mutex_case(map, copy_x, copy_y);
            pthread_mutex_t *mutex_dest = get_mutex_case(map, monster->coord_x, monster->coord_y);

            if (!mutex_current || !mutex_dest) {
                monster->coord_x = copy_x;
                monster->coord_y = copy_y;
                continue;
            }

            if (pthread_mutex_trylock(mutex_dest) == 0) {
                pthread_mutex_lock(mutex_current);

                if (can_move(map, monster->coord_x, monster->coord_y)) {
                    case_t *cur = get_case(map, copy_x, copy_y);
                    case_t *dest = get_case(map, monster->coord_x, monster->coord_y);

                    if (!cur || !dest ||
                        is_hero(map, dest->coord_x, dest->coord_y) ||
                        is_tresor(map, dest->coord_x, dest->coord_y) ||
                        is_artefact(map, dest->coord_x, dest->coord_y)) {
                        monster->coord_x = copy_x;
                        monster->coord_y = copy_y;
                    }
                    else {
                        cur->symbol = SYMB_EMPTY_CASE;
                        cur->id_entity = -1;
                        dest->symbol = SYMB_MONSTRE;
                        dest->id_entity = monster->index;
                    }
                }
                else {
                    monster->coord_x = copy_x;
                    monster->coord_y = copy_y;
                }

                pthread_mutex_unlock(mutex_current);
                pthread_mutex_unlock(mutex_dest);
            }
        }
        else {
            monster->coord_x = copy_x;
            monster->coord_y = copy_y;
        }

        usleep(sleep_val);
    }

    return NULL;
}

static void *treasure_routine(void *arg) {
    if (!arg) return NULL;

    data_treasure_routine_t *data = (data_treasure_routine_t *)arg;
    map_t *map = data->map;
    free(arg);

    if (!map) return NULL;

    srand((unsigned int)time(NULL));

    while (1) {
        pthread_testcancel();

        case_t *ca = get_random_case_to_spawn(map);
        if (!ca) continue;

        pthread_mutex_t *mutex = get_mutex_case(map, ca->coord_x, ca->coord_y);
        if (!mutex) continue;

        if (pthread_mutex_trylock(mutex) == 0) {
            ca->symbol = SYMB_TRESOR;
            pthread_mutex_unlock(mutex);

            // Random spawn delay between 60-120 seconds
            sleep(60 + (rand() % 61));
        }
    }

    return NULL;
}

static void run_thread_map(map_t *map) {
    if (!map) return;

    // Handle monster threads
    int nb_monster = get_nb_monster_on_map(map);
    if (nb_monster > 0) {
        pthread_mutex_lock(&MUTEX_MONSTER_THREAD);

        pthread_t *new_threads = realloc(monster_threads,
                                       sizeof(pthread_t) * (nb_monster_thread + nb_monster));
        if (!new_threads) {
            pthread_mutex_unlock(&MUTEX_MONSTER_THREAD);
            return;
        }
        monster_threads = new_threads;

        entity_t *all_monster = get_all_monster_on_map(map);
        if (all_monster) {
            for (int i = 0; i < nb_monster; i++) {
                data_monster_routine_t *data = malloc(sizeof(data_monster_routine_t));
                if (!data) continue;

                data->map = map;
                data->monster = &all_monster[i];
                data->id_thread = nb_monster_thread + i + 1;

                create_thread_check(&monster_threads[nb_monster_thread + i],
                                  monster_routine, data);
            }
            free(all_monster);
        }

        nb_monster_thread += nb_monster;
        pthread_mutex_unlock(&MUTEX_MONSTER_THREAD);
    }

    // Handle treasure thread
    pthread_mutex_lock(&MUTEX_TREASURE_THREAD);

    pthread_t *new_treasure_threads = realloc(treasure_threads,
                                            sizeof(pthread_t) * (nb_treasure_thread + 1));
    if (!new_treasure_threads) {
        pthread_mutex_unlock(&MUTEX_TREASURE_THREAD);
        return;
    }
    treasure_threads = new_treasure_threads;

    data_treasure_routine_t *data = malloc(sizeof(data_treasure_routine_t));
    if (data) {
        data->map = map;
        data->id_thread = nb_treasure_thread + 1;

        create_thread_check(&treasure_threads[nb_treasure_thread],
                          treasure_routine, data);
        nb_treasure_thread++;
    }

    pthread_mutex_unlock(&MUTEX_TREASURE_THREAD);
}

static void *client_routine(void *arg) {
    if (!arg) return NULL;

    data_client_routine_t *data = (data_client_routine_t *)arg;
    int id = data->id_thread;
    int socket_client_fd = data->socket_fd_client;
    free(arg);

    srand((unsigned int)time(NULL) + id);

    map_t *map = NULL;
    hero_t *hero = NULL;
    char *msg = NULL;
    int input = -1;
    int flag = -1;
    int notify_grand_tout = TRUE;

    pthread_mutex_lock(&MUTEX_MAP_PATH);
    map = get_map_by_position_in_map_path(map_path, 0, 0);
    pthread_mutex_unlock(&MUTEX_MAP_PATH);

    if (!map) {
        printf("[Log - ERROR] Map initiale non trouvée pour client %d\n", id);
        return NULL;
    }

    hero = create_default_hero();
    if (!hero) {
        printf("[Log - ERROR] Création du héro impossible pour client %d\n", id);
        return NULL;
    }

    msg = malloc(256);
    if (!msg) {
        free_hero(&hero);
        return NULL;
    }

    lock_map(map);
    spawn_hero(map, hero);
    unlock_map(map);

    printf("[Log] - Connexion du client %d\n", id);
    printf("[Log] - Le hero du client %d est sur la carte \"%s\"\n",
           id, map->name ? map->name : "unknown");

    while (!stop) {
        // Check hero death
        if (hero->health <= 0) {
            pthread_mutex_lock(&MUTEX_MAP_PATH);
            map_t *spawn = get_map_by_position_in_map_path(map_path, 0, 0);
            if (spawn) {
                pthread_mutex_t *mutex_current = get_mutex_case(map, hero->coord_x, hero->coord_y);
                pthread_mutex_t *mutex_spawn = get_mutex_case(spawn, hero->x_spawn, hero->y_spawn);

                if (mutex_current && mutex_spawn) {
                    pthread_mutex_lock(mutex_current);
                    pthread_mutex_lock(mutex_spawn);

                    case_t *current = get_case(map, hero->coord_x, hero->coord_y);
                    case_t *spawn_case = get_case(spawn, hero->x_spawn, hero->y_spawn);

                    if (current && spawn_case) {
                        current->symbol = SYMB_EMPTY_CASE;
                        spawn_case->symbol = SYMB_HERO;
                        map = spawn;
                    }

                    pthread_mutex_unlock(mutex_current);
                    pthread_mutex_unlock(mutex_spawn);
                }
            }
            pthread_mutex_unlock(&MUTEX_MAP_PATH);

            flag = FLAG_ERROR_MESSAGE;
            strcpy(msg, "[Info] - Vous êtes mort :(\n");
        }

        // Read client input
        read_int(socket_client_fd, &input);

        // Handle client input
        switch (input) {
            case KEY_q:
                printf("[Log] - Client %d : Quitter\n", id);
                pthread_mutex_lock(&MUTEX_NB_CLIENT);
                count_client--;
                pthread_mutex_unlock(&MUTEX_NB_CLIENT);
                goto cleanup;

            case KEY_SPACE:
                pthread_mutex_lock(&MUTEX_GRAND_TOUT);
                if (GRAND_TOUT_LVL_1 || GRAND_TOUT_LVL_2 || GRAND_TOUT_LVL_3) {
                    strcpy(msg, "[Info] - Un autre joueur a déjà activé le grand tout.\n");
                    printf("[Log] - Client %d : ECHEC ! Le grand tout est déjà activé.\n", id);
                }
                else if (hero->nb_piece == 0) {
                    flag = FLAG_ERROR_MESSAGE;
                    strcpy(msg, "[Info] - Pas assez de pièces grand tout !\n");
                }
                else {
                    switch (hero->nb_piece) {
                        case 1:
                            GRAND_TOUT_LVL_1 = TRUE;
                            alarm(ALARM_SECONDS_GRAND_TOUT);
                            printf("[Log] - Grand tout NIVEAU 1 activé (client %d)\n", id);
                            break;
                        case 2:
                            GRAND_TOUT_LVL_2 = TRUE;
                            alarm(ALARM_SECONDS_GRAND_TOUT);
                            printf("[Log] - Grand tout NIVEAU 2 activé (client %d)\n", id);
                            break;
                        case 3:
                            GRAND_TOUT_LVL_3 = TRUE;
                            alarm(ALARM_SECONDS_GRAND_TOUT);
                            printf("[Log] - Grand tout NIVEAU 3 activé (client %d)\n", id);
                            break;
                    }
                    hero->nb_piece = 0;
                }
                pthread_mutex_unlock(&MUTEX_GRAND_TOUT);
                break;

            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                handle_movement(hero, map, input, msg, &flag);
                break;
        }

        // Handle GRAND_TOUT notifications
        pthread_mutex_lock(&MUTEX_GRAND_TOUT);
        if (notify_grand_tout) {
            if (GRAND_TOUT_LVL_1 || GRAND_TOUT_LVL_2 || GRAND_TOUT_LVL_3) {
                flag = FLAG_WARNING_MESSAGE;
                notify_grand_tout = FALSE;
                snprintf(msg, 256, "[Info] - Grand tout NIVEAU %d activé\n",
                        GRAND_TOUT_LVL_1 ? 1 : (GRAND_TOUT_LVL_2 ? 2 : 3));
            }
        }
        else if (!GRAND_TOUT_LVL_1 && !GRAND_TOUT_LVL_2 && !GRAND_TOUT_LVL_3) {
            flag = FLAG_WARNING_MESSAGE;
            notify_grand_tout = TRUE;
            strcpy(msg, "[Info] - Effet du grand tout terminé\n");
        }
        pthread_mutex_unlock(&MUTEX_GRAND_TOUT);

        // Send data to client
        write_int(socket_client_fd, flag);
        if (flag >= FLAG_SUCCESS_MESSAGE && flag <= FLAG_TAKE_ARTEFACT) {
            write_string(socket_client_fd, msg);
            flag = -1;
        }

        trylock_map(map);
        write_map(socket_client_fd, map);
        unlock_map(map);

        write_hero(socket_client_fd, hero);
        usleep(convert_theoric_move_speed_for_thread(hero->move_speed));
    }

cleanup:
    free_hero(&hero);
    free(msg);
    return NULL;
}

static void init_spawn(void) {
    map_t *spawn = get_loaded_map_by_name(all_loaded_maps, nb_maps, SPAWN_NAME_MAP);
    if (!spawn) {
        fprintf(stderr, "[ERROR] Carte spawn non trouvée\n");
        exit(EXIT_FAILURE);
    }

    map_path = create_map_path(spawn);
    if (!map_path) {
        fprintf(stderr, "[ERROR] Création du chemin impossible\n");
        exit(EXIT_FAILURE);
    }

    run_thread_map(spawn);
    printf("[Log] - Initialisation spawn OK\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Setup signal handlers
    struct sigaction sa = {
        .sa_handler = handler,
        .sa_flags = 0
    };
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1 ||
        sigaction(SIGALRM, &sa, NULL) == -1 ||
        sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    // Load maps
    nb_maps = count_maps();
    printf("[Log] - Chargement des maps (%d)...\n", nb_maps);
    all_loaded_maps = load_all_maps();
    if (!all_loaded_maps) {
        fprintf(stderr, "[ERROR] Chargement des maps impossible\n");
        return EXIT_FAILURE;
    }

    // Initialize spawn
    init_spawn();

    // Setup server socket
    int server_fd = create_ipv4_tcp_socket();
    struct sockaddr_in server_addr = {0};
    init_any_ipv4_address(&server_addr, atoi(argv[1]));
    bind_address(server_fd, &server_addr);
    listen_socket(server_fd, 5);

    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl");
        close_socket(server_fd);
        return EXIT_FAILURE;
    }

    printf("\nServeur en attente (port %s)...\n", argv[1]);

    pthread_t *client_threads = NULL;
    int exit_server = FALSE;

    while (!stop) {
        pthread_mutex_lock(&MUTEX_NB_CLIENT);
        if (count_client == 0 && exit_server) {
            stop = 1;
            printf("[Log] - Arrêt du serveur (aucun client)\n");
        }
        pthread_mutex_unlock(&MUTEX_NB_CLIENT);

        int client_fd = accept_socket(server_fd);
        if (client_fd != -1) {
            pthread_mutex_lock(&MUTEX_NB_CLIENT);
            count_client++;

            // Reallocate client threads array
            pthread_t *new_threads = realloc(client_threads,
                                           sizeof(pthread_t) * count_client);
            if (!new_threads) {
                perror("realloc client_threads");
                close_socket(client_fd);
                count_client--;
                pthread_mutex_unlock(&MUTEX_NB_CLIENT);
                continue;
            }
            client_threads = new_threads;

            if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
                perror("fcntl client");
                close_socket(client_fd);
                count_client--;
                pthread_mutex_unlock(&MUTEX_NB_CLIENT);
                continue;
            }

            data_client_routine_t *data = malloc(sizeof(data_client_routine_t));
            if (!data) {
                perror("malloc client data");
                close_socket(client_fd);
                count_client--;
                pthread_mutex_unlock(&MUTEX_NB_CLIENT);
                continue;
            }

            data->socket_fd_client = client_fd;
            data->id_thread = count_client;

            create_thread_check(&client_threads[count_client - 1],
                              client_routine, data);

            if (count_client == 1) {
                exit_server = TRUE;
            }
            pthread_mutex_unlock(&MUTEX_NB_CLIENT);
        }
    }

    // Cleanup
    printf("[Log] - Nettoyage...\n");

    // Cancel and join monster threads
    printf("[Log] - Arrêt des threads monstres...\n");
    pthread_mutex_lock(&MUTEX_MONSTER_THREAD);
    for (int i = 0; i < nb_monster_thread; i++) {
        pthread_cancel(monster_threads[i]);
        pthread_join(monster_threads[i], NULL);
    }
    pthread_mutex_unlock(&MUTEX_MONSTER_THREAD);

    // Cancel and join treasure threads
    printf("[Log] - Arrêt des threads trésors...\n");
    pthread_mutex_lock(&MUTEX_TREASURE_THREAD);
    for (int i = 0; i < nb_treasure_thread; i++) {
        pthread_cancel(treasure_threads[i]);
        pthread_join(treasure_threads[i], NULL);
    }
    pthread_mutex_unlock(&MUTEX_TREASURE_THREAD);

    // Cleanup resources
    close_socket(server_fd);
    free(client_threads);
    cleanup_server();

    printf("Serveur terminé.\n");
    return EXIT_SUCCESS;
}