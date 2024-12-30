#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "macros.h"
#include "other_utils.h"
#include "ncurses_utils.h"

/**
 * @brief Permet de créer un allocation mémoire et de vérifier 
 * 
 * @param size la taille à allouer
 * @return void* la variable allouée
 */
void *malloc_check(size_t size) {
    void *var;
    if ((var = malloc(size)) == NULL) {
        ncurses_stop();
        fprintf(stderr, "An error occurred while trying to allocate memory\n");
        exit(EXIT_FAILURE);
    }
    return var;
}


/**
 * @brief Permet de vérifier si une valeur est null
 * 
 * @param arg la variable
 */
void check_null(void * arg){
    if (arg == NULL){
        ncurses_stop();
        fprintf(stderr, "NullPointerException\n");
        exit(EXIT_FAILURE);
    }
}


/**
 * @brief Permet de convertir la vitesse déplacement théorique en une valeur compréhesible pour le thread
 * 
 * @param vd vitesse théorique
 * @return unsigned int la vitesse
 */
unsigned int convert_theoric_move_speed_for_thread(int vd){
    if (vd == 1){
        return MAXIMUM_MOVE_SPEED_IN_SEC * 1000000;
    } else {
        int compute = (MAXIMUM_MOVE_SPEED_IN_SEC - vd * MOVE_SPEED_BY_POINT_IN_SEC);
        if(compute >= MINIMUM_MOVE_SPEED_IN_SEC ){
            return compute * 1000000;
        } else {
            return MINIMUM_MOVE_SPEED_IN_SEC * 1000000;
        }
    }
}