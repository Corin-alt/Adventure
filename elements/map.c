/**
 * @file map.c
 * @brief Implémentation des fonctions lièes à la structure case_t & map_t
 * @author Dupont Corentin & Lacroix Owen
 * @date 2022-02-19
 */
#include <dirent.h> 
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "../utils/ncurses_utils.h"
#include "../utils/files_utils.h"
#include "../utils/other_utils.h"
#include "../utils/threads_utils.h"
#include "../utils/macros.h"
#include "../elements/entity.h"
#include "map.h"

/**
 * @brief Permet d'initaliser uen case de la map
 * 
 * @param [in, out] c la case
 * @param [in] biome le biome / environnement c'est à dire la couleur de fond de la case
 * @param [in] symbol le symbole de l'objet
 */
void init_case(case_t * c, int coord_x, int coord_y, char symbol, int biome, int id_entity) {
    check_null(c);
    c->coord_x = coord_x;
    c->coord_y = coord_y;
    c -> symbol = symbol;
    c -> biome = biome;
    c -> id_entity = id_entity;
}

/**
 * @brief Libére l'espace mémoire d'une case
 * 
 * @param [in] c la case
 */
void free_case(case_t **c) {
    check_null(*c);
    free(*c);
    *c = NULL;
}


int count_maps(){
    int nb = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(PATH_LOCATION_MAPS_FILES);
    if (d) {
        while ((dir = readdir(d)) != NULL) {   
            if(strstr(dir->d_name, ".bin")){
                nb++;
            }
        }
        closedir(d);
    }

    return nb;
}

map_t *load_all_maps(){
    int index = 0;
    int nb_maps = count_maps();
    map_t *maps = (map_t*) malloc_check(sizeof(map_t) *nb_maps);
    DIR *d;
    struct dirent *dir;
    d = opendir(PATH_LOCATION_MAPS_FILES);
    if (d) {
        while ((dir = readdir(d)) != NULL) {   
            if(strstr(dir->d_name, ".bin")){
                char file[256] = PATH_LOCATION_MAPS_FILES;
                strcat(file, dir->d_name);
                printf("%s\n", file);

                maps[index] = *load_map(file);

                file[0] = '\0'; //vide la variable
                index++;
            }
        }
        closedir(d);
    }
    return maps;
}


map_t *get_loaded_map_by_name(map_t *list_maps, int nb_maps, char*name){
    check_null(list_maps);
    for (int i = 0; i < nb_maps; i++){
        if(strcmp(list_maps[i].name, name) == 0){
            return &list_maps[i];
        }
    }
    return NULL;
}

/**
 * @brief Permet de charger une map depuis son fichier binaire associé
 * 
 * @param [in] filename nom du fichier
 * @return map_t* la map
 */
map_t *load_map(char * filename) {
    check_null(filename);
    map_t *map = NULL;
    int fd;

    char file[256] = PATH_LOCATION_MAPS_FILES;
    strcat(file, filename);

    if ((fd = open(file, O_RDONLY)) == -1) {
        if (errno != ENOENT) {
            fprintf(stderr, "Erreur lors de l'ouverture du fichier %s ", file);
            perror("");
            exit(EXIT_FAILURE);
        }
    }

    if (fd != -1) {
        map = (map_t*) malloc_check(sizeof(map_t));
        read_map(fd, map);
    
        for (int i = 0; i < NB_COL * NB_LIGNE; i++){
            mutex_init_check(&map->MUTEX_CASES[i]);
        }
    }

    return map;
}

/**
 * @brief Permet de sauvegarder une map dans un fichier binaire
 * 
 * @param [in] filename le nom du fichier
 * @param [in] map la map
 */
