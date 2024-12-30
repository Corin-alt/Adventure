/**
 * @file monster.c
 * @author Dupont Corentin & Lacroix Owen
 * @brief Implémentation des fonctions nécessaire à la structure entity_t
 * @date 2022-02-19
 */

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "../utils/macros.h"
#include "../utils/fenetre.h"
#include "../utils/other_utils.h"
#include "entity.h"

/**
 * @brief Permer de créer une entité
 * 
 * @param [in, out] entity l'entité
 * @param [in] infos la fenêtre d'infos
 * @param [in] fiche la fenêtre de la fiche
 * @return int erreur ou succés
 */
void create_entity(entity_t * entity, fenetre_t * info, fenetre_t * fiche, char symb) {
    
    check_null(entity);
    check_null(info);
    check_null(fiche);

    char *type; 
    char tmp_name[256];
    if(symb == SYMB_MONSTRE) 
    {
        entity->type = MONSTER_TYPE;
        type = "Monstre";
        entity->can_attack = TRUE;
    } 
    else 
    {
        entity->type = ARTEFACT_TYPE;
        type = "Artefact";
        entity->can_attack = FALSE;
    }

    fenetre_erase(fiche);

    fenetre_printw_col(fiche, CYAN, "#--%s---#\n", type);
    fenetre_refresh(fiche);

    //Nom
    fenetre_printw_col(info, YELLOW, "[INPUT - %s] - Entrez son nom\n", type);
    fenetre_refresh(info);

    wscanw(info -> interieur, "%99s", (char *) &tmp_name);
    entity->name = malloc(sizeof(char) * strlen(tmp_name));
    strcpy(entity->name, tmp_name);
  
    fenetre_printw_col(fiche, CYAN, " -> Nom : %s\n",  entity->name);
    fenetre_refresh(fiche);

    //vie
    fenetre_printw_col(info, YELLOW, "[INPUT - %s] - Entrez sa vie\n", type);
    fenetre_refresh(info);

    wscanw(info -> interieur, "%d", & entity -> health);

    fenetre_printw_col(fiche, CYAN, " -> Vie : %d\n", entity -> health);
    fenetre_refresh(fiche);

    //armure
    fenetre_printw_col(info, YELLOW, "[INPUT - %s] - Entrez son armure\n", type);
    fenetre_refresh(info);

    wscanw(info -> interieur, "%d", & entity -> armor);

    fenetre_printw_col(fiche, CYAN, " -> Armure : %d\n", entity -> armor);
    fenetre_refresh(fiche);

    //force
    fenetre_printw_col(info, YELLOW, "[INPUT - %s] - Entrez sa force\n", type);
    fenetre_refresh(info);

    wscanw(info -> interieur, "%d", & entity -> strength);

    fenetre_printw_col(fiche, CYAN, " -> Force : %d\n", entity -> strength);
    fenetre_refresh(fiche);

    //vitesse dep
    fenetre_printw_col(info, YELLOW, "[INPUT - %s] - Entrez sa vitesse de deplacement\n", type);
    fenetre_refresh(info);

    wscanw(info -> interieur, "%d", & entity -> move_speed);

    fenetre_printw_col(fiche, CYAN, " -> V.Deplacement : %d\n", entity -> move_speed);
    fenetre_refresh(fiche);

    //vitesse frappe
    fenetre_printw_col(info, YELLOW, "[INPUT - %s] - Entrez sa vitesse de frappe\n", type);
    fenetre_refresh(info);

    wscanw(info -> interieur, "%d", & entity -> hit_speed);

    fenetre_printw_col(fiche, CYAN, " -> V.Frappe : %d\n", entity -> hit_speed);
    fenetre_refresh(fiche);

    fenetre_printw_col(info, GREEN, "[%s] - Le %s a été crée\n", entity -> name, type);
    fenetre_refresh(info);
}

/**
 * @brief Affiche les info de l'entité
 * 
 * @param [in] entity une entité
 * @param [in] fiche la fenêtre de la fiche
 * @return int erreur ou succés
 */
int print_fiche_entity(entity_t entity, fenetre_t * fiche, char symb) {

    if ( fiche == NULL) {
        return -1;
    }

    char *type; 
    if(symb == SYMB_MONSTRE) 
    {
        type = "Monstre";
    } 
    else 
    {
        type = "Artefacte";
    }

    fenetre_erase(fiche);
    fenetre_printw_col(fiche, CYAN, "#---%s---#\n", entity. name);
    fenetre_printw_col(fiche, CYAN, " -> Type : %s\n", type);
    fenetre_printw_col(fiche, CYAN, " -> Vie : %d\n", entity.health);
    fenetre_printw_col(fiche, CYAN, " -> Armure : %d\n", entity. armor);
    fenetre_printw_col(fiche, CYAN, " -> Force : %d\n", entity. strength);
    fenetre_printw_col(fiche, CYAN, " -> V.Deplacement : %d\n", entity . move_speed);
    fenetre_printw_col(fiche, CYAN, " -> V.Frappe : %d\n", entity. hit_speed);
    fenetre_refresh(fiche);

    return 0;
}



void dispaly_colored_stat_entity(entity_t entity, fenetre_t * win_hero){
    
    
    fenetre_printw_col(win_hero, WHITE, "%s : ", entity.name);

    if(entity.health >= 0){
        fenetre_printw_col(win_hero, GREEN, "+%d, ", entity.health);
    } else {
        fenetre_printw_col(win_hero, RED, "%d, ", entity.health);
    }
    
    if(entity.armor >= 0){
        fenetre_printw_col(win_hero, GREEN, "+%d, ", entity.armor);
    } else {
        fenetre_printw_col(win_hero, RED, "%d, ", entity.armor);
    }

    if(entity.strength >= 0){
        fenetre_printw_col(win_hero, GREEN, "+%d, ", entity.strength);
    } else {
        fenetre_printw_col(win_hero, RED, "%d, ", entity.strength);
    }

    if(entity.move_speed >= 0){
        fenetre_printw_col(win_hero, GREEN, "+%d, ", entity.move_speed);
    } else {
        fenetre_printw_col(win_hero, RED, "%d, ", entity.move_speed);
    }
    
    if(entity.hit_speed >= 0){
        fenetre_printw_col(win_hero, GREEN, "+%d", entity.hit_speed);
    } else {
        fenetre_printw_col(win_hero, RED, "%d", entity.hit_speed);
    }
    
    fenetre_printw_col(win_hero, WHITE, ")\n");
}

/**
 * @brief Libére l'espace mémoire d'une entité
 * 
 * @param [in] entity une entité
 * @return int erreur ou succés
 */
int free_entity(entity_t **entity) {
    if (*entity == NULL) {
        return -1;
    }
    free(*entity);
    *entity = NULL;
    
    return 0;
}