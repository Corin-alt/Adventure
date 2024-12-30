/**
 * @file files_utils.h
 * @brief Protoypes des fonctions nécessaire aux fichiers
 * @author Dupont Corentin & Lacroix Owen
 * @date 2022-03-14
 */

#ifndef H_FILES_UTILS
#define H_FILES_UTILS

#include "../elements/map.h"
#include "../elements/entity.h"
#include "../elements/hero.h"
#include "include.h"

/**
 * @brief Permet d'écrire un "char *" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui sera écrite
 */
void write_string(int fd, char *str);

/**
 * @brief Permet d'écrire un "unsigned short" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param us la variable qui sera écrite
 */
void write_us(int fd, unsigned short us);

/**
 * @brief Permet d'écrire un "int" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param i la variable qui sera écrite
 */
void write_int(int fd, int i);

/**
 * @brief Permet d'écrire un "hero_t" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param hero la variable qui sera écrite
 */
void write_hero(int fd, hero_t *hero);

/**
 * @brief Permet d'écrire les "case_t *" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param cases la variable qui sera écrite
 */
void write_cases(int fd, case_t *cases);

/**
 * @brief Permet d'écrire les "entity_t *" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param entity la variable qui sera écrite
 */
void write_entity(int fd, entity_t *entity);

/**
 * @brief Permet d'écrire un "map_t" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param map la variable qui sera écrite
 */
void write_map(int fd, map_t *map);

/**
 * @brief Permet de lire un "char *" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui contiendra ce qui a été lu
 */
void read_string(int fd, char **str);

/**
 * @brief Permet d'écrire une " query_t" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui contiendra ce qui a été lu
 */
void write_query(int fd, query_t *q);

/**
 * @brief Permet d'écrire une "response_t" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui contiendra ce qui a été lu
 */
void write_response(int fd, response_t *r);


/**
 * @brief Permet de lire un "unsigned short" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param us la variable qui contiendra ce qui a été lu
 */
void read_us(int fd, unsigned short *us);

/**
 * @brief Permet de lire un "int" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param i la variable qui contiendra ce qui a été lu
 */
void read_int(int fd, int *i);

/**
 * @brief Permet de lire une "entity_t *" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param entity la variable qui contiendra ce qui a été lu
 */
void read_entity(int fd, entity_t *entity);

/**
 * @brief Permet de lire un " hero_t *" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param entity la variable qui contiendra ce qui a été lu
 */
void read_hero(int fd, hero_t *hero);

/**
 * @brief Permet de lire les "case_t *" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param cases la variable qui contiendra ce qui a été lu
 */
void read_cases(int fd, case_t *cases);

/**
 * @brief Permet de lire une "map_t" dans un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param map la variable qui contiendra ce qui a été lu
 */
void read_map(int fd, map_t *map);

/**
 * @brief Permet de lire une "query_t" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui contiendra ce qui a été lu
 */
void read_query(int fd, query_t *q);

/**
 * @brief Permet de lire une "response_t" depuis un descripteur de fichier
 * 
 * @param fd le descripteur de fichier
 * @param str la variable qui contiendra ce qui a été lu
 */
void read_response(int fd, response_t *r);

#endif