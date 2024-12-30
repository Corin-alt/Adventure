/**
 * @file entity.c
 * @brief Implémentation des fonctions nécessaires à la structure entity_t
 */

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "../utils/macros.h"
#include "../utils/fenetre.h"
#include "../utils/other_utils.h"
#include "entity.h"

void create_entity(entity_t *entity, fenetre_t *info, fenetre_t *fiche, char symb) {
    if (!entity || !info || !fiche) return;

    char *type = (symb == SYMB_MONSTRE) ? "Monstre" : "Artefact";
    char tmp_name[256] = {0};

    entity->type = (symb == SYMB_MONSTRE) ? MONSTER_TYPE : ARTEFACT_TYPE;
    entity->can_attack = (symb == SYMB_MONSTRE) ? TRUE : FALSE;

    fenetre_erase(fiche);
    fenetre_printw_col(fiche, CYAN, "#--%s---#\n", type);
    fenetre_refresh(fiche);

    // Get name
    fenetre_printw_col(info, YELLOW, "[INPUT - %s] - Entrez son nom\n", type);
    fenetre_refresh(info);
    wscanw(info->interieur, "%99s", tmp_name);

    entity->name = strdup(tmp_name);
    if (!entity->name) return;

    // Print stats
    fenetre_printw_col(fiche, CYAN, " -> Nom : %s\n", entity->name);

    // Get stats
    #define GET_STAT(prompt, field) do { \
        fenetre_printw_col(info, YELLOW, "[INPUT - %s] - " prompt "\n", type); \
        fenetre_refresh(info); \
        wscanw(info->interieur, "%d", &entity->field); \
        fenetre_printw_col(fiche, CYAN, " -> " prompt ": %d\n", entity->field); \
        fenetre_refresh(fiche); \
    } while(0)

    GET_STAT("Entrez sa vie", health);
    GET_STAT("Entrez son armure", armor);
    GET_STAT("Entrez sa force", strength);
    GET_STAT("Entrez sa vitesse de deplacement", move_speed);
    GET_STAT("Entrez sa vitesse de frappe", hit_speed);

    #undef GET_STAT

    fenetre_printw_col(info, GREEN, "[%s] - Le %s a été créé\n", entity->name, type);
    fenetre_refresh(info);
}

void print_fiche_entity(entity_t entity, fenetre_t *fiche, char symb) {
    if (!fiche) return;

    char *type = (symb == SYMB_MONSTRE) ? "Monstre" : "Artefact";

    fenetre_erase(fiche);
    fenetre_printw_col(fiche, CYAN, "#---%s---#\n", entity.name);
    fenetre_printw_col(fiche, CYAN, " -> Type : %s\n", type);
    fenetre_printw_col(fiche, CYAN, " -> Vie : %d\n", entity.health);
    fenetre_printw_col(fiche, CYAN, " -> Armure : %d\n", entity.armor);
    fenetre_printw_col(fiche, CYAN, " -> Force : %d\n", entity.strength);
    fenetre_printw_col(fiche, CYAN, " -> V.Deplacement : %d\n", entity.move_speed);
    fenetre_printw_col(fiche, CYAN, " -> V.Frappe : %d\n", entity.hit_speed);
    fenetre_refresh(fiche);
}

void dispaly_colored_stat_entity(entity_t entity, fenetre_t *win_hero) {
    if (!win_hero) return;

    fenetre_printw_col(win_hero, WHITE, "%s : ", entity.name);

    #define PRINT_STAT(val, format) \
        fenetre_printw_col(win_hero, (val) >= 0 ? GREEN : RED, format, (val))

    PRINT_STAT(entity.health, "%+d, ");
    PRINT_STAT(entity.armor, "%+d, ");
    PRINT_STAT(entity.strength, "%+d, ");
    PRINT_STAT(entity.move_speed, "%+d, ");
    PRINT_STAT(entity.hit_speed, "%+d");

    #undef PRINT_STAT

    fenetre_printw_col(win_hero, WHITE, "\n");
}

void free_entity(entity_t **entity) {
    if (!entity || !*entity) return;

    if ((*entity)->name) {
        free((*entity)->name);
        (*entity)->name = NULL;
    }

    free(*entity);
    *entity = NULL;
}