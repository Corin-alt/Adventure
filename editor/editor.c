/**
 * @file editeur.c
 * @author Dupont Corentin & Lacroix Owen
 * @brief l'éditeur
 * @date 2022-02-19
 */

#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include "interface.h"
#include "../utils/macros.h"
#include "../utils/ncurses_utils.h"
#include "../elements/map.h"

int main(int argc, char * argv[]) {
    int ch;
    interface_t * interface;
    map_t * map;

    bool quitter = FALSE;

    // Vérification des arguments et affichage de l'aide    
    if (argc == 2) {
        // Vérification de l'extension
        if (strstr(argv[1], ".bin") == NULL) {
            fprintf(stderr, "Le programme n'accepte que des fichiers binaire (.bin).\n\t\"%s\" n'est pas un fichier binaire\n", argv[1]);
            exit(EXIT_FAILURE);
        }

        // Chargement d'un fichier
        if ((map = load_map(argv[1])) == NULL) {
            //si la map n'existe pas on en créer une vide et pas besoin de spécifier les dimensions car elles sont obligatoirement de 20 par 40
            map = init_empty_map(argv[1], NB_LIGNE, NB_COL);
        }
    } else {
        // Les arguments sont incorrects
        fprintf(stderr, "Chargement d'un fichier existant :\n\t%s <fichier>\n[Param] : fichier : le nom du fichier à charger\n[Info] - Si ce dernier n'existe pas il sera crée.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialisation de ncurses
    setlocale(LC_ALL, "en_US.UTF-8");
    ncurses_init();
    ncurses_initsouris();
    ncurses_couleurs();
    palette();
    clear();
    refresh();

    // Création de l'interface
    interface = interface_creer(map);

    // Boucle principale
    while (quitter == FALSE) {
        ch = getch();
        if ((ch == 'S') || (ch == 's')) {
            save_map(argv[1], map);
            fenetre_printw_col(interface -> win_infos, GREEN, "[Save] - La map a bien été sauvegardée. (\"%s\")\n", argv[1]);
            fenetre_refresh(interface -> win_infos);
        }
        if ((ch == 'Q') || (ch == 'q'))
            quitter = true;
        else
            interface_main(interface, ch);
    }

    // Arrêt de ncurses
    ncurses_stop();

    // Sauvegarde de la map
    save_map(argv[1], map);

    // Suppression de l'interface
    interface_supprimer(&interface);

    return EXIT_SUCCESS;
}