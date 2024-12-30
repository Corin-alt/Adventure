
/**
 * @file map_path.h
 * @brief Protoypes des fonctions nécessaire à la structures des chemins des cartes
 * @author Dupont Corentin & Lacroix Owen
 * @date 2022-03-30
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../elements/map.h"

#ifndef H_MAP_PATH
#define H_MAP_PATH


typedef struct node_map_path_t { 
    map_t *map;
    struct node_map_path_t *next;
} node_map_path_t;

/**
 * @brief Permet de créer un noeud d'une map_path
 * 
 * @param current la map du noeud
 * @return node_map_path_t* la map_path
 */
node_map_path_t *create_node_map_path(map_t *current);

/**
 * @brief Libère l'espace mémoire d'un noeud d'une map_path
 * 
 * @param node le noeud
 */
void free_node_map_path(node_map_path_t **node);


typedef struct {
    int size;
    int max_nb_maps;
    node_map_path_t *head;
} map_path_t;

/**
 * @brief Créer et init une structure map path
 * 
 * @param spawn la map du spawn
 * @return map_path_t* la structure
 */
map_path_t *create_map_path(map_t *spawn);

/**
 * @brief Insère une map dans le map path
 * 
 * @param map_path la structure 
 * @param map la map
 */
void insert_map_path(map_path_t *map_path, map_t *map);

/**
 * @brief Permet de récupérer une map à partir de sa position dans la structure
 * 
 * @param map_path la structure 
 * @param pos_x coordonée X de la map dans la structure 
 * @param pos_y coordonée Y de la map dans la structure 
 * @return map_t* la map
 */
map_t *get_map_by_position_in_map_path(map_path_t *map_path, int pos_x, int pos_y);


/**
 * @brief Vérifie si une map est contenu la structure
 * 
 * @param map_path la structure 
 * @param map la map
 * @return int 
 */
int contains_map_in_map_path(map_path_t *map_path, map_t *map);

/**
 * @brief Permet de connecter toutes les maps voisine 'si existantes) à une nouvelle map
 * 
 * @param map_path la structure 
 * @param map la nouvelle map
 */
void connect_map_neighbour(map_path_t *map_path, map_t *map);

/**
 * @brief Récupére ou génére une map de puis une direction d'une map source (changement de cmap quand le joueur se dépalace)
 * 
 * @param map_path la structure 
 * @param src la map source d'ou le joueur arrive pour passer à uen autre map
 * @param direction la direction dans la quelle le joueur sort de la map source
 * @return map_t* la map récupéré ou générée
 */
map_t *get_or_generate_next_map(map_path_t *map_path, map_t *src, map_t *all_loaded_maps, int nb_maps, int direction);


#endif







