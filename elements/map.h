/**
 * @file map.h
 * @author Dupont Corentin & Lacroix Owen
 * @brief Protoypes des fonctions nécessaire à la structure case_t & map_t
 * @date 2022-02-19
 */

#ifndef _map_
#define _map_

#include <pthread.h>
#include "map.h"
#include "../elements/entity.h"
#include "../utils/macros.h"

/**
 * Structure d'une case
 */
typedef struct {
    int coord_x;
    int coord_y;
    char symbol;
    unsigned int biome;
    int id_entity;
}
case_t;

/**
 * @brief Permet d'initaliser uen case de la map
 * 
 * @param [in, out] c la case
 * @param [in] biome le biome / environnement c'est à dire la couleur de fond de la case
 * @param [in] symbol le symbole de l'objet
 */
void init_case(case_t * c,  int coord_x, int coord_y, char symbol, int biome, int id_entity);

/**
 * @brief Libére l'espace mémoire d'une case
 * 
 * @param [in] c la case
 */
void free_case(case_t **c);

/**
 * Structure d'une map
 */
typedef struct map_t{
    char *name;

    int pos_x;
    int pos_y;

    struct map_t *top;
    struct map_t *bottom;
    struct map_t *left;
    struct map_t *right;

    unsigned short height;
    unsigned short width;
    unsigned short nb_entity;
    entity_t *entities;
    case_t *cases;

    pthread_mutex_t MUTEX_CASES[800];
}
map_t;

int count_maps();
map_t *load_all_maps();
map_t *get_loaded_map_by_name(map_t *list_maps, int nb_maps, char*name);


/**
 * @brief Permet de charger une map depuis son fichier binaire associé
 * 
 * @param [in] filename nom du fichier
 * @return map_t* la map
 */
map_t * load_map(char * filename);

/**
 * Permet de sauvegarder une map dans un fichier binaire
 * 
 * @param [in] filename le nom du fichier
 * @param [in] map la map
 */
void save_map(char *filename, map_t *map);

/**
 * Permet de créer une map par défaut (vide)
 * 
 * @param [in] height la hauteur de la map
 * @param [in] witdh la largeur de la map
 * @return map_t* la map
 */
map_t *init_empty_map(char*name, unsigned short height, unsigned short width);

/**
 * @brief Ajoute une entité à une case de la map
 * 
 * @param map la map
 * @param ca  la case
 * @param entity l'entité
 */
void add_entity(map_t * map, case_t *ca, entity_t *entity);

/**
 * @brief Récupére l'entité de la case
 * 
 * @param map la map
 * @param ca  la case
 * @return entity_t l'entité
 */
entity_t  get_entity(map_t *map, case_t *ca);

/**
 * @brief Récupére le nombre de monstre sur la map
 * 
 * @param map la map
 * @return int le nombre de monstre
 */
int get_nb_monster_on_map(map_t *map);

/**
 * @brief Récupére le nombre d'artefact sur la map
 * 
 * @param map la map
 * @return int le le nombre d'artefact 
 */
int get_nb_artefact_on_map(map_t *map);

/**
 * @brief Récupére tous les monstres présent sur la map
 * 
 * @param map la map
 * @return entity_t* la liste de mosntre
 */
entity_t *get_all_monster_on_map(map_t *map);

/**
 * @brief Ajoute une case à la map
 * 
 * @param [in, out] map la map
 * @param [in] c la case
 * @param [in] x la position X de la case
 * @param [in] y la position Y de la case
 */
void add_case(map_t * map, case_t * c, int x, int y);

/**
 * Récupére la case associé à une position de la map
 * 
 * @param [in] map la map
 * @param [in] x la position X de la map
 * @param [in] y la position Y de la map
 * @return case_t* la case
 */
case_t * get_case(map_t * map, int x, int y);

/**
 * @brief Récupére le mutex de la case
 * 
 * @param [in] map la map
 * @param [in] x la position X de la map
 * @param [in] y la position Y de la map
 * @return pthread_mutex_t le mutex
 */
pthread_mutex_t *get_mutex_case(map_t * map, int x, int y);

/**
 * @brief Vérouille tous les mutex de la map
 * 
 * @param map la map
 */
void lock_map(map_t * map);

/**
 * @brief Dévérouille tous les mutex de la map
 * 
 * @param map la map
 */
void unlock_map(map_t * map);

/**
 * @brief Essauye de vérouille tous les mutex de la map
 * 
 * @param map la map
 */
void trylock_map(map_t * map);



/**
 * @brief Récupére une case aléatoire sur la map
 * 
 * @param map la map
 * @return case_t*  la case
 */
case_t *get_random_case(map_t * map);

/**
 * @brief Vérifie si une case est spawnable
 * 
 * @param ca la case 
 * @return int 
 */
int is_spawnable_spawn(case_t *ca);

/**
 * @brief Récupére une case aléatoire d'une map
 * 
 * @param [in] map la map
 * @param [in] x la position X de la map
 * @param [in] y la position Y de la map
 * @return case_t* la case
 */
case_t * get_random_case_to_spawn(map_t * map);

/**
 * @brief Permet de savoir si une case est valide pour se deplacer dessus
 * 
 * @param [in] map la map
 * @param [in] x la position X de la case
 * @param [in] y la position Y de la case
 * @return int 
 */
int can_move(map_t *map, int x, int y);

/**
 * @brief Permet de vérifier si le hero/monstre sortent de la map
 * 
 * @param [in] x la position X du deplacement
 * @param [in] y la position y du deplacement
 * @return int 
 */
int is_out_of_map(int x, int y);

/**
 * @brief Récupére la direction de la sortie de map
 * 
 * @param [in] x la position X du deplacement
 * @param [in] y la position y du deplacement
 * @return int 
 */
int get_direction_out_of_map(int x, int y);

/**
 * @brief Permet de savoir si sur la case il y a un monstre
 * 
 * @param map la map
 * @param x la position X de la case
 * @param y la position Y de la case
 * @return int 
 */
int is_monster(map_t *map, int x, int y);

/**
 * @brief Permet de savoir si sur la case il y a un artefact
 * 
 * @param map la map
 * @param x coord x de la case
 * @param y coord y de la case
 * @return int 
 */
int is_artefact(map_t *map, int x, int y);
/**
 * @brief Permet de savoir si sur la case il y a un tresor
 * 
 * @param map la map
 * @param x coord x de la case
 * @param y coord y de la case
 * @return int 
 */
int is_tresor(map_t *map, int x, int y);

void spawn_tresor(map_t *map); 

/**
 * @brief Permet de savoir si sur la case il y a un hero
 * 
 * @param map la map
 * @param x coord x de la case
 * @param y coord y de la case
 * @return int 
 */
int is_hero(map_t *map, int x, int y);

/**
 * @brief Libére l'espace mémoire d'une map
 * 
 * @param [in] map la map
 */
void free_map(map_t **map);

#endif