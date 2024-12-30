/**
 * @file fenetre.c
 * @author Cyril Rabat
 * @brief Implémentation des fonctions lièes à la structure fenetre_t
 */

#include <stdlib.h>
#include <curses.h>
#include "fenetre.h"

/**
 * Crée une nouvelle fenêtre.
 * @param[in] posX la position en X de la fenêtre
 * @param[in] posY la position en Y de la fenêtre
 * @param[in] largeur la largeur de la fenêtre
 * @param[in] hauteur la heuteur de la fenêtre
 * @param[in] titre le titre de la fenêtre
 * @param[in] scroll si 'TRUE', active le défilement dans la fenêtre
 * @return la fenêtre créée
 */
fenetre_t * fenetre_creer(int posX, int posY, int largeur, int hauteur, char * titre, bool scroll) {
    fenetre_t * retour;

    // Allocation de la structure
    if ((retour = malloc(sizeof(fenetre_t))) == NULL) {
        fprintf(stderr, "Erreur lors de l'allocation de la structure de fenêtre.\n");
        exit(EXIT_FAILURE);
    }

    // Mémorisation des informations générales
    retour -> posx = posX;
    retour -> posy = posY;
    retour -> hauteur = hauteur;
    retour -> largeur = largeur;

    // Création de la fenêtre principale avec le cadre
    retour -> cadre = newwin(hauteur, largeur, posY, posX);
    box(retour -> cadre, 0, 0);
    wprintw(retour -> cadre, titre);
    wrefresh(retour -> cadre);

    // Création de la fenêtre intérieure
    retour -> interieur = subwin(retour -> cadre, hauteur - 2, largeur - 2, posY + 1, posX + 1);
    scrollok(retour -> interieur, scroll);
    wrefresh(retour -> interieur);

    return retour;
}

/**
 * Supprime une fenêtre.
 * @param[in,out] fenetre un pointeur sur la fenêtre à supprimer
 */
void fenetre_supprimer(fenetre_t ** fenetre) {
    delwin(( * fenetre) -> interieur);
    delwin(( * fenetre) -> cadre);
    free( * fenetre);
    * fenetre = NULL;
}

/**
 * Vérifie si des coordonnées globales se trouvent dans une fenêtre.
 * @param[in] fenetre la fenêtre
 * @param[in] x coordonnée en x
 * @param[in] y coordonnée en y
 * @return TRUE si oui, FALSE sinon
 */
bool fenetre_estdans(fenetre_t * fenetre, int x, int y) {
    bool resultat = FALSE;

    if (((x >= fenetre -> posx + 1) && (x < fenetre -> largeur + fenetre -> posx - 1)) &&
        ((y >= fenetre -> posy + 1) && (y < fenetre -> hauteur + fenetre -> posy - 1)))
        resultat = TRUE;

    return resultat;
}

/**
 * Recupère depuis des coordonnées globales, les coordonnées au sein d'une fenêtre.
 * @param[in] fenetre la fenêtre
 * @param[in] posx coordonnée globale x
 * @param[in] poxy coordonnée globale y
 * @param[out] x coordonnée calculée en x dans la fenêtre
 * @param[out] y coordonnée calculée en y dans la fenêtre
 * @return TRUE si les coordonnees sont dans la fenetre, FALSE sinon
 */
bool fenetre_getcoordonnees(fenetre_t * fenetre, int posx, int posy, int * x, int * y) {
    bool resultat = FALSE;

    if (((posx >= fenetre -> posx + 1) && (posx < fenetre -> largeur + fenetre -> posx - 1)) &&
        ((posy >= fenetre -> posy + 1) && (posy < fenetre -> hauteur + fenetre -> posy - 1))) {
        resultat = TRUE;
        * x = posx - fenetre -> posx - 1;
        * y = posy - fenetre -> posy - 1;
    }

    return resultat;
}

/**
 * Affiche un caractère dans une fenêtre.
 * @param[in,out] fenetre la fenêtre
 * @param[in] c le caractère
 */
int fenetre_addch(fenetre_t * fenetre, chtype c) {
    return waddch(fenetre -> interieur, c);
}

/**
 * Affiche un caractère dans une fenêtre à une position donnée.
 * @param[in,out] fenetre la fenêtre
 * @param[in] posY la position en Y
 * @param[in] posX la position en X
 * @param[in] c le caractère
 */
int fenetre_mvaddch(fenetre_t * fenetre, int posY, int posX, chtype c) {
    wmove(fenetre -> interieur, posY, posX);

    return waddch(fenetre -> interieur, c);
}

