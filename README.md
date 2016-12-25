# VoxEngine
## Dépendances
Le moteur nécessite plusieurs bibliothèques pour fonctionner
- [SDL2](https://www.libsdl.org/index.php) pour la gestion des fenêtres et entrées clavier
- [Assimp](http://www.assimp.org/) pour la lecture des fichiers 3D sous tous les formats majoritairement utilisés (obj, fbx, dae, ...)
- [CImg](http://www.cimg.eu/) pour la gestion des maps (déja inclus)
- [stb](https://github.com/nothings/stb) pour le chargement de textures aux formats JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC (déja inclus)

Un compilateur C++11 est également requis, ainsi qu'une partie graphique(GPU ou IGP) compatible openGL 4.3

## Installation
### CImg / stb
CImg et stb sont des bibliothèques contenues dans un seul header.

Elles sont déja inclus dans le projet et nécessitent aucune autre configuration.

### SDL / Assimp
Il n'y a donc que Assimp et SDL à installer pour pouvoir compiler le moteur.
Ces deux bibliothèques étant totalement portables le moteur est à priori compatible avec les principaux OS.

#### Unix (Mac et Linux)
La compilation manuelle pour système Unix est possible pour les deux bibliothèques
- [SDL compilation Mac OS X](https://wiki.libsdl.org/Installation#Mac_OS_X)
- [SDL compilation Linux](https://wiki.libsdl.org/Installation#Linux.2FUnix)
- [Assimp Compilation](https://github.com/assimp/assimp#building)

#### Ubuntu (Linux base Debian)
Pour ubuntu il suffit d'installer les paquets `libsdl2-dev` et `libassimp-dev`.
#### Windows
SDL et Assimp proposent des versions déjà compilées ainsi que des options de compilation pour Windows.
- [SDL Installation Windows](https://wiki.libsdl.org/Installation#Windows_XP.2FVista.2F7)
- [Assimp Compilation Windows](https://github.com/assimp/assimp#building)

## Compilation
### Ubuntu (Linux base Debian)
Un makefile est inclus dans le projet, il suffit d'utiliser la commande `make all` pour compiler le moteur.

Vu le nombre important de fichiers à compiler, une commande du type `make all -j4` pour répartir la compilation sur 4 threads est préférable.

## Lancement
### Pour créer une nouvelle Map
`./VoxEngine new <mapMame> <biomeName>`

### Pour charger une Map
`./VoxEngine load <mapName>`

### Nom des biomes actuels : 
`standard, prairie, valley, snowPrairie, snowValley, desert, archipel, mangrove`

### Visualisation de plusieurs map
L'option multi permet de visualiser jusqu'à 4 map de biomes différents en même temps.
#### Exemple :
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

