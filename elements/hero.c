#include <stdlib.h>
#include <time.h>
#include "../utils/macros.h"
#include "../utils/other_utils.h"
#include "hero.h"
#include "map.h"

/**
 * @brief Créer un hera avec les attributs par defaut
 * 
 * @return hero_t* le hero
 */
hero_t *create_default_hero(){
    hero_t *h = (hero_t*) malloc_check(sizeof(hero_t));

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

    return h;
}

/**
 * @brief Permet de faire spawn le hero uen case valide d'une
 * 
 * @param map la map
 * @param hero le hero
 */
void spawn_hero(map_t *map, hero_t *hero){
    check_null(map);
    check_null(hero);

    case_t *c = get_random_case_to_spawn(map);
  
    c->symbol = SYMB_HERO;
   
    hero->x_spawn = c->coord_x;
    hero->y_spawn = c->coord_y;
    
    hero->coord_x = hero->x_spawn;
    hero->coord_y = hero->y_spawn;
}

/**
 * @brief Permet d'augmenter tous les attributs du hero quand il passe un niveau
 * 
 * @param hero 
 */
void attributes_growth(hero_t *hero){
    hero->max_health = hero->max_health * 1.5;
    hero->strength = hero->strength * 1.5;
    hero->move_speed = hero->move_speed + 1;
}


int add_exp(hero_t *hero, int xp){
    if((hero->xp + xp) >= XP_TO_PASS_LEVEL){
        hero->xp = DEFAULT_HERO_XP;
        attributes_growth(hero);
        return 1;
    } else {
        hero->xp = hero->xp + xp;
    }

    return 0;
}

/**
 * @brief permet au hero de récupérer un trésor
 * 
 * @param hero le hero
 * @param win_info fenêtre d'info
 */
void get_tresor(hero_t *hero, fenetre_t *win_info){
    check_null(hero);
    check_null(win_info);

    srand(time(NULL));

    int random = rand() % 101;

    if (random < PERCENTAGE_LIFE){
        int r_health = rand() % ((int)(hero->max_health/MAX_RECUPERATION_LIFE));
        if (hero->health != hero->max_health){
            if (hero->health + r_health > hero->max_health) {
                hero->health = hero->max_health;
            } else {
                hero->health += r_health;
            }
        }
        fenetre_printw_col(win_info, WHITE, "[Tresor] : +%d PV\n", r_health);
   
    } else {
        if(hero->nb_piece != MAX_NB_PIECE){
            hero->nb_piece++;
            fenetre_printw_col(win_info, YELLOW, "[Tresor] : (LEGENDAIRE) +1 pièce grand-tout\n");
        }
    }
    fenetre_refresh(win_info);
}


/**
 * @brief Permet de d'ajouter et/ou remplacer un artefact dans l'inventaire
 * 
 * @param hero le hero
 * @param artefact l'artefact
 * @param index l'emplacement dans l'inventaire
 * @return entity_t l'artefact à replacer sur la map si on remplace dans l'inventaire 
 */
entity_t add_artefact_in_inventory(hero_t *hero, entity_t artefact, int index){
    check_null(hero);
    check_null(artefact.name);

    entity_t retour;

    if(index < 1 || index > 5){
        return retour;
    }
   
    if (hero->inventory[index - 1].name != NULL){
        remove_artefact_attribute(hero, hero->inventory[index - 1]);
        retour = hero->inventory[index - 1];
    }

    hero->inventory[index - 1] = artefact;
    apply_artefact_attribute(hero, artefact);
    printf("Test : %s\n", hero->inventory[index - 1].name);
    return retour;
}

/**
 * @brief Permet d'appliquer les effets d'un l'artefact au l'hero
 * 
 * @param hero l'hero
 * @param artefact l'artefact
 */
void apply_artefact_attribute(hero_t *hero, entity_t artefact){
    check_null(hero);
    check_null(artefact.name);
        
    if(hero->health - artefact.health < 0) {
        hero->health = 0;
    } else {
        if (hero->health != hero->max_health){
            if (hero->health + artefact.health > hero->max_health) {
                hero->health = hero->max_health;
            } else {
                hero->health += artefact.health;
            }
        }
    }

    if (hero->armor + artefact.armor < 0){
        hero->armor = 0;
    } else {
        hero->armor += artefact.armor;
    }
    
    if (hero->strength + artefact.strength < 0){
        hero->strength = 0;
    } else {
        hero->strength += artefact.strength;
    }
    
    if (hero->move_speed + artefact.move_speed < 0){
        hero->move_speed = 1;
    } else {
        hero->move_speed += artefact.move_speed;
    }
    
    if (hero->hit_speed + artefact.hit_speed < 0){
        hero->hit_speed = 1;
    } else {
        hero->hit_speed += artefact.hit_speed;
    }
}

/**
 * @brief Permet de retirer les effets d'un l'artefact au l'hero
 * 
 * @param hero l'hero
 * @param artefact l'artefact
 */
