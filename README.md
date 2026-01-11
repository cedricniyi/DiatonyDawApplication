# DiatonyDaw : un plugin pour DAW utilisant Diatony, un modèle d'harmonie tonale

Ce repository contient le code source d'un plugin Audio Unit (AU) permettant d'utiliser le modèle [Diatony](https://github.com/sprockeelsd/Diatony) directement dans un Digital Audio Workstation tel que Ableton Live, Logic Pro ou GarageBand.
Ce plugin fournit une interface graphique permettant de générer des progressions harmoniques à quatre voix en respectant les règles de la musique tonale.

Ce projet utilise principalement le framework JUCE ([Github](https://github.com/juce-framework/JUCE) / [Website](https://juce.com/)) et bien évidemment le module Diatony.

Ce projet est développé dans le cadre de mon mémoire de fin d'études à l'Université Catholique de Louvain (2026) sous la direction de Peter Van Roy et Damien Sprockeels. La thèse complète est disponible à la lecture [ici](https://thesis.dial.uclouvain.be/server/api/uclouvain/bitstream/44fd6788-00af-403b-a1cb-39f00e90b148/content?hash=8e9cef596df700de7f1e90ba08abce5fbbded38d9c254ee4e518ddab226414ee64619f4b2a193cc593ca12f92457bef134662dd94f4af88a908bcad84c8cb3c083577ff36fd1934d5c4596db4f6d5b7ab1c546be22f2ec365ac8f4ad2d65a64dc3f782d77a470502800b51108bc95d31).

## Contenu
Le projet contient : le code source de l'interface, les dépendances nécessaires gérées sous forme de [sous-modules](https://github.com/cedricniyi/DiatonyDawApplication/blob/main/.gitmodules) (Diatony et JUCE).

Actuellement, le plugin est seulement fonctionnel sous macOS ARM (version macOS Intel disponible bientôt) et sous la version Audio Unit (et Standalone). La version VST3 n'est pas encore disponible en raison de défauts de compilation.


### Code Source

Le dossier [**src**](https://github.com/cedricniyi/DiatonyDawApplication/tree/main/src) contient le code source du plugin.

Les dossiers sont organisés en fonction du rôle de chacun des fichiers : 

- [**src/ui/**](https://github.com/cedricniyi/DiatonyDawApplication/tree/main/src/ui) : 
Contient les différents composants JUCE qui m'ont permis de construire l'interface utilisateur.

- [**src/controller/**](https://github.com/cedricniyi/DiatonyDawApplication/tree/main/src/controller) : 
Couche faisant le pont entre le code de l'interface graphique et la couche modèle et service.

- [**src/model/**](https://github.com/cedricniyi/DiatonyDawApplication/tree/main/src/model) :
Groupe de classes définissant les objets métiers manipulés par l'utilisateur sur l'interface (ex : une modulation, une progression, un accord, etc.) avec leur logique implémentée.

- [**src/services/**](https://github.com/cedricniyi/DiatonyDawApplication/tree/main/src/services) :
Seule partie du code source de l'interface ayant une dépendance forte avec le modèle Diatony se trouvant également dans le repository ([Diatony [fork]](https://github.com/cedricniyi/Diatony)).

- [**src/utils/**](https://github.com/cedricniyi/DiatonyDawApplication/tree/main/src/utils) :
Classes utilitaires permettant de gérer la sauvegarde de fichiers et les polices d'écriture chargées à l'écran. Contient également les assets SVG et polices d'écriture utilisés dans le plugin.

### Autres fichiers

- [**Papers/**](https://github.com/cedricniyi/DiatonyDawApplication/tree/main/Papers) :
Ce dossier contient l'ensemble des articles scientifiques m'ayant guidé durant le développement et l'écriture de ma thèse.

- [**modules/**](https://github.com/cedricniyi/DiatonyDawApplication/tree/main/modules) :
Contient l'ensemble des sources JUCE nécessaires au projet.

- [**CMakeLists.txt**](https://github.com/cedricniyi/DiatonyDawApplication/tree/main/CMakeLists.txt) :
Fichier principal de configuration CMake définissant les cibles, dépendances et options de compilation du plugin.

- [**CMakePresets.json**](https://github.com/cedricniyi/DiatonyDawApplication/tree/main/CMakePresets.json) :
Presets de build préconfigurés pour macOS ARM (debug, release) utilisant Ninja comme générateur.


- [**TestDiatonyDawApp Project/**](https://github.com/cedricniyi/DiatonyDawApplication/tree/main/TestDiatonyDawApp%20Project) :
Projet d'expérimentation personnel (Ableton Live 11).

## Prérequis (développement)

Pour développer ce projet, j'ai utilisé macOS (Apple Silicon / ARM64) comme système d'exploitation.

### Outils de compilation nécessaires

- **CMake** (>= 3.22, pour ma part j'ai utilisé Homebrew pour installer la dépendance. Version utilisée dernièrement : **3.30.5**)

- **Ninja**, système de build rapide utilisé par CMake pour la compilation.

### Dépendances externes

- [Gecode](https://formulae.brew.sh/formula/gecode), installé à l'aide de Homebrew (`brew install gecode`) car les sources disponibles via le site de Gecode ciblent une architecture Intel de macOS. Gecode est la bibliothèque utilisée par le modèle Diatony. (Plus d'informations : [*Gecode Documentation*](https://www.gecode.dev/documentation.html))

- [JUCE](https://github.com/juce-framework/JUCE/tree/51a8a6d7aeae7326956d747737ccf1575e61e209) **>= 8.0.3**, la version 8.0.3 est celle incluse comme sous-module Git au projet.

- [Diatony](https://github.com/cedricniyi/Diatony), module conçu par Damien Sprockeels, inclus comme sous-module Git au projet.

### Installation des dépendances
```
brew install cmake ninja gecode
```

### Assets
- San Francisco Pro, police d'écriture conçue par Apple. (Plus d'informations : [*Fonts for Apple platforms*](https://developer.apple.com/fonts/))

## Compilation

### Cloner le repository avec sous-modules

```bash
git clone --recursive https://github.com/cedricniyi/DiatonyDawApplication.git
cd DiatonyDawApplication
```

Si vous avez cloné sans prendre en compte récursivement les sous-modules, utilisez cette commande :
```bash
git submodule update --init --recursive
```

### Compiler toutes les cibles définies

```
cmake --preset debug
cmake --build cmake-build-debug
```

Cette commande produit 2 exécutables du plugin :
- la version **Standalone**, `DiatonyDawApplication.app`, utilisée pour le développement rapide de l'interface
- la version **Audio Unit**, `DiatonyDawApplication.component`, version utilisable dans un DAW

**_Remarque_** : Pour le moment, ces 2 versions nécessitent que vous ayez une machine sous macOS ARM (puce M1 ou plus récente).

Après compilation, vous serez en mesure de lancer les tests que j'ai écrits pour vérifier ma couche Model, Controller ainsi que mon service de génération :

```bash
DYLD_LIBRARY_PATH=/opt/homebrew/opt/gecode/lib ./cmake-build-debug/DiatonyTests_artefacts/Debug/DiatonyTests
```

Si vous avez une version de Gecode installée via Homebrew, le chemin spécifié devrait être bon. Sinon, à vous de modifier le PATH vers la bonne destination.

## Remarques

Concernant l'UI, actuellement le code du footer est présent mais non utilisé dans la version compilée et téléchargeable.
