/**
 * @file fonction.c
 * @author Cyril Rabat
 * Modifié par Dupont Corentin & Lacroix Owen
 * @brief Implémentation des fonctions utiles
 */

#include <string.h>
#include <unistd.h>
#include <ncurses.h>   // Pour toutes les fonctions/constantes ncurses
#include <stdlib.h> 
#include "ncurses_utils.h"
#include "../elements/entity.h"     // Pour exit, EXIT_FAILURE
#include "../elements/map.h"   

/**
 * Initialisation de ncurses.
 */
void ncurses_init() {
    initscr(); // Démarre le mode ncurses
    cbreak(); // Désactive la mise en buffer
    echo(); // active l'affichage des caractères saisis
    keypad(stdscr, TRUE); // Active les touches spécifiques (flèches)
    refresh(); // Met à jour l'affichage
    curs_set(FALSE); // Masque le curseur
}

void ncurses_game_init() {
    initscr(); // Démarre le mode ncurses
    cbreak(); // Désactive la mise en buffer
    noecho(); // desactive l'affichage des caractères saisis
    keypad(stdscr, TRUE); // Active les touches spécifiques (flèches)
    refresh(); // Met à jour l'affichage
    curs_set(FALSE); // Masque le curseur
}
/**
 * Fin de ncurses.
 */
void ncurses_stop() {
    endwin();
}

/**
 * Initialisation des couleurs.
 */
void ncurses_couleurs() {
    // Verification du support de la couleur
    if (has_colors() == FALSE) {
        ncurses_stop();
        fprintf(stderr, "Le terminal ne supporte pas les couleurs.\n");
        exit(EXIT_FAILURE);
    }

    // Activation des couleurs
    start_color();
}

/**
 * Initialisation de la souris.
 */
void ncurses_initsouris() {
    if (!mousemask(BUTTON1_PRESSED, NULL)) {
        ncurses_stop();
        fprintf(stderr, "Pas de gestion de la souris.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Récupération de la position de la souris.
 * @param[out] x la position en x de la souris
 * @param[out] y la position en y de la souris
 * @return OK ou ERR en cas d'erreur
 */
int souris_getpos(int * x, int * y) {
    MEVENT event;
    int resultat = getmouse( & event);
    if (resultat == OK) {
        * x = event.x;
        * y = event.y;
    }
    return resultat;
}

/**
 * Récupére la couleur à la postion cliqué
 * 
 * @param [in] win la windows
 * @param [in] posX la position x du window
 * @param [in] posY la position y du window
 * @return int l'id de la pallette de couleur
 */
int get_color_clicked(WINDOW * win, int posX, int posY) {
    return PAIR_NUMBER((mvwinch(win, posY, posX)) & A_COLOR);
}

/**
 * Récupére le code ascii du symbole à la postion cliqué
 * 
 * @param [in] win la windows
 * @param [in] posX la position x du window
 * @param [in] posY la position y du window
 * @return int le code ascii du symbole
 */
int get_ascci_code_clicked(WINDOW * win, int posX, int posY) {
    return mvwinch(win, posY, posX) & A_CHARTEXT;
}

/**
 * Récupére le symbole à la postion cliqué
 * 
 * @param [in] win la windows
 * @param [in] posX la position x du window
 * @param [in] posY la position y du window
 * @return char le symbole
 */
char get_symbol_clicked(WINDOW * win, int posX, int posY) {
    int code = get_ascci_code_clicked(win, posX, posY);
    if (code == CODE_ASCII_TRESOR) {
        return SYMB_TRESOR;
    } else if (code == CODE_ASCII_ARTEFACT) {
        return SYMB_ARTEFACT;
    } else if (code == CODE_ASCII_MONSTRE) {
        return SYMB_MONSTRE;
    } else if (code == CODE_ASCII_OBSTACLE) {
        return SYMB_OBSTACLE;
    } else {
        return SYMB_EMPTY_CASE;
    }
}

