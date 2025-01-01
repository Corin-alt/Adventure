# Jeu d'aventure en C

Cette application qui est un jeu d'aventure est le projet commun à la matière **système d'exploitation avancée** et **programmationmulti-threadée** de la troisième année de licence Informatique.

Cette application vous permet de créer vos propre carte de jeu.

#### __Spécificités__

- Multi-Joueurs en réseaux
- Multi-Threadé
- NCurses

#### __Règles du jeu__

#### __Compilation__
1- Ouvrez un terminal depuis ```/projet601-604```

2- Nettoyez le dossier :

    - make clean
    
3- Puis compilez :

    - make

#### __Utilisation__

- ##### __Éditeur__

L'éditeur permet de créer des carte de jeu, pour l'aventure. Celui-ci ne prend que des fichiers binaire.

Pour accéder à l'éditeur, ouvrez un terminal depuis ```/projet601-604/editor``` puis :

    - ./editeur <carte.bin>

- ``<carte.bin>`` : le fichier de la carte, s'il n'existe pas alors le fichierest est crée sinon le fichier est chargé

Les cartes sont sauvegardées dans  ```/projet601-604/carts```
    
- ##### __Jeu__

#### Auteurs

- Dupont Corentin
- Lacroix Owen