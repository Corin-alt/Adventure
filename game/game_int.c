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
#include "game_int.h"

/**
 *Définition de la palette.
 */
void game_palette() {
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
void game_interface_dimensions(unsigned short largeur, unsigned short hauteur) {
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
interface_t * game_interface_creer(map_t * map, hero_t *hero) {
    interface_t * retour;

    // Vérification des dimensions
    game_interface_dimensions(map -> width, map -> height);

    // Allocation de la structure
    if ((retour = malloc(sizeof(interface_t))) == NULL) {
        ncurses_stop();
        perror("Erreur lors de la création de l'interface ");
        exit(EXIT_FAILURE);
    }
    
    retour->hero = hero;

    /*-----------------------------Infos-------------------------------*/
    retour -> win_infos = fenetre_creer(0, 0, COLS, 10, "Informations", TRUE);
    fenetre_printw_col(retour -> win_infos, RED, "- Appuyez sur 'Q' OU 'q' pour quitter.\n");
    fenetre_refresh(retour -> win_infos);


    /*-----------------------------Map---------------------------------*/
    retour -> map = map;
    retour -> win_map = fenetre_creer(0, 10, 2 + map -> width, 2 + map -> height, "Carte", FALSE);
    for (int x = 0; x < NB_LIGNE; x++) {
        for (int y = 0; y < NB_COL; y++) {
            case_t * ca = get_case(map, x, y);
            fenetre_mvaddch_col(retour -> win_map, x, y, ca -> biome, ca -> symbol); 
        }
    } 
    fenetre_refresh(retour -> win_map);


    /*-----------------------------Heros-----------------------------*/
    retour -> win_hero = fenetre_creer(60, 10, 40, 4 + map -> height, "Attributs", FALSE);
    fenetre_refresh(retour -> win_hero);

    return retour;
}

/**
 *Suppression d'une interface.
 *@param[in,out] interface l'interface à supprimer
 */
void game_interface_supprimer(interface_t ** interface) {
    fenetre_supprimer(&(*interface) -> win_infos);
    fenetre_supprimer(&(*interface) -> win_map);
    fenetre_supprimer(&(*interface) -> win_hero);
    free_map(&(*interface) -> map);
    free_hero(&(*interface) -> hero);
    free(*interface);
    interface = NULL;
}


/**
 * @brief Permet de rafaichier l'affichage de la map
 * 
 * @param interface l'interface
 * @param map la map
 */
void refresh_map_in_interface(interface_t * interface, map_t *map){
    for (int x = 0; x < NB_LIGNE; x++) {
        for (int y = 0; y < NB_COL; y++) {
            case_t * ca = get_case(map, x, y);
            fenetre_mvaddch_col(interface -> win_map, x, y, ca -> biome, ca -> symbol); 
        }
    } 
    fenetre_refresh(interface -> win_map);
}

/**
 *Gestion des actions dans la fenêtre de la map.
 *@param[in,out] interface l'interface
 *@param[in] posX la position X du clic dans la fenêtre
 *@param[in] posY la position Y du clic dans la fenêtre
 */
void game_interface_map(interface_t * interface, int c) {
    
    int copy_x = interface->hero->coord_x;
    int copy_y = interface->hero->coord_y;
    
    if (c == KEY_UP){
        interface->hero->coord_x -= 1;
    }else if (c == KEY_DOWN){
        interface->hero->coord_x += 1;
    } else if (c == KEY_LEFT){
        interface->hero->coord_y -= 1;
    } else if (c == KEY_RIGHT){
        interface->hero->coord_y += 1;
    }

    int test = 0;

    if(!is_out_of_map(interface->hero->coord_x, interface->hero->coord_y)){
        if(can_move(interface->map, interface->hero->coord_x, interface->hero->coord_y)){ 
            //ARTEFACT
            if(is_artefact(interface->map, interface->hero->coord_x, interface->hero->coord_y)){
                int tmp;
                case_t *ca = get_case(interface->map, interface->hero->coord_x, interface->hero->coord_y);
                entity_t entity = get_entity(interface->map, ca);

                fenetre_printw_col(interface -> win_infos, YELLOW, "Voulez-vous rammasser l'artefact : (non : 0 | oui : 1) \n");
                dispaly_colored_stat_entity(entity, interface -> win_infos);
                wscanw(interface -> win_infos->interieur, "%d", &tmp);

                if(tmp == 1){

                    fenetre_printw_col(interface -> win_infos, YELLOW, "Placez l'artefact dans l'inventaire [1-5] :\n");
                    fenetre_refresh(interface -> win_infos);

                    wscanw(interface -> win_infos->interieur, "%d", &tmp);

                    entity_t art_replace = add_artefact_in_inventory(interface->hero, entity, tmp);
                    
                    fenetre_printw_col(interface -> win_infos, WHITE, "L'artefact %s a été placé à l'emplacement %d de l'inventaire.\n", entity.name, tmp);
                    
                    if (art_replace.name != NULL){
                        test = 1;
                        fenetre_printw_col(interface -> win_infos, WHITE, "L'artefact %s a été drop.\n", art_replace.name, tmp);
                        
                        case_t *old = get_case(interface->map, copy_x, copy_y);
                        old->symbol = SYMB_ARTEFACT;
                        add_entity(interface->map, old, &art_replace);  

                        case_t *new = get_case(interface->map, interface->hero->coord_x, interface->hero->coord_y);
                        new->symbol = SYMB_HERO; 
                    }
                    print_hero_stat(interface->hero, interface->win_hero);  
                } else {
                    fenetre_printw_col(interface -> win_infos, WHITE, "L'artefact n'a pas été rammasé.\n");
                    interface->hero->coord_x = copy_x;  
                    interface->hero->coord_y = copy_y;
                }
            } 
            
            //TRESOR
            else if (is_tresor(interface->map, interface->hero->coord_x, interface->hero->coord_y)){
                get_tresor(interface->hero, interface->win_infos);
                print_hero_stat(interface->hero, interface->win_hero);
            }
            
            
            
            
            else if (is_monster(interface->map, interface->hero->coord_x, interface->hero->coord_y)) {
                fenetre_printw_col(interface -> win_infos, GREEN, "MONSTRE\n");
            } 
            

            
            else if (is_hero(interface->map, interface->hero->coord_x, interface->hero->coord_y)){
                 fenetre_printw_col(interface -> win_infos, GREEN, "HERO\n");
            }

            if(test != 1) {
                case_t *old = get_case(interface->map, copy_x, copy_y);
                old->symbol = ' ';
                case_t *new = get_case(interface->map, interface->hero->coord_x, interface->hero->coord_y);
                new->symbol = SYMB_HERO; 
            }

        } else {
            interface->hero->coord_x = copy_x;  
            interface->hero->coord_y = copy_y;
        }
    } else {

        //change de map    







        interface->hero->coord_x = copy_x;  
        interface->hero->coord_y = copy_y;
    }
    
    fenetre_refresh(interface -> win_infos);
    refresh_map_in_interface(interface, interface->map);
}

/**
 *Gestion des actions de l'utilisateur dans l'interface.
 *@param[in,out] interface l'interface
 *@param[in] c la touche pressée
 */
void game_interface_main(interface_t * interface, int c) {
    int sourisX, sourisY;
    //posX, posY;

    // Gestion des actions de la souris
    if ((c == KEY_MOUSE) && (souris_getpos( & sourisX, & sourisY) == OK)) {
        /**
        if (fenetre_getcoordonnees(interface -> win_palette, sourisX, sourisY, & posX, & posY)) {
            interface_palette(interface, posX, posY);
        } else if (fenetre_getcoordonnees(interface -> win_map, sourisX, sourisY, & posX, & posY)) {
            interface_map(interface, posX, posY);
        }
        */
    } else {
        switch (c) {
            default:
                game_interface_map(interface, c);
                break;
            }
    }
}