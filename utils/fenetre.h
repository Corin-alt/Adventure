/**
 * @file fenetre.h
 * @author Cyril Rabat
 * @brief Prototype des fonctions lièes à la structure fenetre_t
 */

#ifndef _FENETRE_
#define _FENETRE_

#include <ncurses.h>

// Structure représentant une fenêtre avec un cadre et un intérieur
typedef struct {
    int posx, posy; // La position de la fenêtre sur l'écran
    int largeur, hauteur; // La hauteur et la largeur de la fenêtre
    WINDOW * cadre; // Le cadre de la fenêtre
    WINDOW * interieur; // L'intérieur de la fenêtre
}
fenetre_t;

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
fenetre_t * fenetre_creer(int posX, int posY, int largeur, int hauteur, char * titre, bool scroll);

/**
 * Supprime une fenêtre.
 * @param[in,out] fenetre un pointeur sur la fenêtre à supprimer
 */
void fenetre_supprimer(fenetre_t ** fenetre);

/**
 * Vérifie si des coordonnées globales se trouvent dans une fenêtre.
 * @param[in] fenetre la fenêtre
 * @param[in] x coordonnée en x
 * @param[in] y coordonnée en y
 * @return TRUE si oui, FALSE sinon
 */
bool fenetre_estdans(fenetre_t * fenetre, int x, int y);

/**
 * Recupère depuis des coordonnées globales, les coordonnées au sein d'une fenêtre.
 * @param[in] fenetre la fenêtre
 * @param[in] posx coordonnée globale x
 * @param[in] poxy coordonnée globale y
 * @param[out] x coordonnée calculée en x dans la fenêtre
 * @param[out] y coordonnée calculée en y dans la fenêtre
 * @return TRUE si les coordonnees sont dans la fenetre, FALSE sinon
 */
bool fenetre_getcoordonnees(fenetre_t * fenetre, int posx, int posy, int * x, int * y);

/**
 * Affiche un caractère dans une fenêtre.
 * @param[in,out] fenetre la fenêtre
 * @param[in] c le caractère
 */
int fenetre_addch(fenetre_t * fenetre, chtype c);

/**
 * Affiche un caractère dans une fenêtre à une position donnée.
 * @param[in,out] fenetre la fenêtre
 * @param[in] posY la position en Y
 * @param[in] posX la position en X
 * @param[in] c le caractère
 */
int fenetre_mvaddch(fenetre_t * fenetre, int posY, int posX, chtype c);

/**
 * Affiche un caractère dans une fenêtre dans une couleur donnée.
 * @param[in,out] fenetre la fenêtre
 * @param[in] c le caractère
 */
int fenetre_addch_col(fenetre_t * fenetre, unsigned int couleur, chtype c);

/**
 * Affiche un caractère dans une fenêtre à une position et dans une couleur données.
 * @param[in,out] fenetre la fenêtre
 * @param[in] posY la position en Y
 * @param[in] posX la position en X
 * @param[in] couleur la couleur
 * @param[in] c le caractère
 */
int fenetre_mvaddch_col(fenetre_t * fenetre, int posY, int posX, unsigned int couleur, chtype c);

/**
 * Affiche une chaîne de caractères dans une fenêtre.
 * @param[in,out] fenetre la fenêtre
 * @param[in] fmt le format
 */
int fenetre_printw(fenetre_t * fenetre,
    const char * fmt, ...);

/**
 * Affiche une chaîne de caractères dans une fenêtre dans une couleur donnée.
 * @param[in,out] fenetre la fenêtre
 * @param[in] couleur la couleur
 * @param[in] fmt le format
 */
int fenetre_printw_col(fenetre_t * fenetre, unsigned int couleur,
    const char * fmt, ...);

/**
 * Affiche une chaîne de caractères dans une fenêtre à une position donnée.
 * @param[in,out] fenetre la fenêtre
 * @param[in] posY la position en X
 * @param[in] posX la position en Y
 * @param[in] fmt le format
 */
int fenetre_mvprintw(fenetre_t * fenetre, int posY, int posX,
    const char * fmt, ...);

/**
 * Affiche une chaîne de caractères dans une fenêtre à une position et dans une couleur données.
 * @param[in,out] fenetre la fenêtre
 * @param[in] posY la position en X
 * @param[in] posX la position en Y
 * @param[in] couleur la couleur
 * @param[in] fmt le format
 */
int fenetre_mvprintw_col(fenetre_t * fenetre, int posY, int posX, unsigned int couleur,
    const char * fmt, ...);

/**
 * Raffraichit une fenêtre.
 * @param[in, out] fenetre la fenêtre
 */
int fenetre_refresh(fenetre_t * fenetre);

/**
 * Change la couleur du texte.
 * @param[in, out] fenetre la fenêtre
 * @param[in] couleur la couleur
 */
int fenetre_couleur(fenetre_t * fenetre, unsigned int couleur);

/**
 * Vide la fenêtre
 * @param[in, out] fenetre la fenêtre
 */
int fenetre_erase(fenetre_t * fenetre);

#endif