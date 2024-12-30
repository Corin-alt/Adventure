#include <stdlib.h>
#include <string.h>
#include "../utils/macros.h"
#include "../utils/ncurses_utils.h"
#include "../elements/map.h"
#include "../elements/entity.h"
#include "game_int.h"

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

void game_interface_dimensions(unsigned short largeur, unsigned short hauteur) {
    if (COLS < largeur + 17 || LINES < hauteur + 12) {
        ncurses_stop();
        fprintf(stderr, "Terminal trop petit : besoin de l=%d,h=%d (actuel: l=%d,h=%d)\n",
                largeur + 17, hauteur + 12, COLS, LINES);
        exit(EXIT_FAILURE);
    }
}

interface_t *game_interface_creer(map_t *map, hero_t *hero) {
    if (!map || !hero) return NULL;

    game_interface_dimensions(map->width, map->height);

    interface_t *retour = malloc_check(sizeof(interface_t));
    if (!retour) return NULL;

    retour->hero = hero;

    // Fenêtre d'informations
    retour->win_infos = fenetre_creer(0, 0, COLS, 10, "Informations", TRUE);
    if (!retour->win_infos) {
        free(retour);
        return NULL;
    }
    fenetre_printw_col(retour->win_infos, RED, "- Appuyez sur 'Q' ou 'q' pour quitter.\n");
    fenetre_refresh(retour->win_infos);

    // Fenêtre de carte
    retour->map = map;
    retour->win_map = fenetre_creer(0, 10, 2 + map->width, 2 + map->height, "Carte", FALSE);
    if (!retour->win_map) {
        fenetre_supprimer(&retour->win_infos);
        free(retour);
        return NULL;
    }

    for (int x = 0; x < NB_LIGNE; x++) {
        for (int y = 0; y < NB_COL; y++) {
            case_t *ca = get_case(map, x, y);
            if (ca) {
                fenetre_mvaddch_col(retour->win_map, x, y, ca->biome, ca->symbol);
            }
        }
    }
    fenetre_refresh(retour->win_map);

    // Fenêtre de héros
    retour->win_hero = fenetre_creer(60, 10, 40, 4 + map->height, "Attributs", FALSE);
    if (!retour->win_hero) {
        fenetre_supprimer(&retour->win_map);
        fenetre_supprimer(&retour->win_infos);
        free(retour);
        return NULL;
    }
    fenetre_refresh(retour->win_hero);

    return retour;
}

void game_interface_supprimer(interface_t **interface) {
    if (!interface || !*interface) return;

    if ((*interface)->win_infos) fenetre_supprimer(&(*interface)->win_infos);
    if ((*interface)->win_map) fenetre_supprimer(&(*interface)->win_map);
    if ((*interface)->win_hero) fenetre_supprimer(&(*interface)->win_hero);

    free(*interface);
    *interface = NULL;
}

void refresh_map_in_interface(interface_t *interface, map_t *map) {
    if (!interface || !map || !interface->win_map) return;

    for (int x = 0; x < NB_LIGNE; x++) {
        for (int y = 0; y < NB_COL; y++) {
            case_t *ca = get_case(map, x, y);
            if (ca) {
                fenetre_mvaddch_col(interface->win_map, x, y, ca->biome, ca->symbol);
            }
        }
    }
    fenetre_refresh(interface->win_map);
}

static void handle_artefact_pickup(interface_t *interface, case_t *ca, case_t *old_case) {
    if (!interface || !ca || !old_case) return;

    int tmp;
    entity_t entity = get_entity(interface->map, ca);

    fenetre_printw_col(interface->win_infos, YELLOW,
                      "Voulez-vous ramasser l'artefact : (non : 0 | oui : 1)\n");
    dispaly_colored_stat_entity(entity, interface->win_infos);
    wscanw(interface->win_infos->interieur, "%d", &tmp);

    if (tmp == 1) {
        fenetre_printw_col(interface->win_infos, YELLOW,
                          "Placez l'artefact dans l'inventaire [1-5] :\n");
        fenetre_refresh(interface->win_infos);
        wscanw(interface->win_infos->interieur, "%d", &tmp);

        entity_t art_replace = add_artefact_in_inventory(interface->hero, entity, tmp);
        fenetre_printw_col(interface->win_infos, WHITE,
                          "L'artefact %s a été placé à l'emplacement %d\n",
                          entity.name, tmp);

        if (art_replace.name) {
            fenetre_printw_col(interface->win_infos, WHITE,
                              "L'artefact %s a été drop\n", art_replace.name);
            old_case->symbol = SYMB_ARTEFACT;
            add_entity(interface->map, old_case, &art_replace);
            ca->symbol = SYMB_HERO;
        }
        print_hero_stat(interface->hero, interface->win_hero);
    } else {
        fenetre_printw_col(interface->win_infos, WHITE,
                          "L'artefact n'a pas été ramassé\n");
        interface->hero->coord_x = old_case->coord_x;
        interface->hero->coord_y = old_case->coord_y;
    }
}

void game_interface_map(interface_t *interface, int c) {
    if (!interface || !interface->hero || !interface->map) return;

    int copy_x = interface->hero->coord_x;
    int copy_y = interface->hero->coord_y;

    switch(c) {
        case KEY_UP:    interface->hero->coord_x--; break;
        case KEY_DOWN:  interface->hero->coord_x++; break;
        case KEY_LEFT:  interface->hero->coord_y--; break;
        case KEY_RIGHT: interface->hero->coord_y++; break;
        default: return;
    }

    if (!is_out_of_map(interface->hero->coord_x, interface->hero->coord_y)) {
        if (can_move(interface->map, interface->hero->coord_x, interface->hero->coord_y)) {
            case_t *dest = get_case(interface->map, interface->hero->coord_x, interface->hero->coord_y);
            case_t *old = get_case(interface->map, copy_x, copy_y);

            if (!dest || !old) {
                interface->hero->coord_x = copy_x;
                interface->hero->coord_y = copy_y;
                return;
            }

            if (is_artefact(interface->map, interface->hero->coord_x, interface->hero->coord_y)) {
                handle_artefact_pickup(interface, dest, old);
            }
            else if (is_tresor(interface->map, interface->hero->coord_x, interface->hero->coord_y)) {
                get_tresor(interface->hero, interface->win_infos);
                print_hero_stat(interface->hero, interface->win_hero);
                old->symbol = SYMB_EMPTY_CASE;
                dest->symbol = SYMB_HERO;
            }
            else if (!is_monster(interface->map, dest->coord_x, dest->coord_y) &&
                     !is_hero(interface->map, dest->coord_x, dest->coord_y)) {
                old->symbol = SYMB_EMPTY_CASE;
                dest->symbol = SYMB_HERO;
            }
            else {
                interface->hero->coord_x = copy_x;
                interface->hero->coord_y = copy_y;
            }
        } else {
            interface->hero->coord_x = copy_x;
            interface->hero->coord_y = copy_y;
        }
    } else {
        interface->hero->coord_x = copy_x;
        interface->hero->coord_y = copy_y;
    }

    fenetre_refresh(interface->win_infos);
    refresh_map_in_interface(interface, interface->map);
}

void game_interface_main(interface_t *interface, int c) {
    if (!interface) return;

    int sourisX, sourisY;
    if (c == KEY_MOUSE && souris_getpos(&sourisX, &sourisY) == OK) {
        // Handle mouse input here if needed
    } else {
        game_interface_map(interface, c);
    }
}