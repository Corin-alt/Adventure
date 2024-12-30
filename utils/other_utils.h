#ifndef H_OTHER_UTILS
#define H_OTHER_UTILS

/**
 * @brief Permet de créer un allocation mémoire et de vérifier 
 * 
 * @param size la taille à allouer
 * @return void* la variable allouée
 */
void *malloc_check(size_t size);

/**
 * @brief Permet de vérifier si une valeur est null
 * 
 * @param arg la variable
 */
void check_null(void *arg);

/**
 * @brief Permet de convertir la vitesse déplacement théorique en une valeur compréhesible pour le thread
 * 
 * @param vd vitesse théorique
 * @return unsigned int la vitesse
 */
unsigned int convert_theoric_move_speed_for_thread(int vd);

#endif