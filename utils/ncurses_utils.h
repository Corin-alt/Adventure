/**
 * @file ncurses_utils.h
 * @author Cyril Rabat
 * Modifié par Dupont Corentin & Lacroix Owen
 * @brief Prototype des fonctions utiles
 */

#ifndef H_NCURSES_UTILS
#define H_NCURSES_UTILS

#include <ncurses.h>

/**
 * Initialisation de ncurses.
 */
void ncurses_init();

void ncurses_game_init();

/**
 * Fin de ncurses.
 */
void ncurses_stop();

/**
 * Initialisation des couleurs.
 */
void ncurses_couleurs();

/**
 * Initialisation de la souris.
 */
void ncurses_initsouris();

/**
 * Récupération de la position de la souris.
 * @param[out] x la position en x de la souris
 * @param[out] y la position en y de la souris
 * @return OK ou ERR en cas d'erreur 
 */
int souris_getpos(int * x, int * y);

/**
 * Récupére la couleur à la postion cliqué
 * 
 * @param [in] win la windows
 * @param [in] posX la position x du window
 * @param [in] posY la position y du window
 * @return int l'id de la pallette de couleur
 */
int get_color_clicked(WINDOW * win, int posX, int posY);

/**
 * Récupére le code ascii du symbole à la postion cliqué
 * 
 * @param [in] win la windows
 * @param [in] posX la position x du window
 * @param [in] posY la position y du window
 * @return int le code ascii du symbole
 */
int get_ascci_code_clicked(WINDOW * win, int posX, int posY);

/**
 * Récupére le symbole à la postion cliqué
 * 
 * @param [in] win la windows
 * @param [in] posX la position x du window
 * @param [in] posY la position y du window
 * @return char le symbole
 */
char get_symbol_clicked(WINDOW * win, int posX, int posY);

#endif