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

int nb_maps;
int stop = 0;

int id_client_quit = -1;
int count_client = 0;
pthread_mutex_t MUTEX_NB_CLIENT = PTHREAD_MUTEX_INITIALIZER;

int GRAND_TOUT_LVL_1 = FALSE;
int GRAND_TOUT_LVL_2 = FALSE;
int GRAND_TOUT_LVL_3 = FALSE;

pthread_mutex_t MUTEX_GRAND_TOUT = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t COND_GRAND_TOUT_LVL2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t COND_GRAND_TOUT_LVL3 = PTHREAD_COND_INITIALIZER;

int nb_monster_thread = 0;
pthread_t * monster_threads;
pthread_mutex_t MUTEX_MONSTER_THREAD = PTHREAD_MUTEX_INITIALIZER;

int nb_treasure_thread = 0;
pthread_t * treasure_threads;
pthread_mutex_t MUTEX_TREASURE_THREAD = PTHREAD_MUTEX_INITIALIZER;

map_t * all_loaded_maps;

map_path_t * map_path;
pthread_mutex_t MUTEX_MAP_PATH = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int id_thread;
    int socket_fd_client;
}
data_client_routine_t;

typedef struct {
    int id_thread;
    entity_t * monster;
    map_t * map;
}
data_monster_routine_t;

typedef struct {
    int id_thread;
    map_t * map;
}
data_treasure_routine_t;

void * treasure_routine(void * arg);
void * monster_routine(void * arg);

void handler(int signum) {
    if (signum == SIGINT) {
        printf("[Log - Serveur] - Des clients sont encore connectés. Le serveur se stoppera quand il n'y en aura plus.\n");
        stop = 1;
    } else if (signum == SIGALRM) {
        mutex_lock_check( & MUTEX_GRAND_TOUT);
        if (GRAND_TOUT_LVL_1 == TRUE) {
            GRAND_TOUT_LVL_1 = FALSE;
        } else if (GRAND_TOUT_LVL_2 == TRUE) {
            mutex_cond_broadcast_check( & COND_GRAND_TOUT_LVL2);
            GRAND_TOUT_LVL_2 = FALSE;
        } else if (GRAND_TOUT_LVL_3 == TRUE) {
            mutex_cond_broadcast_check( & COND_GRAND_TOUT_LVL3);
            GRAND_TOUT_LVL_3 = FALSE;
        }
        mutex_unlock_check( & MUTEX_GRAND_TOUT);
    }
}

/**
 * @brief Lance tous les threads liès à une map (les mosntres et trésors)
 * 
 * @param map la map
 */
void run_thread_map(map_t * map) {
   int nb_monster = get_nb_monster_on_map(map);
   nb_monster_thread = nb_monster_thread + nb_monster;

   if (nb_monster != 0) {
       mutex_lock_check(&MUTEX_MONSTER_THREAD);
       if (monster_threads == NULL) {
           monster_threads = (pthread_t *)malloc_check(sizeof(pthread_t) * nb_monster);
       } else {
           monster_threads = (pthread_t *)realloc(monster_threads, sizeof(pthread_t) * nb_monster_thread);
       }

       if (monster_threads == NULL) {
           printf("[Log - ERROR] Échec d'allocation des threads monstres\n");
           exit(EXIT_FAILURE);
       }

       int tmp_index = 0;
       entity_t * all_monster = get_all_monster_on_map(map);

       for (int i = (nb_monster_thread - nb_monster); i < nb_monster_thread; i++) {
           data_monster_routine_t * data = (data_monster_routine_t *)malloc_check(sizeof(data_monster_routine_t));
           data->map = map;
           data->monster = &all_monster[tmp_index];
           data->id_thread = i + 1;
           tmp_index++;
           create_thread_check(&monster_threads[i], monster_routine, data);
       }
       mutex_unlock_check(&MUTEX_MONSTER_THREAD);
   }

   mutex_lock_check(&MUTEX_TREASURE_THREAD);
   if (treasure_threads == NULL) {
       treasure_threads = (pthread_t *)malloc_check(sizeof(pthread_t) * 1);
   } else {
       treasure_threads = (pthread_t *)realloc(treasure_threads, sizeof(pthread_t) * (nb_treasure_thread + 1));
   }

   if (treasure_threads == NULL) {
       printf("[Log - ERROR] Échec d'allocation des threads trésors\n");
       exit(EXIT_FAILURE);
   }

   nb_treasure_thread++;

   data_treasure_routine_t * data = (data_treasure_routine_t *)malloc_check(sizeof(data_treasure_routine_t));
   data->map = map;
   data->id_thread = nb_treasure_thread;

   create_thread_check(&treasure_threads[nb_treasure_thread - 1], treasure_routine, data);
   mutex_unlock_check(&MUTEX_TREASURE_THREAD);
}

