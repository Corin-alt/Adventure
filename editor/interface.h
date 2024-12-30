/**
 * @file interface.h
 * @author Cyril Rabat
 * @brief Implémentation des fonctions lièes à la structure interface_t
 */

#ifndef _INTERFACE_
#define _INTERFACE_

#include "../utils/fenetre.h"
#include "../elements/map.h"

// Structure représentant l'interface de l'application
typedef struct {
    fenetre_t * win_infos; // La fenêtre d'informations
    fenetre_t * win_palette; // La fenêtre de la palette
    fenetre_t * win_map; // La fenêtre de la map de jeu
    fenetre_t * win_key; // La fenêtre du guide des touches
    fenetre_t * win_fiche; // La fenêtre de la fiche technique d'un élément
    map_t * map; //la map de jeu
    unsigned int selection; // La couleur sélectionnée
    char objet;
}
interface_t;

/**
 * Définition de la palette.
 */
void palette();

/**
 * Vérification des dimensions du terminal.
 * @param[in] largeur la largeur de l'image
 * @param[out] hauteur la hauteur de l'image
 */
void interface_dimensions(unsigned short largeur, unsigned short hauteur);

/**
 * Création d'une interface.
 * @param[in] image l'image
 * @return l'interface créée
 */
interface_t * interface_creer(map_t * map);

/**
 * Suppression d'une interface.
 * @param[in,out] interface l'interface à supprimer
 */
void interface_supprimer(interface_t ** interface);

/**
 * Met à jour la fenêtre 'palette'.
 * @param[in,out] interface l'interface
 */
void interface_MAJPalette(interface_t * interface);

/**
 * Gestion des actions dans la fenêtre palette.
 * @param[in,out] interface l'interface
 * @param[in] posX la position X du clic dans la fenêtre
 * @param[in] posY la position Y du clic dans la fenêtre
 */
void interface_palette(interface_t * interface, int posX, int posY);

/**
 * Gestion des actions dans la fenêtre image.
 * @param[in,out] interface l'interface
 * @param[in] posX la position X du clic dans la fenêtre
 * @param[in] posY la position Y du clic dans la fenêtre
 */
void interface_map(interface_t * interface, int posX, int posY);

/**
 * Gestion des actions de l'utilisateur dans l'interface.
 * @param[in,out] interface l'interface
 * @param[in] c la touche pressée
 */
void interface_main(interface_t * interface, int c);

#endif