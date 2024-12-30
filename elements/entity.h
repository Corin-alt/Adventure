/**
 * @file monster.h
 * @author Dupont Corentin & Lacroix Owen
 * @brief Protoypes des fonctions nécessaire à la structure entity_t
 * @date 2022-02-19
 */

#ifndef _ENTITY_
#define _ENTITY_

#include "../utils/fenetre.h"

/**
 * @brief Struture d'une entité
 */
typedef struct {
    int type;
    char *name;
    int health;
    int armor;
    int strength;
    int move_speed;
    int hit_speed;
    
    int coord_x;
    int coord_y;

    int index;

    int can_attack;
    int id_thread;
}
entity_t;

/**
 * @brief Permer de créer une entité
 * 
 * @param [in, out] entity l'entité
 * @param [in] infos la fenêtre d'infos
 * @param [in] fiche la fenêtre de la fiche
 */
void create_entity(entity_t *entity, fenetre_t * infos, fenetre_t * fiche, char symb);

/**
 * @brief Affiche les info de l'entité dans la fenêtre de fiche
 * 
 * @param [in] entity l'entité
 * @param [in] fiche la fenêtre de la fiche
 * @return int erreur ou succés
 */
int print_fiche_entity(entity_t entity, fenetre_t * fiche, char symb);



void dispaly_colored_stat_entity(entity_t entity, fenetre_t * fiche);

/**
 * @brief Libére l'espace mémoire d'une entité
 * 
 * @param [in] entity l'entité
 * @return int erreur ou succés
 */
int free_entity(entity_t **entity);

#endif