/**
 * @brief Routine de thread pour les trésors
 * 
 * @param arg 
 * @return void* 
 */
void * treasure_routine(void * arg) {
    srand(time(NULL));

    data_treasure_routine_t * data = (data_treasure_routine_t * ) arg;
    map_t * map = data -> map;

    case_t * ca;

    while (1) {
        int next_spawn = 0;
        ca = get_random_case_to_spawn(map);
        pthread_mutex_t * mutex = get_mutex_case(map, ca -> coord_x, ca -> coord_y);
        if (pthread_mutex_trylock(mutex) == 0) {
            ca -> symbol = SYMB_TRESOR;
            next_spawn = rand() % 2;
            mutex_unlock_check(mutex);
        }

        if (next_spawn == 0) {
            sleep(60); //prochain trésor dans 1 minute
        } else {
            sleep(120); //prochain trésor dans 2 minute
        }

        pthread_testcancel();
    }

    return NULL;
}

/**
 * @brief Routine de thread pour les monstres
 * 
 * @param arg 
 * @return void* 
 */
void * monster_routine(void * arg) {

    //on applique le mode d'anulation du thread en mode différé
    if (pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL) != 0) {
        printf("[Log - ERROR] - Erreur lors du mode différé. Lorsque les monstres mourront leurs threads ne seront pas stoppés.");
    }

    //crréation point d'anulation pour la ddestrution du thread si on coupe le serveur
    pthread_testcancel();

    data_monster_routine_t * data = (data_monster_routine_t * ) arg;
    //int id_thread = data->id_thread;
    entity_t * monster = data -> monster;
    map_t * map = data -> map;

    int copy_x;
    int copy_y;
    int direction;

    mutex_lock_check(get_mutex_case(map, monster -> coord_x, monster -> coord_y));
    case_t * ca = get_case(map, monster -> coord_y, monster -> coord_x);
    ca -> id_entity = -1;
    ca -> symbol = SYMB_EMPTY_CASE;
    mutex_unlock_check(get_mutex_case(map, monster -> coord_x, monster -> coord_y));

    unsigned int sleep_val = convert_theoric_move_speed_for_thread(monster -> move_speed);

    while (1) {
        if (pthread_mutex_trylock( & MUTEX_GRAND_TOUT) != EBUSY) {
            if (GRAND_TOUT_LVL_1 == TRUE) {
                sleep_val = 3;
            } else if (GRAND_TOUT_LVL_2 == TRUE) {
                monster -> can_attack = FALSE;
                while (GRAND_TOUT_LVL_2 == TRUE) {
                    pthread_cond_wait( & COND_GRAND_TOUT_LVL2, & MUTEX_GRAND_TOUT);
                }
                monster -> can_attack = TRUE;;
            } else if (GRAND_TOUT_LVL_3 == TRUE) {
                while (GRAND_TOUT_LVL_3 == TRUE) {
                    pthread_cond_wait( & COND_GRAND_TOUT_LVL3, & MUTEX_GRAND_TOUT);
                }
                monster -> health = monster -> health / 2;
            }
            mutex_unlock_check( & MUTEX_GRAND_TOUT);
        }

        copy_x = monster -> coord_x;
        copy_y = monster -> coord_y;

        direction = rand() % 4;

        if (direction == 0) {
            monster -> coord_x -= 1;
        } else if (direction == 1) {
            monster -> coord_x += 1;
        } else if (direction == 2) {
            monster -> coord_y -= 1;
        } else if (direction == 3) {
            monster -> coord_y += 1;
        }

        if (!is_out_of_map(monster -> coord_x, monster -> coord_y)) {
            pthread_mutex_t * mutex_current_case = get_mutex_case(map, copy_x, copy_y);
            pthread_mutex_t * mutex_dest_case = get_mutex_case(map, monster -> coord_x, monster -> coord_y);
            if (pthread_mutex_trylock(mutex_dest_case) != EBUSY) {
                mutex_lock_check(mutex_current_case);
                if (can_move(map, monster -> coord_x, monster -> coord_y)) {

                    case_t * cur = get_case(map, copy_x, copy_y);
                    case_t * dest = get_case(map, monster -> coord_x, monster -> coord_y);

                    if (is_hero(map, dest -> coord_x, dest -> coord_y)) {
                        monster -> coord_x = copy_x;
                        monster -> coord_y = copy_y;
                    } else if (is_tresor(map, dest -> coord_x, dest -> coord_y) || is_artefact(map, dest -> coord_x, dest -> coord_y)) {
                        monster -> coord_x = copy_x;
                        monster -> coord_y = copy_y;
                    } else {
                        cur -> symbol = SYMB_EMPTY_CASE;
                        cur -> id_entity = -1;

                        dest -> symbol = SYMB_MONSTRE;
                        dest -> id_entity = monster -> index;
                    }
                } else {
                    monster -> coord_x = copy_x;
                    monster -> coord_y = copy_y;
                }
                mutex_unlock_check(mutex_current_case);
                mutex_unlock_check(mutex_dest_case);
            }

        } else {
            monster -> coord_x = copy_x;
            monster -> coord_y = copy_y;
        }

        //sleep(sleep_val);
        usleep(sleep_val);
    }

    return NULL;
}