void remove_artefact_attribute(hero_t *hero, entity_t artefact){
    check_null(hero);
    check_null(artefact.name);

    //si l'artefact à une val neg quand on retire faut ajouter et inversement
    if (artefact.health < 0){
        if (hero->health != hero->max_health){
            if (hero->health + artefact.health > hero->max_health) {
                hero->health = hero->max_health;
            } else {
                hero->health += artefact.health;
            }
        }
    } else {
        if(hero->health - artefact.health < 0) {
             hero->health = 0;
         } else {
             hero->health -= artefact.health;
         }
    }

    if(hero->health - artefact.health < 0) {
        hero->health = 0;
    } else {
        hero->health -= artefact.health;
    }

    if (artefact.armor < 0){
         hero->armor += artefact.armor;
    } else {
        if(hero->armor - artefact.armor < 0) {
            hero->armor = 0;
        } else {
            hero->armor -= artefact.armor;
        }
    }
    
    if (artefact.strength < 0){
         hero->strength += artefact.strength;
    } else {
        if(hero->strength - artefact.strength < 0) {
            hero->strength = 0;
        } else {
            hero->strength -= artefact.strength;
        }
    }
    
    if (artefact.move_speed < 0){
         hero->move_speed += artefact.move_speed;
    } else {
        if(hero->move_speed - artefact.move_speed < 0) {
            hero->move_speed = 1;
        } else {
            hero->move_speed -= artefact.move_speed;
        }
    }

        
    if (artefact.hit_speed < 0){
         hero->hit_speed += artefact.hit_speed;
    } else {
        if(hero->hit_speed - artefact.hit_speed < 0) {
            hero->hit_speed = 1;
        } else {
            hero->hit_speed -= artefact.hit_speed;
        }
    }
}

/**
 * @brief Permet de reset tous les attributs du hero
 * 
 * @param hero le hero
 */
void reset_attribute_value(hero_t *hero){
    check_null(hero);
    
    hero->coord_x = hero->x_spawn;
    hero->coord_y = hero->y_spawn;

    hero->max_health = DEFAULT_HERO_HEALTH;
    hero->health = DEFAULT_HERO_HEALTH;
    hero->armor = DEFAULT_HERO_ARMOR;
    hero->strength = DEFAULT_HERO_STRENGH;
    hero->move_speed = DEFAULT_HERO_MOVE_SPEED;
    hero->hit_speed = DEFAULT_HERO_HIT_SPEED;
    hero->xp = DEFAULT_HERO_XP;
    hero->nb_piece = DEFAULT_HERO_NB_PIECE;
}

/**
 * @brief Affiche les informations du hero
 * 
 * @param hero le hero
 * @param win_hero la fenêtre d'info du hero
 */
void print_hero_stat(hero_t *hero, fenetre_t *win_hero) {
    check_null(hero);
    check_null(win_hero);

    fenetre_erase(win_hero);

    fenetre_printw_col(win_hero, CYAN, "Héro (exp. %d/%d)\n", hero->xp, XP_TO_PASS_LEVEL);

    fenetre_printw_col(win_hero, YELLOW, "\n------------Statistiques------------\n");
    
    fenetre_printw_col(win_hero, WHITE, "Vie : %d/%d\n", hero->health, hero->max_health);
    fenetre_printw_col(win_hero, WHITE, "Armure : %d\n", hero->armor);
    fenetre_printw_col(win_hero, WHITE, "Force : %d\n", hero->strength);
    fenetre_printw_col(win_hero, WHITE, "V.Deplacement : %d (%.1fsec)\n", hero->move_speed, ((double)convert_theoric_move_speed_for_thread(hero->move_speed) / 1000000.0));
    fenetre_printw_col(win_hero, WHITE, "V.Frappe : %d (%.1fsec)\n", hero->hit_speed, ((double)convert_theoric_move_speed_for_thread(hero->hit_speed) / 1000000.0));


    fenetre_printw_col(win_hero, YELLOW, "\n------------Inventaire------------\n");
    
    if(hero->nb_piece == 0){
        fenetre_printw_col(win_hero, WHITE, "Pièces : %d/%d \n", hero->nb_piece, MAX_NB_PIECE);
    } else {
        fenetre_printw_col(win_hero, WHITE, "Pièces : %d/%d ", hero->nb_piece, MAX_NB_PIECE);
        if (hero->nb_piece == 1){
            fenetre_printw_col(win_hero, GREEN, "(niveau 1 disponible)\n");
        } else if (hero->nb_piece == 2){
            fenetre_printw_col(win_hero, YELLOW, "(niveau 2 disponible)\n");
        } else if (hero->nb_piece == 3){
            fenetre_printw_col(win_hero, RED, "(niveau 3 disponible)\n");
        }
    }
    
    
    fenetre_printw_col(win_hero, WHITE, "Artefact : \n");
 

    for (int i = 0; i < INVENTORY_SIZE; i++){
        if (hero->inventory[i].name == NULL){
            fenetre_printw_col(win_hero, WHITE, "  [%d] : Libre\n", (i+1));
        } else {
            entity_t art = hero->inventory[i];
            fenetre_printw_col(win_hero, WHITE, "  [%d] : ", (i+1), art.name);
            dispaly_colored_stat_entity(art, win_hero);
        }
    }

    fenetre_printw_col(win_hero, YELLOW, "\n------------Position------------\n");
    fenetre_printw_col(win_hero, WHITE, "X : %d| Y : %d", hero->coord_y, hero->coord_x);

    fenetre_refresh(win_hero);
}

/**
 * @brief Libière l'espace mémoire d'un héro
 * 
 * @param hero 
 */
void free_hero(hero_t **hero){
    check_null(*hero);
    free(*hero);
    *hero = NULL;
}