void save_map(char * filename, map_t * map) {
    check_null(filename);
    check_null(map);

    int fd;
    char file[256] = PATH_LOCATION_MAPS_FILES;
    strcat(file,filename);

    if ((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s ", file);
        perror("");
        exit(EXIT_FAILURE);
    }

    write_map(fd, map);

    if (close(fd) == -1) {
        perror("Erreur lors de la fermeture du fichier ");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet de créer une map par défaut (vide)
 * 
 * @param [in] height la hauteur de la map
 * @param [in] witdh la largeur de la map
 * @return map_t* la map
 */
map_t *init_empty_map(char* name, unsigned short height, unsigned short witdh) {
    check_null(name);
   
    map_t * map;
    map = (map_t*) malloc_check(sizeof(map_t));

    map->name = name;
    map->pos_x = -1;
    map->pos_y = -1;
    map->top = NULL;
    map->bottom = NULL;
    map->left = NULL;
    map->right = NULL;

    map -> height = height;
    map -> width = witdh;
    map -> nb_entity = 0;
    map -> entities = NULL;

    map->cases = (case_t*) malloc_check(sizeof(case_t) * height * witdh);

    for (int i = 0; i < NB_LIGNE; i++) {
        for (int j = 0; j < NB_COL; j++) {
            case_t c;
            init_case(&c, i, j,  SYMB_EMPTY_CASE, HERBE, -1);
            map -> cases[i * map->width + j] = c;
        }
    }

    for (int i = 0; i < NB_COL * NB_LIGNE; i++){
        mutex_init_check(&map->MUTEX_CASES[i]);
    }       

    return map;
}

/**
 * @brief Ajoute une entité à une case de la map
 * 
 * @param map la map
 * @param ca  la case
 * @param entity l'entité
 */
void add_entity(map_t * map, case_t *ca, entity_t *entity){  
    check_null(map);
    check_null(ca);

    if(map->entities == NULL){
        if((map->entities = (entity_t *) malloc(sizeof(entity_t) * 1)) == NULL){
            ncurses_stop();
            perror("Erreur lors de l'allocation du tableau d'entités ");
            exit(EXIT_FAILURE);
        }
      
    } else {
        if((map->entities = (entity_t *) realloc(map->entities, sizeof(entity_t) * (map->nb_entity + 1))) == NULL){
            ncurses_stop();
            perror("Erreur lors de la réaloaction du tableau d'entités ");
            exit(EXIT_FAILURE);
        }
    }

    map->entities[map->nb_entity] = *entity;
    ca->id_entity = map->nb_entity;

    map->nb_entity++;
}

/**
 * @brief Récupére l'entité de la case
 * 
 * @param map la map
 * @param ca  la case
 * @return entity_t l'entité
 */
entity_t get_entity(map_t *map, case_t *ca){
    check_null(map);
    check_null(ca);

    return map->entities[ca->id_entity];
}


/**
 * @brief Récupére le nombre de monstre sur la map
 * 
 * @param map la map
 * @return int le nombre de monstre
 */
int get_nb_monster_on_map(map_t *map){
    int nb = 0;
    if (map->nb_entity != 0){
        if (map->nb_entity == 1){
            return 1;
        } else {
            for (int i = 0; i < map->nb_entity; i++){
                if(map->entities[i].type == MONSTER_TYPE){
                    nb++;
                }
            }
        }
    }
    return nb;
}

/**
 * @brief Récupére le nombre d'artefact sur la map
 * 
 * @param map la map
 * @return int le le nombre d'artefact 
 */
int get_nb_artefact_on_map(map_t *map){
    int nb = 0;
    if (map->nb_entity != 0){
        if (map->nb_entity == 1){
            return 1;
        } else {
            for (int i = 0; i < map->nb_entity; i++){
                if(map->entities[i].type == ARTEFACT_TYPE){
                    nb++;
                }
            }
        }
    }
    return nb;
}


/**
 * @brief Récupére tous les monstres présent sur la map
 * 
 * @param map la map
 * @return entity_t* la liste de mosntre
 */
entity_t *get_all_monster_on_map(map_t *map){
    int nb = get_nb_monster_on_map(map);
    int index = 0;
    entity_t *all_monsters = (entity_t *)malloc_check(sizeof(entity_t) * nb);
    for (int i = 0; i < map->nb_entity; i++){
        if(map->entities[i].type == MONSTER_TYPE){
            all_monsters[index] = map->entities[i];
            index++;
        }    
    }

    return all_monsters;         
}


/**
 * @brief Ajoute une case à la map
 * 
 * @param [in, out] map la map
 * @param [in] c la case
 * @param [in] x la position X de la case
 * @param [in] y la position Y de la case
 */
void add_case(map_t * map, case_t * c, int x, int y) {
    check_null(map);
    check_null(c);

    if ((x > NB_LIGNE || x < 0) || (y > NB_COL || y < 0)) {
        return;
    }

    map -> cases[y * map->width + x] = * c;
}

/**
 * @brief Récupére la case associé à une position de la map
 * 
 * @param [in] map la map
 * @param [in] x la position X de la map
 * @param [in] y la position Y de la map
 * @return case_t* la case
 */
case_t * get_case(map_t * map, int x, int y) {
    check_null(map);

    if ((x > NB_LIGNE || x < 0) || (y > NB_COL || y < 0)) {
        return NULL;
    }

    return &map -> cases[x * map->width + y];
}


/**
 * @brief Récupére le mutex de la case
 * 
 * @param [in] map la map
 * @param [in] x la position X de la map
 * @param [in] y la position Y de la map
 * @return pthread_mutex_t le mutex
 */
pthread_mutex_t *get_mutex_case(map_t * map, int x, int y){
    check_null(map);
    return &map -> MUTEX_CASES[x * map->width + y];
}

/**
 * @brief Vérouille tous les mutex de la map
 * 
 * @param map la map
 */
void lock_map(map_t * map){
    check_null(map);
    for (int i = 0; i < NB_COL * NB_LIGNE; i++){
        mutex_lock_check(&map->MUTEX_CASES[i]);
    }
}

/**
 * @brief Dévérouille tous les mutex de la map
 * 
 * @param map la map
 */
void unlock_map(map_t * map){
    check_null(map);
    for (int i = 0; i < NB_COL * NB_LIGNE; i++){
        mutex_unlock_check(&map->MUTEX_CASES[i]);
    }
}

/**
 * @brief Essauye de vérouille tous les mutex de la map
 * 
 * @param map la map
 */
void trylock_map(map_t * map){
    check_null(map);
    for (int i = 0; i < NB_COL * NB_LIGNE; i++){
        pthread_mutex_trylock(&map->MUTEX_CASES[i]);
    }
}



/**
 * @brief Récupére une case aléatoire sur la map
 * 
 * @param map la map
 * @return case_t*  la case
 */
case_t * get_random_case(map_t * map) {
    srand(time(NULL));
    check_null(map);
    
    int x = rand() % NB_LIGNE - 1;
    int y = rand() % NB_COL - 1;
   
    return get_case(map, abs(x), abs(y));
}

/**
 * @brief Vérifie si une case est spawnable
 * 
 * @param ca la case 
 * @return int 
 */
int is_spawnable_spawn(case_t *ca){
    check_null(ca);
    return ca->biome != EAU && ca->symbol == SYMB_EMPTY_CASE;
}

/**
 * @brief Récupére une case aléatoire d'une map
 * 
 * @param [in] map la map
 * @param [in] x la position X de la map
 * @param [in] y la position Y de la map
 * @return case_t* la case
 */
case_t * get_random_case_to_spawn(map_t * map) {
    check_null(map);

    case_t *ca = get_random_case(map);

    while (!is_spawnable_spawn(ca))
    {
       ca = get_random_case(map);
    }
   
    return ca;
}

/**
 * @brief Permet de savoir si une case est valide pour se deplacer dessus
 * 
 * @param [in] map la map
 * @param [in] x la position X de la case
 * @param [in] y la position Y de la case
 * @return int 
 */
int can_move(map_t *map, int x, int y) {
    check_null(map);
    case_t *c = get_case(map, x, y);
    return c->biome != EAU && c->symbol != SYMB_OBSTACLE;
}

/**
 * @brief Permet de vérifier si le hero/monstre sortent de la map
 * 
 * @param [in] x la position X du deplacement
 * @param [in] y la position y du deplacement
 * @return int 
 */
int is_out_of_map(int x, int y) {
    return x < 0 || y < 0 || y >= NB_COL || x >= NB_LIGNE ;
}

/**
 * @brief Récupére la direction de la sortie de map MERCI NCURSES DE TOUT INVERSER
 * 
 * @param [in] x la position X du deplacement
 * @param [in] y la position y du deplacement
 * @return int 
 */
int get_direction_out_of_map(int x, int y) {
   if (x < 0) {
       return TOP;
   } else if (x >= NB_LIGNE){
       return BOTTOM;
   } else if (y < 0){
       return LEFT;
   } else if (y >= NB_COL){
       return RIGHT;
   } else {
       return -1;
   }
}

/**
 * @brief Permet de savoir si sur la case il y a un monstre
 * 
 * @param map la map
 * @param x la position X de la case
 * @param y la position Y de la case
 * @return int 
 */
int is_monster(map_t *map, int x, int y){
    check_null(map);
    case_t *c = get_case(map, x, y);
    return c->symbol == SYMB_MONSTRE;
}

/**
 * @brief Permet de savoir si sur la case il y a un artefact
 * 
 * @param map la map
 * @param x coord x de la case
 * @param y coord y de la case
 * @return int 
 */
int is_artefact(map_t *map, int x, int y){
    check_null(map);
    case_t *c = get_case(map, x, y);
    return c->symbol == SYMB_ARTEFACT;
}

/**
 * @brief Permet de savoir si sur la case il y a un tresor
 * 
 * @param map la map
 * @param x coord x de la case
 * @param y coord y de la case
 * @return int 
 */
int is_tresor(map_t *map, int x, int y){
    check_null(map);
    case_t *c = get_case(map, x, y);
    return c->symbol == SYMB_TRESOR;
}


void spawn_tresor(map_t *map){
    check_null(map);
    case_t *c = get_random_case_to_spawn(map);
    c->symbol = SYMB_TRESOR;
}


/**
 * @brief Permet de savoir si sur la case il y a un hero
 * 
 * @param map la map
 * @param x coord x de la case
 * @param y coord y de la case
 * @return int 
 */
int is_hero(map_t *map, int x, int y){
    check_null(map);
    case_t *c = get_case(map, x, y);
    return c->symbol == SYMB_HERO;
}

/**
 * @brief Libére l'espace mémoire d'une map
 * 
 * @param [in] map la map
 */
void free_map(map_t **map) {
    check_null(*map);
    /**
    check_null(*map);

    for (int i = 0; i < (*map)->nb_entity; i++)
    {
        entity_t *entity = &(*map)->entities[i];
        free_entity(&entity);
    }
   
    for (int i = 0; i < NB_LIGNE; i++) {
        for (int j = 0; j < NB_COL; j++) {
            case_t *ca = get_case(*map, i, j);
            free_case(&ca);
        }   
    }
    */
    free(*map);
    *map = NULL;
}