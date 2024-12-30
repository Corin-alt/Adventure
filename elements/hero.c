#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../utils/macros.h"
#include "../utils/other_utils.h"
#include "hero.h"
#include "map.h"

hero_t *create_default_hero() {
    hero_t *h = (hero_t*)malloc_check(sizeof(hero_t));
    if (!h) return NULL;

    // Initialize all values to 0
    memset(h, 0, sizeof(hero_t));

    // Set default values
    h->map_x = 0;
    h->map_y = 0;
    h->coord_x = 0;
    h->coord_y = 0;
    h->x_spawn = 0;
    h->y_spawn = 0;
    h->max_health = DEFAULT_HERO_HEALTH;
    h->health = DEFAULT_HERO_HEALTH;
    h->armor = DEFAULT_HERO_ARMOR;
    h->strength = DEFAULT_HERO_STRENGH;
    h->move_speed = DEFAULT_HERO_MOVE_SPEED;
    h->hit_speed = DEFAULT_HERO_HIT_SPEED;
    h->xp = DEFAULT_HERO_XP;
    h->nb_piece = DEFAULT_HERO_NB_PIECE;
    h->nb_art_in_inv = DEFAULT_HERO_NB_ART;

    // Initialize inventory
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        h->inventory[i].name = NULL;
    }

    return h;
}

void spawn_hero(map_t *map, hero_t *hero) {
    if (!map || !hero) return;

    case_t *c = get_random_case_to_spawn(map);
    if (!c) return;

    c->symbol = SYMB_HERO;

    hero->x_spawn = c->coord_x;
    hero->y_spawn = c->coord_y;

    hero->coord_x = hero->x_spawn;
    hero->coord_y = hero->y_spawn;
}

void attributes_growth(hero_t *hero) {
    if (!hero) return;

    hero->max_health = (int)(hero->max_health * 1.5);
    hero->strength = (int)(hero->strength * 1.5);
    hero->move_speed = hero->move_speed + 1;
}

int add_exp(hero_t *hero, int xp) {
    if (!hero || xp < 0) return 0;

    if((hero->xp + xp) >= XP_TO_PASS_LEVEL) {
        hero->xp = DEFAULT_HERO_XP;
        attributes_growth(hero);
        return 1;
    }

    hero->xp += xp;
    return 0;
}

void get_tresor(hero_t *hero, fenetre_t *win_info) {
    if (!hero || !win_info) return;

    srand((unsigned int)time(NULL));
    int random = rand() % 101;

    if (random < PERCENTAGE_LIFE) {
        int r_health = rand() % ((int)(hero->max_health/MAX_RECUPERATION_LIFE));
        if (hero->health != hero->max_health) {
            hero->health = (hero->health + r_health > hero->max_health) ?
                          hero->max_health : hero->health + r_health;
        }
        fenetre_printw_col(win_info, WHITE, "[Tresor] : +%d PV\n", r_health);
    }
    else if (hero->nb_piece != MAX_NB_PIECE) {
        hero->nb_piece++;
        fenetre_printw_col(win_info, YELLOW, "[Tresor] : (LEGENDAIRE) +1 pièce grand-tout\n");
    }

    fenetre_refresh(win_info);
}

entity_t add_artefact_in_inventory(hero_t *hero, entity_t artefact, int index) {
    entity_t retour = {0};

    if (!hero || !artefact.name || index < 1 || index > 5) {
        return retour;
    }

    index--;
    if (hero->inventory[index].name) {
        remove_artefact_attribute(hero, hero->inventory[index]);
        retour = hero->inventory[index];
    }

    hero->inventory[index] = artefact;
    apply_artefact_attribute(hero, artefact);

    return retour;
}

void apply_artefact_attribute(hero_t *hero, entity_t artefact) {
    if (!hero || !artefact.name) return;

    // Health
    if (hero->health != hero->max_health) {
        int new_health = hero->health + artefact.health;
        hero->health = (new_health > hero->max_health) ?
                      hero->max_health :
                      (new_health < 0) ? 0 : new_health;
    }

    // Other attributes
    hero->armor = (hero->armor + artefact.armor < 0) ? 0 : hero->armor + artefact.armor;
    hero->strength = (hero->strength + artefact.strength < 0) ? 0 : hero->strength + artefact.strength;
    hero->move_speed = (hero->move_speed + artefact.move_speed < 1) ? 1 : hero->move_speed + artefact.move_speed;
    hero->hit_speed = (hero->hit_speed + artefact.hit_speed < 1) ? 1 : hero->hit_speed + artefact.hit_speed;
}