/**
 * Affiche un caractère dans une fenêtre dans une couleur donnée.
 * @param[in,out] fenetre la fenêtre
 * @param[in] couleur la couleur
 * @param[in] c le caractère
 */
int fenetre_addch_col(fenetre_t * fenetre, unsigned int couleur, chtype c) {
    int retour;

    wattron(fenetre -> interieur, COLOR_PAIR(couleur));
    retour = waddch(fenetre -> interieur, c);
    wattroff(fenetre -> interieur, COLOR_PAIR(couleur));

    return retour;
}

/**
 * Affiche un caractère dans une fenêtre à une position et dans une couleur données.
 * @param[in,out] fenetre la fenêtre
 * @param[in] posY la position en Y
 * @param[in] posX la position en X
 * @param[in] couleur la couleur
 * @param[in] c le caractère
 */
int fenetre_mvaddch_col(fenetre_t * fenetre, int posY, int posX, unsigned int couleur, chtype c) {
    int retour;

    wattron(fenetre -> interieur, COLOR_PAIR(couleur));
    wmove(fenetre -> interieur, posY, posX);
    retour = waddch(fenetre -> interieur, c);
    wattroff(fenetre -> interieur, COLOR_PAIR(couleur));

    return retour;
}

/**
 * Affiche une chaîne de caractères dans une fenêtre.
 * @param[in,out] fenetre la fenêtre
 * @param[in] fmt le format
 */
int fenetre_printw(fenetre_t * fenetre,
    const char * fmt, ...) {
    va_list lstPar;
    int retour;

    va_start(lstPar, fmt);
    retour = vw_printw(fenetre -> interieur, fmt, lstPar);
    va_end(lstPar);

    return retour;
}

/**
 * Affiche une chaîne de caractères dans une fenêtre dans une couleur donnée.
 * @param[in,out] fenetre la fenêtre
 * @param[in] couleur la couleur
 * @param[in] fmt le format
 */
int fenetre_printw_col(fenetre_t * fenetre, unsigned int couleur,
    const char * fmt, ...) {
    va_list lstPar;
    int retour;

    va_start(lstPar, fmt);
    wattron(fenetre -> interieur, COLOR_PAIR(couleur));
    retour = vw_printw(fenetre -> interieur, fmt, lstPar);
    wattroff(fenetre -> interieur, COLOR_PAIR(couleur));
    va_end(lstPar);

    return retour;
}

/**
 * Affiche une chaîne de caractères dans une fenêtre à une position donnée.
 * @param[in,out] fenetre la fenêtre
 * @param[in] posY la position en X
 * @param[in] posX la position en Y
 * @param[in] fmt le format
 */
int fenetre_mvprintw(fenetre_t * fenetre, int posY, int posX,
    const char * fmt, ...) {
    va_list lstPar;
    int retour;

    va_start(lstPar, fmt);
    wmove(fenetre -> interieur, posY, posX);
    retour = vw_printw(fenetre -> interieur, fmt, lstPar);
    va_end(lstPar);

    return retour;
}

/**
 * Affiche une chaîne de caractères dans une fenêtre à une position et dans une couleur données.
 * @param[in,out] fenetre la fenêtre
 * @param[in] couleur la couleur 
 * @param[in] posY la position en X
 * @param[in] posX la position en Y
 * @param[in] fmt le format
 */
int fenetre_mvprintw_col(fenetre_t * fenetre, int posY, int posX, unsigned int couleur,
    const char * fmt, ...) {
    va_list lstPar;
    int retour;

    va_start(lstPar, fmt);
    wmove(fenetre -> interieur, posY, posX);
    wattron(fenetre -> interieur, COLOR_PAIR(couleur));
    retour = vw_printw(fenetre -> interieur, fmt, lstPar);
    wattroff(fenetre -> interieur, COLOR_PAIR(couleur));
    va_end(lstPar);

    return retour;
}

/**
 * Raffraichit une fenêtre.
 * @param[in, out] fenetre la fenêtre
 */
int fenetre_refresh(fenetre_t * fenetre) {
    return wrefresh(fenetre -> interieur);
}

/**
 * Change la couleur du texte.
 * @param[in, out] fenetre la fenêtre
 * @param[in] couleur la couleur
 */
int fenetre_couleur(fenetre_t * fenetre, unsigned int couleur) {
    return wattron(fenetre -> interieur, COLOR_PAIR(couleur));
}

/**
 * Vide la fenêtre
 * @param[in, out] fenetre la fenêtre
 */
int fenetre_erase(fenetre_t * fenetre) {
    return werase(fenetre -> interieur);
}