void * client_routine(void * arg) {

    //création d'un point d'anulation si un client se déconnecte
    pthread_testcancel();

    srand(time(NULL));

    data_client_routine_t * data = (data_client_routine_t * ) arg;

    int id = data -> id_thread;
    int socket_client_fd = data -> socket_fd_client;

    int input;
    int flag = -1;
    char * msg = malloc_check(sizeof(char) * 256);
    int notify_grand_tout = TRUE;
    int copy_x;
    int copy_y;

    map_t * map;
    hero_t * hero;
    mutex_lock_check( & MUTEX_MAP_PATH);
    map = get_map_by_position_in_map_path(map_path, 0, 0); // récupére la map du spawn dans le chemin des cartes
    mutex_unlock_check( & MUTEX_MAP_PATH);

    hero = create_default_hero();

    lock_map(map);
    spawn_hero(map, hero);
    unlock_map(map);

    printf("[Log] - Connexion du client %d \n", id);
    printf("[Log] - Le hero du client %d est sur la carte \"%s\"\n", id, map -> name);

    while (1) {

        //le hero est-il mort ?        
        if (hero -> health <= 0) {
            mutex_lock_check( & MUTEX_MAP_PATH);

            map_t * spawn = get_map_by_position_in_map_path(map_path, 0, 0);

            pthread_mutex_t * mutex_current_case = get_mutex_case(map, hero -> coord_x, hero -> coord_y);
            pthread_mutex_t * mutex_spawn_case = get_mutex_case(spawn, hero -> x_spawn, hero -> y_spawn);

            mutex_lock_check(mutex_current_case);
            mutex_lock_check(mutex_spawn_case);

            case_t * current_ca = get_case(map, hero -> coord_x, hero -> coord_y);
            case_t * spawn_ca = get_case(spawn, hero -> x_spawn, hero -> y_spawn);

            current_ca -> symbol = SYMB_EMPTY_CASE;
            spawn_ca -> symbol = SYMB_HERO;

            map = spawn;

            mutex_unlock_check(mutex_current_case);
            mutex_unlock_check(mutex_spawn_case);
            mutex_unlock_check( & MUTEX_MAP_PATH);

            flag = FLAG_ERROR_MESSAGE;
            strcpy(msg, "[Info] - Vous êtes mort :( .\n");
        }

        read_int(socket_client_fd, & input);

        if (input == KEY_q) {
            printf("[Log] - Client %d : Quitter\n", id);

            mutex_lock_check( & MUTEX_NB_CLIENT);
            count_client--;
            mutex_unlock_check( & MUTEX_NB_CLIENT);

        } else if (input == KEY_SPACE) {
            mutex_lock_check( & MUTEX_GRAND_TOUT);
            if (GRAND_TOUT_LVL_1 || GRAND_TOUT_LVL_2 || GRAND_TOUT_LVL_3) {
                strcpy(msg, "[Info] - Un autre joueur à déjà activé le grand tout.\n");
                printf("[Log] - Client %d : ECHEC ! Le grand tout est déjà activé.\n", id);
            } else {
                if (hero -> nb_piece == 0) {
                    flag = FLAG_ERROR_MESSAGE;
                    strcpy(msg, "[Info] - Tu n'as pas assez de pièce grand tout pour l'activer !\n");
                    printf("[Log] - Client %d : ECHEC ! Pas assez de pièces pour activer le grand tout.\n", id);
                } else {
                    if (hero -> nb_piece == 1) {
                        GRAND_TOUT_LVL_1 = TRUE;
                        printf("[Log] - Application du grand tout NIVEAU 1 par le client %d.\n", id);
                        if (alarm(ALARM_SECONDS_GRAND_TOUT) == -1) {
                            perror("Probléme lors de l'application de l'alarme ");
                            exit(EXIT_FAILURE);
                        }
                    } else if (hero -> nb_piece == 2) {
                        GRAND_TOUT_LVL_2 = TRUE;
                        printf("[Log] - Application du grand tout NIVEAU 2 par le client %d.\n", id);
                        if (alarm(ALARM_SECONDS_GRAND_TOUT) == -1) {
                            perror("Probléme lors de l'application de l'alarme ");
                            exit(EXIT_FAILURE);
                        }
                    } else if (hero -> nb_piece == 3) {
                        GRAND_TOUT_LVL_3 = TRUE;
                        printf("[Log] - Application du grand tout NIVEAU 3 par le client %d.\n", id);
                        if (alarm(ALARM_SECONDS_GRAND_TOUT) == -1) {
                            perror("Probléme lors de l'application de l'alarme ");
                            exit(EXIT_FAILURE);
                        }
                    }
                    hero -> nb_piece = 0;
                }

            }
            mutex_unlock_check( & MUTEX_GRAND_TOUT);
        } else if (input == KEY_UP || input == KEY_DOWN || input == KEY_LEFT || input == KEY_RIGHT) {

            copy_x = hero -> coord_x;
            copy_y = hero -> coord_y;

            if (input == KEY_UP) {
                printf("[Log] - Client %d : Avancer\n", id);
                hero -> coord_x -= 1;
            } else if (input == KEY_DOWN) {
                printf("[Log] - Client %d : Reculer\n", id);
                hero -> coord_x += 1;
            } else if (input == KEY_LEFT) {
                printf("[Log] - Client %d : Aller à gauche\n", id);
                hero -> coord_y -= 1;
            } else if (input == KEY_RIGHT) {
                printf("[Log] - Client %d : Aller à droite\n", id);
                hero -> coord_y += 1;
            }

            if (!is_out_of_map(hero -> coord_x, hero -> coord_y)) {

                pthread_mutex_t * mutex_current_case = get_mutex_case(map, copy_x, copy_y);
                pthread_mutex_t * mutex_dest_case = get_mutex_case(map, hero -> coord_x, hero -> coord_y);

                if (pthread_mutex_trylock(mutex_dest_case) != EBUSY) {
                    mutex_lock_check(mutex_current_case);

                    if (can_move(map, hero -> coord_x, hero -> coord_y)) {

                        case_t * current = get_case(map, copy_x, copy_y);
                        current -> symbol = SYMB_EMPTY_CASE;

                        case_t * dest = get_case(map, hero -> coord_x, hero -> coord_y);

                        if (is_artefact(map, dest -> coord_x, dest -> coord_y)) {
                            if (hero -> nb_art_in_inv == 5) {
                                flag = FLAG_ERROR_MESSAGE;
                                strcpy(msg, "Ton inventaire est déjà pleins d'artefact.\n");
                                hero -> coord_x = copy_x;
                                hero -> coord_y = copy_y;
                            } else {
                                flag = FLAG_SUCCESS_MESSAGE;
                                entity_t artefact = get_entity(map, dest);
                                add_artefact_in_inventory(hero, artefact, hero -> nb_art_in_inv + 1);
                                hero -> nb_art_in_inv++;
                                strcpy(msg, "Tu as ramassé un artefact.\n");
                            }
                        } else if (is_monster(map, dest -> coord_x, dest -> coord_y)) {
                            hero -> coord_x = copy_x;
                            hero -> coord_y = copy_y;

                            char tmp[256];
                            flag = FLAG_WARNING_MESSAGE;

                            entity_t monster = get_entity(map, dest);

                            int who_attack = rand() % 2;

                            if (who_attack == 0) {

                                if (pthread_mutex_trylock( & MUTEX_GRAND_TOUT) != EBUSY) {
                                    if (GRAND_TOUT_LVL_1 == TRUE || GRAND_TOUT_LVL_1 == TRUE || GRAND_TOUT_LVL_1 == TRUE) {
                                        if (alarm(0) == -1) {
                                            perror("Probléme lors de l'application de l'alarme ");
                                            exit(EXIT_FAILURE);
                                        }
                                    }

                                    int degats = (int)(hero -> strength - (hero -> strength % monster.armor));
                                    monster.health = monster.health - degats;
                                    flag = FLAG_SUCCESS_MESSAGE;
                                    sprintf(tmp, "[Info] - Vous avez croisé %s et vous lui avez infligé %d PV au monstre\n", monster.name, degats);
                                    strcpy(msg, tmp);
                                    tmp[0] = '\0';

                                    if (monster.health <= 0) {
                                        flag = FLAG_SUCCESS_MESSAGE;
                                        pthread_mutex_t * mutex = get_mutex_case(map, monster.coord_x, monster.coord_y);
                                        if (pthread_mutex_trylock(mutex) != EBUSY) {
                                            case_t * ca = get_case(map, monster.coord_x, monster.coord_y);
                                            ca -> id_entity = -1;
                                            ca -> symbol = SYMB_EMPTY_CASE;
                                            mutex_unlock_check(mutex);
                                        }

                                        if (add_exp(hero, 25) == 1) {
                                            sprintf(tmp, "[Info] - Vous avez vaincu %s et vous avez passé 1 niveau. Vos attibuts on été augmentés !\n", monster.name);
                                        } else {
                                            sprintf(tmp, "[Info] - Vous avez vaincu %s et vous avez gagné 25 pts d'xp.\n", monster.name);
                                        }
                                        strcpy(msg, tmp);
                                        tmp[0] = '\0';
                                    }
                                }
                                mutex_unlock_check( & MUTEX_GRAND_TOUT);
                            } else {
                                int degats = (int)(monster.strength - (monster.strength % hero -> armor));
                                hero -> health = hero -> health - degats;
                                flag = FLAG_WARNING_MESSAGE;
                                sprintf(tmp, "[Info] - Vous avez croisé %s et vous avez perdu %d PV.\n", monster.name, degats);
                                strcpy(msg, tmp);
                                tmp[0] = '\0';
                            }

                        } else if (is_tresor(map, dest -> coord_x, dest -> coord_y)) {
                            int random = rand() % 101;
                            if (random < PERCENTAGE_LIFE) {
                                char tmp[64];
                                int r_health = rand() % ((int)(hero -> max_health / MAX_RECUPERATION_LIFE));
                                if (hero -> health != hero -> max_health) {
                                    if (hero -> health + r_health > hero -> max_health) {
                                        hero -> health = hero -> max_health;
                                    } else {
                                        hero -> health += r_health;
                                    }
                                }
                                flag = FLAG_SUCCESS_MESSAGE;
                                sprintf(tmp, "[Tresor] : +%d PV\n", r_health);
                                strcpy(msg, tmp);
                            } else {
                                if (hero -> nb_piece != MAX_NB_PIECE) {
                                    flag = FLAG_WARNING_MESSAGE;
                                    hero -> nb_piece++;
                                    strcpy(msg, "[Tresor] : (LEGENDAIRE) +1 pièce grand-tout\n");
                                }
                            }
                        }

                        dest -> symbol = SYMB_HERO;

                    } else {
                        hero -> coord_x = copy_x;
                        hero -> coord_y = copy_y;
                    }
                    mutex_unlock_check(mutex_current_case);
                    mutex_unlock_check(mutex_dest_case);
                }
            } else {

                trylock_map(map);

                map_t * tmp;
                int tmp_size_to_know_if_map_has_been_added;
                int direction = get_direction_out_of_map(hero -> coord_x, hero -> coord_y);

                if (direction == -1) {
                    printf("[Log] - ERREUR ! Direction inconnue.\n");
                    hero -> coord_x = copy_x;
                    hero -> coord_y = copy_y;
                }

                tmp_size_to_know_if_map_has_been_added = map_path -> size;

                printf("[Log] - Le client %d est sur la carte \"%s\"\n", id, map -> name);

                mutex_lock_check( & MUTEX_MAP_PATH);
                tmp = get_or_generate_next_map(map_path, map, all_loaded_maps, nb_maps, direction);

                if (tmp == NULL) {
                    printf("[Log] - Toutes les cartes existantes sont déjà utilisées dans le chemin des cartes.\n");
                    hero -> coord_x = copy_x;
                    hero -> coord_y = copy_y;
                } else {

                    trylock_map(tmp);

                    case_t * old = get_case(map, copy_x, copy_y);
                    old -> symbol = SYMB_EMPTY_CASE;

                    unlock_map(map);
                    map = tmp;

                    if (map_path -> size > tmp_size_to_know_if_map_has_been_added) {
                        //une nouvelle map a été ajouté faut lancer ses threads
                        run_thread_map(map);
                        printf("La carte \"%s\" a été ajoutée au chemin des cartes.\n", map -> name);
                    }

                    printf("[Log] - Le client %d est sur la carte \"%s\"\n", id, map -> name);

                    case_t * ca;
                    if (direction == TOP) {
                        ca = get_case(map, NB_LIGNE - 1, hero -> coord_y); // apparait en bas de la prochaine map
                        hero -> coord_x = NB_LIGNE - 1;
                    } else if (direction == BOTTOM) {
                        ca = get_case(map, 0, hero -> coord_y); // apparait en haut de la prochaine map
                        hero -> coord_x = 0;
                    } else if (direction == LEFT) {
                        ca = get_case(map, hero -> coord_x, NB_COL - 1); // apparait à droite de la prochaine map 
                        hero -> coord_y = NB_COL - 1;
                    } else {
                        ca = get_case(map, hero -> coord_x, 0); // apparait à gauche de la prochaine map	
                        hero -> coord_y = 0;
                    }
                    ca -> symbol = SYMB_HERO;

                    unlock_map(tmp);
                }

                mutex_unlock_check( & MUTEX_MAP_PATH);
            }
        }

        //pour eviter de jouer en boucle l'input du joueur
        input = -1;

        if (pthread_mutex_trylock( & MUTEX_GRAND_TOUT) != EBUSY) {
            if (notify_grand_tout == TRUE) {
                if (GRAND_TOUT_LVL_1 == TRUE) {
                    flag = FLAG_WARNING_MESSAGE;
                    notify_grand_tout = FALSE;
                    strcpy(msg, "[Info] - Un joueur a activé le NIVEAU 1 du grand tout.\n -> Monstres ralentit pendant 1 minute.\n");
                } else if (GRAND_TOUT_LVL_2 == TRUE) {
                    flag = FLAG_WARNING_MESSAGE;
                    notify_grand_tout = FALSE;
                    strcpy(msg, "[Info] - Un joueur a activé le NIVEAU 2 du grand tout.\n -> Monstres bloqués et ne peuvent plus attaquer pendant 1 minute.\n");
                } else if (GRAND_TOUT_LVL_3 == TRUE) {
                    flag = FLAG_WARNING_MESSAGE;
                    notify_grand_tout = FALSE;
                    strcpy(msg, "[Info] - Un joueur a activé le NIVEAU 3 du grand tout.\n -> Monstres bloqués pendant 1 minute et perdent la moitié de leur vie.\n");
                }
            } else if ((GRAND_TOUT_LVL_1 == FALSE && GRAND_TOUT_LVL_2 == FALSE && GRAND_TOUT_LVL_3 == FALSE) && notify_grand_tout == FALSE) {
                flag = FLAG_WARNING_MESSAGE;
                notify_grand_tout = TRUE;
                strcpy(msg, "[Info] - L'effet du grand tout a pris fin.\n");
            }
            mutex_unlock_check( & MUTEX_GRAND_TOUT);
        }

        //envoit des données au client
        write_int(socket_client_fd, flag);
        if (flag == FLAG_SUCCESS_MESSAGE || flag == FLAG_WARNING_MESSAGE || flag == FLAG_ERROR_MESSAGE || flag == FLAG_TAKE_ARTEFACT) {
            write_string(socket_client_fd, msg);
            flag = -1;
        }

        trylock_map(map);
        write_map(socket_client_fd, map);
        unlock_map(map);

        write_hero(socket_client_fd, hero);

        usleep(convert_theoric_move_speed_for_thread(hero -> move_speed));
        //sleep(1);
    }

    free_hero( & hero);
    free(arg);
    return NULL;

}

