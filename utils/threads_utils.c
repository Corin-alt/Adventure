/**
 * @file threads_utils.c
 * @brief Implémentation des fonctions utiles aux threads
 * @author Dupont Corentin & Lacroix Owen
 * @date 2022-03-14
 */

#include <stdio.h>
#include <stdlib.h>
#include "threads_utils.h"
#include "ncurses_utils.h"
#include "ncurses.h"

/**
 * @brief Permet de créer un thread avec une vérification
 * 
 * @param thread le thread
 * @param func la routine du thread
 * @param arg les arguments passés au thread
 */
void create_thread_check(pthread_t *thread, void *(*start_routine)(void *), void *arg) {
    int state;
    if ((state = pthread_create(thread, NULL, start_routine, arg)) != 0) {
        ncurses_stop();
        fprintf(stderr, "An error occurred while creating the thread: error %d\n", state);
        exit(EXIT_FAILURE);
    }
}


/**
 * @brief Permet d'initialiser un mutex avec une vérification
 * 
 * @param mutex le mutex
 */
void mutex_init_check(pthread_mutex_t *mutex) {
    int state;
    if ((state = pthread_mutex_init(mutex, NULL)) != 0) {
        ncurses_stop();
        fprintf(stderr, "An error occurred while trying to lock the mutex: error %d\n", state);
        exit(EXIT_FAILURE);
    }
}



/**
 * @brief Permet de vérouiller un mutex avec une vérification
 * 
 * @param mutex le mutex
 */
void mutex_lock_check(pthread_mutex_t *mutex) {
    int state;
    if ((state = pthread_mutex_lock(mutex)) != 0) {
        ncurses_stop();
        fprintf(stderr, "An error occurred while trying to lock the mutex: error %d\n", state);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet de dévérouiller un mutex avec une vérification
 * 
 * @param mutex le mutex
 */
void mutex_unlock_check(pthread_mutex_t *mutex) {
    int state;
    if ((state = pthread_mutex_unlock(mutex)) != 0) {
        ncurses_stop();
        fprintf(stderr, "An error occurred while trying to unlock the mutex, error: %d\n", state);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet de broadcaster une condition avec une vérification
 * 
 * @param mutex le mutex
 */
void mutex_cond_broadcast_check(pthread_cond_t *cond) {
    int state;
    if ((state = pthread_cond_broadcast(cond)) != 0) {
        ncurses_stop();
        fprintf(stderr, "An error occurred while trying to unlock the mutex, error: %d\n", state);
        exit(EXIT_FAILURE);
    }
}




