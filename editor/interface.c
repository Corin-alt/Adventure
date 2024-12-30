/**
 * @file interface.c
 * @author Cyril Rabat
 * Modifié par Dupont Corentin & Lacroix Owen
 * @brief Implémentation des fonctions lièes à la structure interface_t
 */

#include <stdlib.h>
#include <string.h>
#include "../utils/macros.h"
#include "../utils/ncurses_utils.h"
#include "../elements/map.h"
#include "../elements/entity.h"
#include "interface.h"


/**
 *Définition de la palette.
 */
void palette() {
    init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(FD_WHITE, COLOR_BLACK, COLOR_WHITE);
    init_pair(FD_GREEN, COLOR_BLACK, COLOR_GREEN);
    init_pair(FD_BLUE, COLOR_BLACK, COLOR_BLUE);
    init_pair(FD_RED, COLOR_BLACK, COLOR_RED);
    init_pair(FD_YELLOW, COLOR_BLACK, COLOR_YELLOW);
    init_pair(FD_CYAN, COLOR_BLACK, COLOR_CYAN);
    init_pair(FD_MAGENTA, COLOR_BLACK, COLOR_MAGENTA);
}

/**
 *Vérification des dimensions du terminal.
 *@param[in] largeur la largeur de l'image
 *@param[out] hauteur la hauteur de l'image
 */
void interface_dimensions(unsigned short largeur, unsigned short hauteur) {
    if ((COLS < largeur + 17) || (LINES < hauteur + 12)) {
        ncurses_stop();
        fprintf(stderr,
            "Les dimensions du terminal sont insufisantes : l=%d,h=%d au lieu de l=%d,h=%d\n",
            COLS, LINES, largeur + 17, hauteur + 12);
        exit(EXIT_FAILURE);
    }
}

/**
 *Création d'une interface.
 *@param[in] map la map
 *@return l'interface créée
 */
interface_t * interface_creer(map_t * map) {
    interface_t * retour;

    // Vérification des dimensions
    interface_dimensions(map -> width, map -> height);

    // Allocation de la structure
    if ((retour = malloc(sizeof(interface_t))) == NULL) {
        ncurses_stop();
        perror("Erreur lors de la création de l'interface ");
        exit(EXIT_FAILURE);
    }

    retour -> map = map;

    // Création de la fenêtre d'information
    retour -> win_infos = fenetre_creer(0, 0, COLS, 10, "Informations", TRUE);
    fenetre_printw_col(retour -> win_infos, WHITE, "- Pour désinner et/ou poser un élément, sélectionnez le depuis la fenêtre \"Élements\" et cliquez à l'endroit souhaité sur la map.\n");
    fenetre_printw_col(retour -> win_infos, GREEN, "- Appuyez sur 'S' OU 's' pour sauvegarder la map.\n");
    fenetre_printw_col(retour -> win_infos, RED, "- Appuyez sur 'Q' OU 'q' pour quitter. (Une sauvegarde de la map est faite)\n");
    fenetre_refresh(retour -> win_infos);

    // Création de la fenêtre de la palette
    retour -> win_palette = fenetre_creer(map -> width + 2, 10, 18, 2 + map -> height, "Élements", FALSE);
    retour -> selection = FD_WHITE;
    interface_MAJPalette(retour);

    retour -> win_key = fenetre_creer(0, 32, 25, 4, "Touches", FALSE);
    fenetre_printw_col(retour -> win_key, GREEN, "S : Sauvegarder\n");
    fenetre_printw_col(retour -> win_key, RED, "Q : Quitter (Auto save)\n");
    fenetre_refresh(retour -> win_key);

    // Création de la fenêtre de la map
    retour -> win_map = fenetre_creer(0, 10, 2 + map -> width, 2 + map -> height, "map", FALSE);
    

    for (int x = 0; x < NB_LIGNE; x++) {
        for (int y = 0; y < NB_COL; y++) {
            case_t * ca = get_case(map, x, y);
            fenetre_mvaddch_col(retour -> win_map, x, y, ca -> biome, ca -> symbol); 
        }
    } 
  
    fenetre_refresh(retour -> win_map);

    retour -> win_fiche = fenetre_creer(60, 10, 30, 2 + map -> height, "Fiche", TRUE);
    fenetre_refresh(retour -> win_fiche);

    return retour;
}

/**
 *Suppression d'une interface.
 *@param[in,out] interface l'interface à supprimer
 */
void interface_supprimer(interface_t ** interface) {
    fenetre_supprimer( & ( * interface) -> win_infos);
    fenetre_supprimer( & ( * interface) -> win_palette);
    fenetre_supprimer( & ( * interface) -> win_map);
    fenetre_supprimer( & ( * interface) -> win_key);
    fenetre_supprimer( & ( * interface) -> win_fiche);
    free_map(&(*interface) -> map);
    free( * interface);
    interface = NULL;
}

