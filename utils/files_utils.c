/**
 * @file files_utils.c
 * @brief Implémentation des fonctions nécessaire aux fichiers
 * @author Dupont Corentin & Lacroix Owen
 * @date 2022-03-14
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "macros.h"
#include "../elements/map.h"
#include "../elements/entity.h"
#include "../elements/hero.h"
#include "other_utils.h"
#include "ncurses_utils.h"
#include "include.h"

/**
 * @brief Permet d'écrire un "char *" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui sera écrite
 */
void write_string(int fd, char *str){
    size_t taille;
    taille = (strlen(str) + 1) * sizeof(char);
    if (write(fd, &taille, sizeof(size_t)) == -1) {
        ncurses_stop();
        perror("Erreur lors de l'écriture de la taille d'un string  ");
        exit(EXIT_FAILURE);
    }

    if (write(fd, str, taille) == -1) {
        ncurses_stop();
        perror("Erreur lors de l'écriture du string ");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet d'écrire un "unsigned short" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param us la variable qui sera écrite
 */
void write_us(int fd, unsigned short us){
    if (write(fd, &us, sizeof(unsigned short)) == -1) {
        ncurses_stop();
        perror("Erreur lors de l'écriture de l'unsigned short ");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet d'écrire un "int" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param i la variable qui sera écrite
 */
void write_int(int fd, int i){
    if (write(fd, &i, sizeof(int)) == -1) {
        ncurses_stop();
        perror("Erreur lors de l'écriture du int ");
        exit(EXIT_FAILURE);
    }
}


/**
 * @brief Permet d'écrire les "case_t *" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param cases la variable qui sera écrite
 */
void write_cases(int fd, case_t *cases){
    if (write(fd, cases, sizeof(case_t) * NB_COL * NB_LIGNE) == -1) {
        ncurses_stop();
        perror("Erreur lors de l'écriture du int ");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Permet d'écrire les "entity_t *" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param entity la variable qui sera écrite
 */
void write_entity(int fd, entity_t *entity){
    write_int(fd, entity->type);
    write_string(fd, entity->name);
    write_int(fd, entity->health);
    write_int(fd, entity->armor);
    write_int(fd, entity->strength);
    write_int(fd, entity->move_speed);
    write_int(fd, entity->hit_speed);
    write_int(fd, entity->coord_x);
    write_int(fd, entity->coord_y);
    write_int(fd, entity->index);
    write_int(fd, entity->can_attack);
}


/**
 * @brief Permet d'écrire un "map_t" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param map la variable qui sera écrite
 */
void write_map(int fd, map_t *map){
    write_string(fd, map->name);
    write_int(fd, map->pos_x);
    write_int(fd, map->pos_y);
   
    /**
    if(map->top != NULL){
        write_map(fd, map->top);
    } else {
        write_string(fd, "null");
    }

    if(map->bottom != NULL){
        write_map(fd, map->bottom);
    } else {
        write_string(fd, "null");
    }
    
    if(map->left != NULL){
        write_map(fd, map->left);
    } else {
        write_string(fd, "null");
    }
    
    if(map->right != NULL){
        write_map(fd, map->right);
    } else {
        write_string(fd, "null");
    }
    **/
    write_us(fd, map->height);
    write_us(fd, map->width);
    write_us(fd, map->nb_entity);
    if(map->nb_entity != 0){
        if (map->nb_entity == 1){
            write_entity(fd, &map->entities[0]);
        } else {
            for (int i = 0; i < map->nb_entity; i++){
                write_entity(fd, &map->entities[i]);
            }
        }
    }

    write_cases(fd, map->cases);
}

/**
 * @brief Permet d'écrire un "hero_t" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param hero la variable qui sera écrite
 */
void write_hero(int fd, hero_t *hero) {
    write_int(fd, hero->map_x);
    write_int(fd, hero->map_y);
    write_int(fd, hero->x_spawn);
    write_int(fd, hero->y_spawn);
    write_int(fd, hero->coord_x);
    write_int(fd, hero->coord_y);
    write_int(fd, hero->max_health);
    write_int(fd, hero->health);
    write_int(fd, hero->armor);
    write_int(fd, hero->strength);
    write_int(fd, hero->move_speed);
    write_int(fd, hero->hit_speed);
    write_int(fd, hero->xp);
    write_int(fd, hero->nb_piece);
}

/**
 * @brief Permet d'écrire une " query_t" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui contiendra ce qui a été lu
 */
void write_query(int fd, query_t *q){
    check_null(q);
    int data_type;

    write_string(fd, q->message);   

    write_int(fd, q->client_src);
    data_type = q->data_type;
    write_int(fd, q->data_type);
    if(data_type == INT_TYPE || data_type == INPUT_TYPE){
        write_int(fd, *(int*)q->data);
    } else if (data_type == STR_TYPE) {
        write_string(fd, (char *)q->data);
    } else if (data_type == US_TYPE) {
        write_us(fd, *(unsigned short*)q->data);
    } else if (data_type == MAP_TYPE) {
        write_map(fd, (map_t *)q->data);
    } else if (data_type == HERO_TYPE) {
        write_hero(fd, (hero_t *)q->data);
    }
}

/**
 * @brief Permet d'écrire une "response_t" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui contiendra ce qui a été lu
 */
void write_response(int fd, response_t *r){
    check_null(r);
    int data_type;

    write_string(fd, r->message);   
    write_int(fd, r->client_target);
    data_type = r->data_type;
    write_int(fd, r->data_type);
    if(data_type == INT_TYPE || data_type == INPUT_TYPE){
        write_int(fd, *(int*)r->data);
    } else if (data_type == STR_TYPE) {
        write_string(fd, (char *)r->data);
    } else if (data_type == US_TYPE) {
        write_us(fd, *(unsigned short*)r->data);
    } else if (data_type == MAP_TYPE) {
        write_map(fd, (map_t *)r->data);
    } else if (data_type == HERO_TYPE) {
        write_hero(fd, (hero_t *)r->data);
    }
}



/**
 * @brief Permet de lire un "char *" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui contiendra ce qui a été lu
 */
void read_string(int fd, char **str){
    size_t taille;
    if (read(fd, &taille, sizeof(size_t)) == -1) {
        /**
        ncurses_stop();
        perror("Erreur lors de la lecture de la taille du string ");
        exit(EXIT_FAILURE);
        */
    }

    if ((*str = malloc(taille * sizeof(char))) == NULL) {
        /**
        ncurses_stop();
        perror("Erreur lors de l'allocation du string ");
        exit(EXIT_FAILURE);
        */
    }

    if (read(fd, *str, taille) == -1) {
        /**
        ncurses_stop();
        perror("Erreur lors de la lecture du string ");
        exit(EXIT_FAILURE);
        */
    }
}

/**
 * @brief Permet de lire un "unsigned short" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param us la variable qui contiendra ce qui a été lu
 */
void read_us(int fd, unsigned short *us){
    if ((read(fd, us, sizeof(unsigned short))) == -1){
        /**
        ncurses_stop();
        perror("Erreur lors de l'unsigned short ");
        exit(EXIT_FAILURE);
        */
    }
}
/**
 * @brief Permet de lire un "int" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param i la variable qui contiendra ce qui a été lu
 */
void read_int(int fd, int *i){
    if ((read(fd, i, sizeof(int))) == -1){
        /**
        ncurses_stop();
        perror("Erreur lors de la lecture du int ");
        exit(EXIT_FAILURE);
        */
    }
}

/**
 * @brief Permet de lire les "case_t *" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param cases la variable qui contiendra ce qui a été lu
 */
void read_cases(int fd, case_t *cases){       
    if ((read(fd, cases, sizeof(case_t) * NB_COL * NB_LIGNE)) == -1){
        /**
        ncurses_stop();
        perror("Erreur lors de la lecture des cases ");
        exit(EXIT_FAILURE);
        */
    }
}

/**
 * @brief Permet de lire une "entity_t *" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param entity la variable qui contiendra ce qui a été lu
 */
void read_entity(int fd, entity_t *entity){
    read_int(fd, &entity->type);
    read_string(fd, &entity->name);
    read_int(fd, &entity->health);
    read_int(fd, &entity->armor);
    read_int(fd, &entity->strength);
    read_int(fd, &entity->move_speed);
    read_int(fd, &entity->hit_speed);
    read_int(fd, &entity->coord_x);
    read_int(fd, &entity->coord_y);
    read_int(fd, &entity->index);
    read_int(fd, &entity->index);
}


/**
 * @brief Permet de lire un " hero_t *" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param entity la variable qui contiendra ce qui a été lu
 */
void read_hero(int fd, hero_t *hero) {
    read_int(fd, &hero->map_x);
    read_int(fd, &hero->map_y);
    read_int(fd, &hero->x_spawn);
    read_int(fd, &hero->y_spawn);
    read_int(fd, &hero->coord_x);
    read_int(fd, &hero->coord_y);
    read_int(fd, &hero->max_health);
    read_int(fd, &hero->health);
    read_int(fd, &hero->armor);
    read_int(fd, &hero->strength);
    read_int(fd, &hero->move_speed);
    read_int(fd, &hero->hit_speed);
    read_int(fd, &hero->xp);
    read_int(fd, &hero->nb_piece);
}


/**
 * @brief Permet de lire une "map_t" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param map la variable qui contiendra ce qui a été lu
 */
void read_map(int fd, map_t *map){
    read_string(fd, &map->name);
    
    read_int(fd, &map->pos_x);
    read_int(fd, &map->pos_y);

    read_us(fd, &(map->height));
    read_us(fd, &(map->width));
   
    read_us(fd, &(map->nb_entity));
    if(map->nb_entity != 0){
        map->entities = (entity_t*) malloc_check(sizeof(entity_t) * map->nb_entity);
        for (int i = 0; i < map->nb_entity; i++){
            entity_t *entity = (entity_t*) malloc_check(sizeof(entity_t));
            read_entity(fd, entity);
            map->entities[i] = *entity;
            }
        } 
        map->cases = (case_t*) malloc_check(sizeof(case_t) * NB_COL * NB_LIGNE);
        read_cases(fd, map->cases);
}


/**
 * @brief Permet de lire une "query_t" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui contiendra ce qui a été lu
 */
void read_query(int fd, query_t *q){
    check_null(q);
    int data_type;

    read_string(fd, &q->message);   
   
    read_int(fd, &q->client_src);
    read_int(fd, &q->data_type);
    data_type = q->data_type;

    if(data_type == INT_TYPE || data_type == INPUT_TYPE){
        read_int(fd, (int*) q->data);
    } else if (data_type == STR_TYPE) {
        read_string(fd, q->data);
    } else if (data_type == US_TYPE) {
        read_us(fd, (unsigned short*)q->data);
    } else if (data_type == MAP_TYPE) {
        map_t *map = (map_t *) malloc_check(sizeof(map_t));
        read_map(fd, map);
        q->data = map;
    } else if (data_type == HERO_TYPE) {
        hero_t *hero = (hero_t *) malloc_check(sizeof(hero_t));
        read_hero(fd, hero);
        q->data = hero;
    }
}

/**
 * @brief Permet de lire une "response_t" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui contiendra ce qui a été lu
 */
void read_response(int fd, response_t *r){
    if(r == NULL){
        r = (response_t*) malloc_check(sizeof(response_t));
    }
    int data_type;

    read_string(fd, &r->message);   
    read_int(fd, &r->client_target);
    read_int(fd, &r->data_type);
    data_type = r->data_type;

    if(data_type == INT_TYPE || data_type == INPUT_TYPE){
        read_int(fd, (int*) r->data);
    } else if (data_type == STR_TYPE) {
        read_string(fd, r->data);
    } else if (data_type == US_TYPE) {
        read_us(fd, (unsigned short*)r->data);
    } else if (data_type == MAP_TYPE) {
        map_t *map = (map_t *) malloc_check(sizeof(map_t));
        read_map(fd, map);
        r->data = map;
    } else if (data_type == HERO_TYPE) {
        hero_t *hero = (hero_t *) malloc_check(sizeof(hero_t));
        read_hero(fd, hero);
        r->data = hero;
    }
}
