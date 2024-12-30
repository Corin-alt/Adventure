/**
 * @file threads_utils.c
 * @brief Fonctions utiles aux threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "threads_utils.h"
#include "ncurses_utils.h"

#define MAX_RETRIES 3
#define RETRY_DELAY 1000 // 1ms

void create_thread_check(pthread_t *thread, void *(*start_routine)(void *), void *arg) {
    if (!thread || !start_routine) return;

    int ret, retries = 0;
    while (retries < MAX_RETRIES) {
        ret = pthread_create(thread, NULL, start_routine, arg);
        if (ret == 0 || ret != EAGAIN) break;
        usleep(RETRY_DELAY);
        retries++;
    }

    if (ret != 0) {
        ncurses_stop();
        fprintf(stderr, "Création thread échouée: erreur %d\n", ret);
        exit(EXIT_FAILURE);
    }
}

void mutex_init_check(pthread_mutex_t *mutex) {
    if (!mutex) return;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    int ret = pthread_mutex_init(mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    if (ret != 0) {
        ncurses_stop();
        fprintf(stderr, "Init mutex échoué: erreur %d\n", ret);
        exit(EXIT_FAILURE);
    }
}

void mutex_lock_check(pthread_mutex_t *mutex) {
    if (!mutex) return;

    int ret, retries = 0;
    while (retries < MAX_RETRIES) {
        ret = pthread_mutex_lock(mutex);
        if (ret == 0 || ret != EAGAIN) break;
        usleep(RETRY_DELAY);
        retries++;
    }

    if (ret != 0) {
        ncurses_stop();
        fprintf(stderr, "Verrouillage mutex échoué: erreur %d\n", ret);
        exit(EXIT_FAILURE);
    }
}

void mutex_unlock_check(pthread_mutex_t *mutex) {
    if (!mutex) return;

    int ret = pthread_mutex_unlock(mutex);
    if (ret != 0) {
        ncurses_stop();
        fprintf(stderr, "Déverrouillage mutex échoué: erreur %d\n", ret);
        exit(EXIT_FAILURE);
    }
}

void mutex_cond_broadcast_check(pthread_cond_t *cond) {
    if (!cond) return;

    int ret = pthread_cond_broadcast(cond);
    if (ret != 0) {
        ncurses_stop();
        fprintf(stderr, "Broadcast condition échoué: erreur %d\n", ret);
        exit(EXIT_FAILURE);
    }
}

void mutex_destroy_check(pthread_mutex_t *mutex) {
    if (!mutex) return;

    int ret = pthread_mutex_destroy(mutex);
    if (ret != 0) {
        ncurses_stop();
        fprintf(stderr, "Destruction mutex échouée: erreur %d\n", ret);
        exit(EXIT_FAILURE);
    }
}

void thread_join_check(pthread_t thread, void **retval) {
    int ret = pthread_join(thread, retval);
    if (ret != 0) {
        ncurses_stop();
        fprintf(stderr, "Join thread échoué: erreur %d\n", ret);
        exit(EXIT_FAILURE);
    }
}

void thread_cancel_check(pthread_t thread) {
    int ret = pthread_cancel(thread);
    if (ret != 0) {
        ncurses_stop();
        fprintf(stderr, "Cancel thread échoué: erreur %d\n", ret);
        exit(EXIT_FAILURE);
    }
}

void mutex_trylock_check(pthread_mutex_t *mutex) {
    if (!mutex) return;

    int ret = pthread_mutex_trylock(mutex);
    if (ret != 0 && ret != EBUSY) {
        ncurses_stop();
        fprintf(stderr, "Trylock mutex échoué: erreur %d\n", ret);
        exit(EXIT_FAILURE);
    }
}

void cond_wait_check(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    if (!cond || !mutex) return;

    int ret = pthread_cond_wait(cond, mutex);
    if (ret != 0) {
        ncurses_stop();
        fprintf(stderr, "Wait condition échoué: erreur %d\n", ret);
        exit(EXIT_FAILURE);
    }
}