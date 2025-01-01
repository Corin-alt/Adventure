/**
 * @file map_path.c
 * @brief Implémentation des fonctions nécessaire à la structures des chemins des cartes
 * @author Dupont Corentin & Lacroix Owen
 * @date 2022-03-30
 */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "map_path.h"
#include "../elements/map.h"
#include "../utils/macros.h"
#include "../utils/other_utils.h"

/**
 * @brief Permet de créer un noeud d'une map_path
 * 
 * @param current la map du noeud
 * @return node_map_path_t* la map_path
 */
node_map_path_t *create_node_map_path(map_t *current){
    node_map_path_t *n = (node_map_path_t *) malloc_check(sizeof(node_map_path_t) * 1);
    n->map = current;
    n->next = NULL;
    return n;
}

/**
 * @brief Libère l'espace mémoire d'un noeud d'une map_path
 * 
 * @param node le noeud
 */
void free_node_map_path(node_map_path_t **node){
    check_null(*node);
    free(*node);
    *node = NULL;
}

/**
 * @brief Créer et init une structure map path
 * 
 * @param spawn la map du spawn
 * @return map_path_t* la structure
 */
map_path_t *create_map_path(map_t *spawn){
    check_null(spawn);

    spawn->pos_x = 0;
    spawn->pos_y = 0;

    node_map_path_t *spawn_node = create_node_map_path(spawn);
    check_null(spawn_node);

    map_path_t *mp = (map_path_t *) malloc_check(sizeof(map_path_t) * 1);
    
    mp->head = spawn_node;
    mp->size = 1;

    return mp;
}

/**
 * @brief Insère une map dans le map path
 * 
 * @param map_path la structure 
 * @param map la map
 */
void insert_map_path(map_path_t *map_path, map_t *map) {
    check_null(map_path);
    check_null(map);
    
    node_map_path_t *node = create_node_map_path(map);
	check_null(node);
    
    node_map_path_t *current = map_path->head;
    
    while (current->next != NULL){
		current = current->next;
	}

	current->next = node;
	map_path->size++;
}

/**
 * @brief Permet de récupérer une map à partir de sa position dans la structure
 * 
 * @param map_path la structure 
 * @param pos_x coordonée X de la map dans la structure 
 * @param pos_y coordonée Y de la map dans la structure 
 * @return map_t* la map
 */
map_t *get_map_by_position_in_map_path(map_path_t *map_path, int pos_x, int pos_y){
    check_null(map_path);

    node_map_path_t *current = map_path->head;
   
    while (current != NULL){
        if(current->map->pos_x == pos_x && current->map->pos_y == pos_y){
            return current->map;    
        }
        current = current->next;
    }

    return NULL;
}



/**
 * @brief Vérifie si une map est contenu la structure
 * 
 * @param map_path la structure 
 * @param map la map
 * @return int 
 */
int contains_map_in_map_path(map_path_t *map_path, map_t *map){
    check_null(map_path);
    check_null(map);

    node_map_path_t *current = map_path->head;

    while (current != NULL){
        if(strcmp(current->map->name, map->name) == 0){
            return 1;    
        }
        current = current->next;
    }

    return 0;
}

/**
 * @brief Permet de savoir si une map existe déja dans la structure et la retourne si c'est le cas
 * 
 * @param map_path la structure 
 * @param pos_x coordonée X de la map dans la structure 
 * @param pos_y coordonée Y de la map dans la structure 
 * @return map_t* la map retournée si elle est contenue
 */
void connect_map_neighbour(map_path_t *map_path, map_t *map){
    check_null(map_path);
    check_null(map);
    map_t *neighbour_map;
    if(map->top == NULL){
        neighbour_map = get_map_by_position_in_map_path(map_path, map->pos_x, map->pos_y + 1);
        if (neighbour_map != NULL){
            neighbour_map->bottom = map->top;
            map->top = neighbour_map->bottom;
        }
    }
    
    if(map->bottom == NULL){
        neighbour_map = get_map_by_position_in_map_path(map_path, map->pos_x, map->pos_y - 1);
        if (neighbour_map != NULL){
            neighbour_map->top = map->bottom;
            map->bottom = neighbour_map->top;
        }
    }
    
    if(map->left == NULL){
        neighbour_map = get_map_by_position_in_map_path(map_path, map->pos_x - 1, map->pos_y);
        if (neighbour_map != NULL){
            neighbour_map->right = map->left;
            map->left = neighbour_map->right;
        }
    }
    
    if(map->right == NULL){
        neighbour_map = get_map_by_position_in_map_path(map_path, map->pos_x + 1, map->pos_y);
        if (neighbour_map != NULL){
            neighbour_map->left = map->right;
            map->right = neighbour_map->left;
        }
    }
}

/**
 * @brief Récupére ou génére une map depuis une direction d'une map source (changement de map quand le joueur se dépalace)
 * 
 * @param map_path la structure 
 * @param src la map source d'ou le joueur arrive pour passer à une autre map
 * @param direction la direction dans la quelle le joueur sort de la map source
 * @return map_t* la map récupéré ou générée
 */
map_t *get_or_generate_next_map(map_path_t *map_path, map_t *src, map_t *all_loaded_maps, int nb_maps, int direction){
    check_null(map_path);
    check_null(src);

    map_t *dest = NULL;

    if(direction == TOP){
        dest = get_map_by_position_in_map_path(map_path, src->pos_x , src->pos_y + 1);
    } else if (direction == BOTTOM){
        dest = get_map_by_position_in_map_path(map_path, src->pos_x , src->pos_y - 1);
    } else if (direction == LEFT){
        dest = get_map_by_position_in_map_path(map_path, src->pos_x - 1 , src->pos_y);
    } else if (direction == RIGHT){
        dest = get_map_by_position_in_map_path(map_path, src->pos_x + 1 , src->pos_y);      
    }
    
    if(dest == NULL && map_path->size != nb_maps){
        
        srand(time(NULL));
        int random = rand() % nb_maps;
        dest = &all_loaded_maps[random];
        
        while (contains_map_in_map_path(map_path, dest) == 1)
        {
            random = rand() % nb_maps;
            dest = &all_loaded_maps[random];
        }

        if(direction == TOP){
            dest->pos_x = src->pos_x;
            dest->pos_y = src->pos_y + 1;
        } else if (direction == BOTTOM){
            dest->pos_x = src->pos_x;
            dest->pos_y = src->pos_y - 1;
        } else if (direction == LEFT){
            dest->pos_x = src->pos_x - 1;
            dest->pos_y = src->pos_y ;
        } else if (direction == RIGHT){
            dest->pos_x = src->pos_x + 1;
            dest->pos_y = src->pos_y ; 
        }

        connect_map_neighbour(map_path, dest);
        insert_map_path(map_path, dest);
    }

    return dest;
}