void init_spawn() {
    map_t * spawn = get_loaded_map_by_name(all_loaded_maps, nb_maps, SPAWN_NAME_MAP);

    if (spawn != NULL) {
        map_path = create_map_path(spawn);
        printf("\n[Log] - Initialisation du chemin de carte avec la carte spawn : Ok\n");
    } else {
        printf("\n[Log] - Echec de l'initialisation de la structure chemin de carte avec la carte spawn.\n");
        exit(EXIT_FAILURE);
    }

    run_thread_map(spawn);
}

int main(int argc, char * argv[]) {
    struct sockaddr_in adresse;
    struct sigaction action;

    int sock_server_fd, sock_client_fd;
    pthread_t * client_threads = NULL;

    int EXIT_SERVER = FALSE;

    sigemptyset( & action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = handler;
    if (sigaction(SIGCHLD, & action, NULL) == -1) {
        perror("Erreur lors du placement du gestionnaire ");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGINT, & action, NULL) == -1) {
        perror("Erreur lors du placement du gestionnaire ");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGALRM, & action, NULL) == -1) {
        perror("Erreur lors du placement du gestionnaire ");
        exit(EXIT_FAILURE);
    }

    // Vérification des arguments
    if (argc != 2) {
        fprintf(stderr, "Usage : ./server <server_port>\n");
        fprintf(stderr, "With :\n");
        fprintf(stderr, "  server_port : the listening port number of the server\n");
        exit(EXIT_FAILURE);
    }

    nb_maps = count_maps();
    printf("[Log] - Liste des maps (%d) :\n", nb_maps);
    all_loaded_maps = load_all_maps();

    init_spawn();

    printf("\n-----------------------------------------------------------------------------\n");

    sock_server_fd = create_ipv4_tcp_socket(); //création de la socket
    init_any_ipv4_address( & adresse, atoi(argv[1])); //initialisation de l'adrresse (remplissage de la structure)
    bind_address(sock_server_fd, & adresse); //nommage de la socket
    listen_socket(sock_server_fd, 1); //mise en mode passif de la socket

    //on rend la socket du serveur non bloquante
    if ((fcntl(sock_server_fd, F_SETFL, O_NONBLOCK)) == -1) {
        ncurses_stop();
        perror("Erreur lors du débloquage de la socket ");
        exit(EXIT_FAILURE);
    }

    printf("\nServeur : attente de connexion...\n");

    while (stop == 0) {
        //vérification si il y a encore des clients de connectés
        mutex_lock_check( & MUTEX_NB_CLIENT);
        if (count_client == 0 && EXIT_SERVER == TRUE) {
            stop = 1;
            printf("[Log] - Aucun client n'est connecté. Le serveur va stoper...\n");

        }
        mutex_unlock_check( & MUTEX_NB_CLIENT);

        sock_client_fd = accept_socket(sock_server_fd);

        if (sock_client_fd != -1) {
            mutex_lock_check( & MUTEX_NB_CLIENT);
            count_client++;
            if (count_client == 1) {
                EXIT_SERVER = TRUE;
            }
            mutex_unlock_check( & MUTEX_NB_CLIENT);

            //on rend la socket du client non bloquante
            if ((fcntl(sock_client_fd, F_SETFL, O_NONBLOCK)) == -1) {
                ncurses_stop();
                perror("Erreur lors du débloquage de la socket ");
                exit(EXIT_FAILURE);
            }

            if (count_client == 1) {
                client_threads = (pthread_t * ) malloc(sizeof(pthread_t) * 1);
            } else {
                client_threads = (pthread_t * ) realloc(client_threads, sizeof(pthread_t) * (count_client + 1));
            }

            data_client_routine_t * data = (data_client_routine_t * ) malloc_check(sizeof(data_client_routine_t));
            data -> socket_fd_client = sock_client_fd;
            data -> id_thread = count_client;

            create_thread_check( & client_threads[count_client - 1], client_routine, data);
        }
    }

    printf("[Log] - Annulation des threads des mosntres...\n");
    mutex_lock_check( & MUTEX_MONSTER_THREAD);
    if (nb_monster_thread != 0) {
        for (int i = 0; i < nb_monster_thread; i++) {
            pthread_cancel(monster_threads[i]);
            int statut = pthread_join(monster_threads[i], NULL);
            if (statut == 0) {
                printf("[Log - Monstre] - Thread %d : Stoppé\n", i + 1);
            }
        }
    }
    mutex_unlock_check( & MUTEX_MONSTER_THREAD);

    printf("[Log] - Annulation des threads des trésors... (peut prendre jusqu'à 2 minutes)\n");
    mutex_lock_check( & MUTEX_TREASURE_THREAD);
    if (nb_treasure_thread != 0) {
        for (int i = 0; i < nb_treasure_thread; i++) {
            pthread_cancel(treasure_threads[i]);
            int statut = pthread_join(treasure_threads[i], NULL);
            if (statut == 0) {
                printf("[Log - Trésor] - Thread %d : Stoppé\n", i + 1);
            }
        }
    }
    mutex_unlock_check( & MUTEX_TREASURE_THREAD);

    close_socket(sock_server_fd);

    free(client_threads);
    free(treasure_threads);
    free(monster_threads);

    printf("Serveur terminé.\n");

    return EXIT_SUCCESS;
}