/**
 *Met à jour la fenêtre 'palette'.
 *@param[in,out] interface l'interface
 */
void interface_MAJPalette(interface_t * interface) {
    fenetre_erase(interface -> win_palette);

    fenetre_printw_col(interface -> win_palette, FD_GREEN, " ");
    if (interface -> selection == FD_GREEN && interface -> objet == SYMB_EMPTY_CASE) {
        fenetre_couleur(interface -> win_palette, WHITE);
        fenetre_printw_col(interface -> win_palette, FD_WHITE, " Herbe\n");
    } else {
        fenetre_couleur(interface -> win_palette, RED);
        fenetre_printw_col(interface -> win_palette, GREEN, " Herbe\n");
    }

    fenetre_printw_col(interface -> win_palette, FD_RED, " ");
    if (interface -> selection == FD_RED && interface -> objet == SYMB_EMPTY_CASE) {
        fenetre_couleur(interface -> win_palette, WHITE);
        fenetre_printw_col(interface -> win_palette, FD_WHITE, " Caillou\n");
    } else {
        fenetre_couleur(interface -> win_palette, RED);
        fenetre_printw_col(interface -> win_palette, RED, " Caillou\n");
    }

    fenetre_printw_col(interface -> win_palette, FD_YELLOW, " ");
    if (interface -> selection == FD_YELLOW && interface -> objet == SYMB_EMPTY_CASE) {
        fenetre_couleur(interface -> win_palette, WHITE);
        fenetre_printw_col(interface -> win_palette, FD_WHITE, " Sable\n");
    } else {
        fenetre_couleur(interface -> win_palette, RED);
        fenetre_printw_col(interface -> win_palette, YELLOW, " Sable\n");
    }

    fenetre_printw_col(interface -> win_palette, FD_BLUE, " ");
    if (interface -> selection == FD_BLUE && interface -> objet == SYMB_EMPTY_CASE) {
        fenetre_printw_col(interface -> win_palette, FD_WHITE, " Eau\n");
        fenetre_couleur(interface -> win_palette, WHITE);
    } else {
        fenetre_couleur(interface -> win_palette, RED);
        fenetre_printw_col(interface -> win_palette, BLUE, " Eau\n");
    }

    fenetre_printw_col(interface -> win_palette, WHITE, "M");
    if (interface -> objet == SYMB_MONSTRE) {
        fenetre_couleur(interface -> win_palette, WHITE);
        fenetre_printw_col(interface -> win_palette, FD_WHITE, " Monstre\n");
    } else {
        fenetre_couleur(interface -> win_palette, RED);
        fenetre_printw_col(interface -> win_palette, WHITE, " Monstre\n");
    }

    fenetre_printw_col(interface -> win_palette, WHITE, "$");
    if (interface -> objet == SYMB_TRESOR) {
        fenetre_couleur(interface -> win_palette, WHITE);
        fenetre_printw_col(interface -> win_palette, FD_WHITE, " Trésor\n");
    } else {
        fenetre_couleur(interface -> win_palette, RED);
        fenetre_printw_col(interface -> win_palette, WHITE, " Trésor\n");

    }

    fenetre_printw_col(interface -> win_palette, WHITE, "A");
    if (interface -> objet == SYMB_ARTEFACT) {
        fenetre_couleur(interface -> win_palette, WHITE);
        fenetre_printw_col(interface -> win_palette, FD_WHITE, " Artefact\n");
    } else {
        fenetre_couleur(interface -> win_palette, RED);
        fenetre_printw_col(interface -> win_palette, WHITE, " Artefact\n");
    }

    fenetre_printw_col(interface -> win_palette, WHITE, "X");
    if (interface -> objet == SYMB_OBSTACLE) {
        fenetre_couleur(interface -> win_palette, WHITE);
        fenetre_printw_col(interface -> win_palette, FD_WHITE, " Obtsacle\n");
    } else {
        fenetre_couleur(interface -> win_palette, RED);
        fenetre_printw_col(interface -> win_palette, WHITE, " Obtsacle\n");
    }

    fenetre_printw_col(interface -> win_palette, WHITE, "@");
    if (interface -> objet == SYMB_OUTIL_SELECT) {
        fenetre_couleur(interface -> win_palette, WHITE);
        fenetre_printw_col(interface -> win_palette, FD_WHITE, " Outil Select\n");
    } else {
        fenetre_couleur(interface -> win_palette, RED);
        fenetre_printw_col(interface -> win_palette, WHITE, " Outil Select\n");
    }

    fenetre_refresh(interface -> win_palette);
}