void remove_artefact_attribute(hero_t *hero, entity_t artefact) {
    if (!hero || !artefact.name) return;

    // Handle attribute removal with negative checks
    hero->health = (hero->health - artefact.health < 0) ? 0 : hero->health - artefact.health;
    hero->armor = (hero->armor - artefact.armor < 0) ? 0 : hero->armor - artefact.armor;
    hero->strength = (hero->strength - artefact.strength < 0) ? 0 : hero->strength - artefact.strength;
    hero->move_speed = (hero->move_speed - artefact.move_speed < 1) ? 1 : hero->move_speed - artefact.move_speed;
    hero->hit_speed = (hero->hit_speed - artefact.hit_speed < 1) ? 1 : hero->hit_speed - artefact.hit_speed;
}

void reset_attribute_value(hero_t *hero) {
    if (!hero) return;

    // Reset position
    hero->coord_x = hero->x_spawn;
    hero->coord_y = hero->y_spawn;

    // Reset attributes to defaults
    hero->max_health = DEFAULT_HERO_HEALTH;
    hero->health = DEFAULT_HERO_HEALTH;
    hero->armor = DEFAULT_HERO_ARMOR;
    hero->strength = DEFAULT_HERO_STRENGH;
    hero->move_speed = DEFAULT_HERO_MOVE_SPEED;
    hero->hit_speed = DEFAULT_HERO_HIT_SPEED;
    hero->xp = DEFAULT_HERO_XP;
    hero->nb_piece = DEFAULT_HERO_NB_PIECE;
}

void print_hero_stat(hero_t *hero, fenetre_t *win_hero) {
    if (!hero || !win_hero) return;

    fenetre_erase(win_hero);

    fenetre_printw_col(win_hero, CYAN, "Héro (exp. %d/%d)\n", hero->xp, XP_TO_PASS_LEVEL);
    fenetre_printw_col(win_hero, YELLOW, "\n------------Statistiques------------\n");
    fenetre_printw_col(win_hero, WHITE, "Vie : %d/%d\n", hero->health, hero->max_health);
    fenetre_printw_col(win_hero, WHITE, "Armure : %d\n", hero->armor);
    fenetre_printw_col(win_hero, WHITE, "Force : %d\n", hero->strength);
    fenetre_printw_col(win_hero, WHITE, "V.Deplacement : %d (%.1fsec)\n",
                      hero->move_speed,
                      ((double)convert_theoric_move_speed_for_thread(hero->move_speed) / 1000000.0));
    fenetre_printw_col(win_hero, WHITE, "V.Frappe : %d (%.1fsec)\n",
                      hero->hit_speed,
                      ((double)convert_theoric_move_speed_for_thread(hero->hit_speed) / 1000000.0));

    // Inventory section
    fenetre_printw_col(win_hero, YELLOW, "\n------------Inventaire------------\n");

    // Print pieces info
    if (hero->nb_piece == 0) {
        fenetre_printw_col(win_hero, WHITE, "Pièces : %d/%d \n", hero->nb_piece, MAX_NB_PIECE);
    } else {
        fenetre_printw_col(win_hero, WHITE, "Pièces : %d/%d ", hero->nb_piece, MAX_NB_PIECE);
        switch (hero->nb_piece) {
            case 1: fenetre_printw_col(win_hero, GREEN, "(niveau 1 disponible)\n"); break;
            case 2: fenetre_printw_col(win_hero, YELLOW, "(niveau 2 disponible)\n"); break;
            case 3: fenetre_printw_col(win_hero, RED, "(niveau 3 disponible)\n"); break;
        }
    }

    // Print artifacts
    fenetre_printw_col(win_hero, WHITE, "Artefact : \n");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (!hero->inventory[i].name) {
            fenetre_printw_col(win_hero, WHITE, "  [%d] : Libre\n", (i+1));
        } else {
            entity_t art = hero->inventory[i];
            fenetre_printw_col(win_hero, WHITE, "  [%d] : ", (i+1));
            dispaly_colored_stat_entity(art, win_hero);
        }
    }

    // Position section
    fenetre_printw_col(win_hero, YELLOW, "\n------------Position------------\n");
    fenetre_printw_col(win_hero, WHITE, "X : %d| Y : %d", hero->coord_y, hero->coord_x);

    fenetre_refresh(win_hero);
}

void free_hero(hero_t **hero) {
    if (!hero || !*hero) return;

    // Free inventory items
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if ((*hero)->inventory[i].name) {
            free((*hero)->inventory[i].name);
            (*hero)->inventory[i].name = NULL;
        }
    }

    free(*hero);
    *hero = NULL;
}