# VoxEngine
## Dépendances
Le moteur nécessite plusieurs bibliothèques pour fonctionner
- [SDL2](https://www.libsdl.org/index.php) pour la gestion des fenêtres et entrées clavier
- [Assimp](http://www.assimp.org/) pour la lecture des fichiers 3D sous tous les formats majoritairement utilisés (obj, fbx, dae, ...)
- [CImg](http://www.cimg.eu/) pour la gestion des maps (déja inclus)
- [stb](https://github.com/nothings/stb) pour le chargement de textures aux formats JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC (déja inclus)
- [GLEW](https://github.com/nigels-com/glew) pour l'initialisation du contexte openGL
- [GLM](https://github.com/g-truc/glm) pour unifier types utilisés pour les calculs coté CPU et GPU 

Un compilateur C++11 est également requis, ainsi qu'une partie graphique(GPU ou IGP) compatible openGL 4.3

## Installation
Une refonte de la toolchain de compilation est en cours et bientôt ces étapes ne seront plus à la charge de l'utilisateur.
### CImg / stb
CImg et stb sont des bibliothèques contenues dans un seul header.

Elles sont distribuées avec le projet et ne nécessitent aucune autre configuration.

### SDL / Assimp / GLEW / GLM
Il faut donc installer Assimp, SDL 1 et 2, GLEW et GLM pour pouvoir compiler le moteur.

Ces bibliothèques étant totalement portables le moteur est à priori compatible avec tous les principaux OS.

#### Unix (Mac et Linux)
La compilation manuelle pour système Unix est possible pour toutes les bibliothèques
- [SDL compilation Mac OS X](https://wiki.libsdl.org/Installation#Mac_OS_X)
- [SDL compilation Linux](https://wiki.libsdl.org/Installation#Linux.2FUnix)
- [Assimp Compilation](https://github.com/assimp/assimp#building)

#### Ubuntu (Linux base Debian)
Pour ubuntu il suffit d'installer les paquets 
- `libsdl2-dev` et `libsdl-dev`pour la SDL
- `libassimp-dev` pour Assimp
- `libglew-dev` pour GLEW
- `libglm-dev` pour GLM

#### Windows
SDL et Assimp proposent des versions déjà compilées ainsi que des options de compilation pour Windows.
- [SDL Installation Windows](https://wiki.libsdl.org/Installation#Windows_XP.2FVista.2F7)
- [Assimp Compilation Windows](https://github.com/assimp/assimp#building)

L'utilisation de MSYS2 est cependant fortement conseillée (en particulier MingW32) avec les paquets Arch suivants :
- `mingw-w64-i686-SDL` et `mingw-w64-i686-SDL2` pour la SDL
- `mingw-w64-i686-assimp` pour Assimp
- `mingw-w64-i686-glm` pour GLM
- `mingw-w64-i686-glew` pour GLEW

Ces paquets s'installent simplement avec la commande `pacman -S <nom du paquet>` depuis une console MSYS2 et il suffit de remplacer
`i686` par `x86_64` dans le nom des paquets pour obtenir leur version 64 bits.

## Compilation
### Linux et utilisateurs de MSYS2 (Windows)
Un script CMake permet de compiler simplement le projet en suivant ces même commandes pour Linux et MSYS2 :
- `mkdir build && cd build`
- `cmake -G "Unix Makefiles" ..`
- `make`
- `cd ..`
- `Lancement du moteur`

## Lancement
### Pour créer une nouvelle Map
`./VoxEngine new <mapMame> <biomeName>`

### Pour charger une Map
`./VoxEngine load <mapName>`

### Nom des biomes actuels
`standard, prairie, valley, snowPrairie, snowValley, desert, archipel, mangrove`

### Visualisation de plusieurs map
L'option multi permet de visualiser jusqu'à 4 map de biomes différents en même temps.
#### Exemple
`./VoxEngine multi new part1 desert new part2 desert new part3 archipel new part4 archipel`

## Gameplay
Une fois le jeu lancé, les commandes suivantes sont disponibles
- Déplacement du personnage avec les touches `Z,Q,S,D` ou `W,A,S,D`
- Sauter avec `Espace` (possible autant de fois que l'on veut)
- La caméra suit la souris comme dans un FPS
- La touche `Echap` permet de quitter le jeu.
- Le joueur peut retirer le cube en face de lui avec la touche `V`
- Il est également possible de placer des cubes avec la touche `C`
- Le cube posable par défaut est de la GlowStone qui pose également une lumière au même emplacement.
- Il est possible de changer le type de cube à poser avec les touches `1 à 8`(pas du pavé numérique)
- (1=Dirt, 2=Stone, 3=Wood, 4=GlowStone, 5=Grass, 6=Sand, 7=Snow, 8=Water)
- Enfin il est possible de changer le mode de rendu avec `P` et comparer à volonté les modes direct et différé
- Par défaut le jeu se lance en rendu direct qui gère 50 lumières, le rendu différé en gère actuellement 250 
- De possibles optimisations permettraient de considérblement augmenter ce nombre


## Bibliographie
[Etude bibliographique sur les effets spéciaux](https://docs.google.com/presentation/d/1cfbddsps-8mRLpAKf8Bp2wP-CRA9d0MPAZ0FhgjB3E8/edit?usp=sharing)

[Slides de la présentation du projet](https://drive.google.com/open?id=1QN82RJVtlLTZykGtKLsOQzO_qnXBlAVzbdjbhIz7ms4)

### Si l'un des deux liens précédents ne fonctionne pas, contactez [quentin.philippot@gmail.com](quentin.philippot@gmail.com)
