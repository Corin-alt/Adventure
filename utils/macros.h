/**
 * @file macros.h
 * @author Dupont Corentin & Lacroix Owen
 * @brief Liste de macros (constantes) utile 
 * @date 2022-02-19
 */

#ifndef _MACROS_
#define _MACROS_


#define SPAWN_NAME_MAP "carte1.bin"
#define PATH_LOCATION_MAPS_FILES "../maps/"

#define NB_LIGNE 20
#define NB_COL 40

#define WHITE       1
#define GREEN       2
#define BLUE        3
#define RED         4 
#define YELLOW      5
#define CYAN        6
#define MAGENTA     7
#define FD_WHITE    8
#define FD_GREEN    9
#define FD_BLUE    10
#define FD_RED     11
#define FD_YELLOW  12
#define FD_CYAN    13
#define FD_MAGENTA 14

#define HERBE 9 //ref à la color pallette FD_GREEN
#define CAILLOU 11//ref à la color pallette FD_RED
#define SABLE 12 //ref à la color pallette FD_YELLOW
#define EAU 10 //ref à la color pallette  FD_CYAN

#define MONSTER_TYPE 0
#define ARTEFACT_TYPE 1

#define SYMB_HERO 'H'
#define SYMB_MONSTRE 'M'
#define SYMB_TRESOR '$'
#define SYMB_ARTEFACT 'A'
#define SYMB_OBSTACLE 'X'
#define SYMB_OUTIL_SELECT '@'
#define SYMB_EMPTY_CASE ' '

#define CODE_ASCII_EMPTY_CASE 32
#define CODE_ASCII_TRESOR 36
#define CODE_ASCII_ARTEFACT 65
#define CODE_ASCII_HERO 72
#define CODE_ASCII_MONSTRE 77
#define CODE_ASCII_OBSTACLE 88

#define KEY_SPACE 32
#define KEY_Q 81
#define KEY_q 113

#define TOP 0
#define BOTTOM 1
#define RIGHT 2
#define LEFT 3

#define MAX_RECUPERATION_LIFE 3 // 3 -> un tier de sa vie max , 4 -> un quart de sa vie max etc...
#define PERCENTAGE_LIFE 85
#define PERCENTAGE_PIECE 15

#define XP_TO_PASS_LEVEL 50
#define INVENTORY_SIZE 5
#define MAX_NB_PIECE 3

#define DEFAULT_HERO_HEALTH 100
#define DEFAULT_HERO_ARMOR 4
#define DEFAULT_HERO_STRENGH 2
#define DEFAULT_HERO_MOVE_SPEED 1
#define DEFAULT_HERO_HIT_SPEED 3
#define DEFAULT_HERO_XP 0
#define DEFAULT_HERO_NB_PIECE 0
#define DEFAULT_HERO_NB_ART 0


#define FLAG_SUCCESS_MESSAGE 0
#define FLAG_ERROR_MESSAGE 1
#define FLAG_WARNING_MESSAGE 2
#define FLAG_TAKE_ARTEFACT 3

#define ALARM_SECONDS_GRAND_TOUT 60


#define MAXIMUM_MOVE_SPEED_IN_SEC  1.0
#define MINIMUM_MOVE_SPEED_IN_SEC  0.4
#define MOVE_SPEED_BY_POINT_IN_SEC 0.1



#endif