/**
 *Gestion des actions dans la fenêtre palette.
 *@param[in,out] interface l'interface
 *@param[in] posX la position X du clic dans la fenêtre
 *@param[in] posY la position Y du clic dans la fenêtre
 */
void interface_palette(interface_t * interface, int posX, int posY) {
    interface -> objet = SYMB_EMPTY_CASE;
    interface -> selection = HERBE;

    if ((posY >= 0) && (posY <= 9)) {
        switch (posY) {
        case 0:
            interface -> selection = FD_GREEN;
            break;
        case 1:
            interface -> selection = FD_RED;
            break;
        case 2:
            interface -> selection = FD_YELLOW;
            break;
        case 3:
            interface -> selection = FD_BLUE;
            break;
        case 4:
            interface -> objet = SYMB_MONSTRE;
            break;
        case 5:
            interface -> objet = SYMB_TRESOR;
            break;
        case 6:
            interface -> objet = SYMB_ARTEFACT;
            break;
        case 7:
            interface -> objet = SYMB_OBSTACLE;
            break;
        case 8:
            interface -> objet = SYMB_OUTIL_SELECT;
            break;
        }

        interface_MAJPalette(interface);
        fenetre_refresh(interface -> win_infos);
    }
}

/**
 *Gestion des actions dans la fenêtre de la map.
 *@param[in,out] interface l'interface
 *@param[in] posX la position X du clic dans la fenêtre
 *@param[in] posY la position Y du clic dans la fenêtre
 */
void interface_map(interface_t * interface, int posX, int posY) {
    case_t *ca = get_case(interface -> map, posY, posX);

    if (interface -> objet == SYMB_OUTIL_SELECT) {
       entity_t entity = get_entity(interface->map, ca);
        if (ca -> symbol == SYMB_MONSTRE || ca -> symbol == SYMB_ARTEFACT) {
            print_fiche_entity(entity, interface -> win_fiche, ca->symbol);
        }
    } else {
        if (interface -> objet != SYMB_EMPTY_CASE) {

            int color = get_color_clicked(interface -> win_map -> interieur, posX, posY);
            ca -> biome = color;
            ca -> symbol = interface -> objet;
            
            if (interface -> objet == SYMB_MONSTRE || interface -> objet == SYMB_ARTEFACT){
                entity_t* entity;
                
                if ((entity = malloc(sizeof(entity_t))) == NULL) {
                    ncurses_stop();
                    perror("Erreur lors de l'allocation de l'entité ");
                    exit(EXIT_FAILURE);
                }
            
                create_entity(entity, interface -> win_infos, interface -> win_fiche, ca -> symbol);
                
                entity->coord_x = posX;
                entity->coord_y = posY;
                
                add_entity(interface-> map, ca, entity);

                entity->index = interface->map->nb_entity;
            }

            fenetre_mvaddch_col(interface -> win_map, posY, posX, color, interface -> objet);

        } else {
            char symbol = get_symbol_clicked(interface -> win_map -> interieur, posX, posY);
            ca -> biome = interface -> selection;
            ca -> symbol = symbol;
            fenetre_mvaddch_col(interface -> win_map, posY, posX, interface -> selection, symbol);
        }

        fenetre_refresh(interface -> win_map);
    }
}

/**
 *Gestion des actions de l'utilisateur dans l'interface.
 *@param[in,out] interface l'interface
 *@param[in] c la touche pressée
 */
void interface_main(interface_t * interface, int c) {
    int sourisX, sourisY, posX, posY;

    // Gestion des actions de la souris
    if ((c == KEY_MOUSE) && (souris_getpos( & sourisX, & sourisY) == OK)) {
        // Utile en mode debug pour savoir où l'utilisateur a cliqué
        #ifdef _DEBUG_
        fenetre_printw(interface -> win_infos, "\nClic (%d,%d)", sourisX, sourisY);
        fenetre_refresh(interface -> win_infos);
        #endif

        if (fenetre_getcoordonnees(interface -> win_palette, sourisX, sourisY, & posX, & posY)) {
            interface_palette(interface, posX, posY);
        } else if (fenetre_getcoordonnees(interface -> win_map, sourisX, sourisY, & posX, & posY)) {
            interface_map(interface, posX, posY);
        }
    } else {
        /*Gestion du clavier : à modifier pour le projet */
        if ((c >= '0') && (c <= '8'))
            interface_palette(interface, 0, c - '0');
        else
            switch (c) {
            default:
                /*Utile en mode DEBUG si on souhaite afficher le caractère associé à la touche pressée */
                #ifdef _DEBUG_
                fenetre_printw(interface -> win_infos, "\nTouche %d pressee", c);
                fenetre_refresh(interface -> win_infos);
                #endif
                break;
            }
    }
}