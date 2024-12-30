/**
 * @file interface.h
 * @author Cyril Rabat
 * @brief Implémentation des fonctions lièes à la structure interface_t
 */

#ifndef _INTERFACE_
#define _INTERFACE_

#include "../utils/fenetre.h"
#include "../elements/map.h"
#include "../elements/hero.h"

// Structure représentant l'interface de l'application
typedef struct {
    fenetre_t * win_infos; // La fenêtre d'informations
    fenetre_t * win_map; // La fenêtre de la map de jeu
    fenetre_t * win_hero; // La fenêtre de la fiche technique d'un élément
    map_t * map; //la map de jeu
    hero_t *hero; //le hero
}interface_t;

/**
 * Définition de la palette.
 */
void game_palette();

/**
 * Vérification des dimensions du terminal.
 * @param[in] largeur la largeur de l'image
 * @param[out] hauteur la hauteur de l'image
 */
void game_interface_dimensions(unsigned short largeur, unsigned short hauteur);

/**
 * Création d'une interface.
 * @param[in] image l'image
 * @return l'interface créée
 */
interface_t * game_interface_creer(map_t * map, hero_t *hero);

/**
 * @brief Permet de rafaichier l'affichage de la map
 * 
 * @param interface l'interface
 * @param map la map
 */
void refresh_map_in_interface(interface_t * interface, map_t *map);

/**
 * Suppression d'une interface.
 * @param[in,out] interface l'interface à supprimer
 */
void game_interface_supprimer(interface_t ** interface);

/**
 * Gestion des actions dans la fenêtre image.
 * @param[in,out] interface l'interface
 */
void game_interface_map(interface_t * interface, int ch);

/**
 * Gestion des actions de l'utilisateur dans l'interface.
 * @param[in,out] interface l'interface
 * @param[in] c la touche pressée
 */
void game_interface_main(interface_t * interface, int c);

#endif