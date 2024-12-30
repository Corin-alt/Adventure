#ifndef _HERO_
#define _HERO_

#include "map.h"
#include "../utils/macros.h"

/**
 * S'il meurt, le hero respawn avec ses valeurs par defaut
 * Ne peut pas se deplacer sur l'eau et obstacle
 * Gagne des pts d'exp en tuant des monstres / adversaires -> atteint à niveau il augmente tous les attributs
 */


/**
 * @brief Struture d'une entité
 */
typedef struct {
    //point de spawn
    int x_spawn;
    int y_spawn;

    //map courrant
    int map_x;
    int map_y;

    //postion courrante
    int coord_x;
    int coord_y;

    //attributs
    int max_health;
    int health;
    int armor;
    int strength;
    int move_speed;
    int hit_speed;
    int xp;
    int nb_piece;

    int nb_art_in_inv;
    entity_t inventory[INVENTORY_SIZE];
    
}
hero_t;

/**
 * @brief Créer un hera avec les attribut par defaut
 * 
 * @return hero_t* le hero
 */
hero_t *create_default_hero();

/**
 * @brief Permet de faire spawn le hero uen case valide d'une
 * 
 * @param map la map
 * @param hero le hero
 */
void spawn_hero(map_t *map, hero_t *hero);

/**
 * @brief Permet d'augmenter tous les attributs du hero quand il passe un niveau
 * 
 * @param hero 
 */
void attributes_growth(hero_t *hero);

int add_exp(hero_t *hero, int xp);

/**
 * @brief permet au hero de récupérer un trésor
 * 
 * @param hero le hero
 * @param win_info fenêtre d'info
 */
void get_tresor(hero_t *hero, fenetre_t *win_info);

/**
 * @brief Permet de d'ajouter et/ou remplacer un artefact dans l'inventaire
 * 
 * @param hero le hero
 * @param artefact l'artefact
 * @param index l'emplacement dans l'inventaire
 * @return l'artefact à replacer sur la map si on remplace dans l'inventaire 
 */
entity_t add_artefact_in_inventory(hero_t *hero, entity_t artefact, int index);

/**
 * @brief Permet d'appliquer les effets d'un l'artefact au l'hero
 * 
 * @param hero l'hero
 * @param artefact l'artefact
 */
void apply_artefact_attribute(hero_t *hero, entity_t artefact);

/**
 * @brief Permet de retirer les effets d'un l'artefact au l'hero
 * 
 * @param hero l'hero
 * @param artefact l'artefact
 */
void remove_artefact_attribute(hero_t *hero, entity_t artefact);

/**
 * @brief Permet de reset tous les attributs du hero
 * 
 * @param hero le hero
 */
void reset_attribute_value(hero_t *hero);

/**
 * @brief Affiche les informations du hero
 * 
 * @param hero le hero
 * @param win_hero la fenêtre d'info du hero
 */
void print_hero_stat(hero_t *hero, fenetre_t *win_hero);

/**
 * @brief Libière l'espace mémoire d'un héro
 * 
 * @param hero 
 */
void free_hero(hero_t **hero);

#endif