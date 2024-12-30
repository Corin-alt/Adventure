/**
 * @file threads_utils.h
 * @brief Protoypes des fonctions utiles aux threads
 * @author Dupont Corentin & Lacroix Owen
 * @date 2022-03-14
 */

#ifndef H_THREADS_UTILS
#define H_THREADS_UTILS

#include <pthread.h>

/**
 * @brief Permet de créer un thread avec une vérification
 * 
 * @param thread le thread
 * @param func la routine du thread
 * @param arg les arguments passés au thread
 */
void create_thread_check(pthread_t *thread, void *(*start_routine)(void *), void *arg);

/**
 * @brief Permet de vérouiller un mutex avec une vérification
 * 
 * @param mutex le mutex
 */
void mutex_lock_check(pthread_mutex_t *mutex);

/**
 * @brief Permet d'initialiser un mutex avec une vérification
 * 
 * @param mutex le mutex
 */
void mutex_init_check(pthread_mutex_t *mutex);

/**
 * @brief Permet de dévérouiller un mutex avec une vérification
 * 
 * @param mutex le mutex
 */
void mutex_unlock_check(pthread_mutex_t *mutex);

/**
 * @brief Permet de broadcaster une condition avec une vérification
 * 
 * @param mutex le mutex
 */
void mutex_cond_broadcast_check(pthread_cond_t *cond);
#endif