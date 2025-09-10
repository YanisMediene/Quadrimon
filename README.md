# Quadrimon 🎮

Quadrimon est un jeu de cartes interactif développé en C++ avec Qt et OpenGL, intégrant la reconnaissance d'images par webcam pour une expérience de jeu immersive.

## 📋 Table des matières
- [Description](#description)
- [Fonctionnalités](#fonctionnalités)
- [Architecture](#architecture)
- [Technologies utilisées](#technologies-utilisées)
- [Installation](#installation)
- [Utilisation](#utilisation)
- [Structure du projet](#structure-du-projet)
- [Auteurs](#auteurs)
- [Limitations connues](#limitations-connues)

## 🎯 Description

Quadrimon est un jeu de combat de créatures virtuelles où les joueurs utilisent des cartes physiques capturées par webcam pour invoquer et combattre avec des Quadrimons. Le jeu propose une arène 3D avec des animations et des effets visuels, créant une expérience unique mêlant monde physique et virtuel.

## ✨ Fonctionnalités

### 🎮 Gameplay
- **Gestion complète de partie** : Chargement des Quadrimons, phases d'interruption, combats, gestion de la victoire
- **Système de tours** : Alternance entre les joueurs avec différentes phases de jeu
- **Interface intuitive** : Contrôles via boutons à l'écran ou raccourcis clavier

### 🎨 Affichage 3D
- **Arène OpenGL** : Environnement 3D divisé en deux zones de combat
- **Quadrimons animés** : Créatures avec formes, couleurs et textures uniques selon leur type
- **Animations complètes** :
  - Animation d'attaque
  - Animation de réception de dégâts
  - Animation de mort
  - Animation de victoire
  - Animation au repos

### 🎯 Types de Quadrimons
- **Feu** 🔥 : Quadrimons de type offensif
- **Eau** 💧 : Quadrimons équilibrés
- **Plante** 🌱 : Quadrimons défensifs

### 📷 Reconnaissance d'images
- **Capture webcam** : Affichage en temps réel de la webcam
- **Reconnaissance de cartes** : Détection et identification automatique des cartes physiques
- **Affichage des informations** : Statistiques des cartes reconnues affichées à l'écran

### 🗺️ Système de terrains
- **Capsules de terrain** : Différents environnements (grotte, toile d'araignée, etc.)
- **Effets visuels** : Arrière-plans et textures thématiques
- **Impact sur le gameplay** : Modification des capacités selon le terrain

## 🏗️ Architecture

Le projet suit une architecture **MVC (Modèle-Vue-Contrôleur)** :

### 📊 Modèle
- **`Quadrimon`** : Gestion des créatures (statistiques, types, animations)
- **`Joueur`** : Gestion des joueurs et de leurs Quadrimons
- **`Combat`** : Gestion de l'état du jeu et des tours
- **`Terrain`** : Gestion des zones de combat
- **`CapsuleTerrain`** : Gestion des différents environnements

### 🖼️ Vue
- **`MyGLWidget`** : Rendu OpenGL de l'arène, des Quadrimons et des terrains

### 🎛️ Contrôleur
- **`MainWindow`** : Interface principale et logique de jeu

## 🛠️ Technologies utilisées

- **C++** - Langage principal
- **Qt Framework** - Interface utilisateur et gestion des événements
- **OpenGL** - Rendu 3D et animations
- **OpenCV** - Traitement d'images et reconnaissance de cartes
- **QMake** - Système de build

## 📦 Installation

### Prérequis
```bash
# Ubuntu/Debian
sudo apt-get install qt5-default libopencv-dev libgl1-mesa-dev

# macOS (avec Homebrew)
brew install qt opencv

# Windows
# Installez Qt Creator avec Qt 5.x et OpenCV
```

### Compilation
```bash
# Cloner le repository
git clone https://github.com/YanisMediene/Quadrimon.git
cd Quadrimon/App

# Compiler avec qmake
qmake App.pro
make

# Ou ouvrir App.pro dans Qt Creator
```

## 🎯 Utilisation

### Lancement du jeu
```bash
./App  # Linux/macOS
# ou App.exe sur Windows
```

### Contrôles
- **Capture de carte** : Bouton "Capture" ou touche clavier
- **Validation** : Bouton "Validate" ou touche clavier
- **Fin de tour** : Bouton "End Turn" ou touche clavier

### Déroulement d'une partie
1. **Phase de capture** : Prenez en photo vos cartes avec la webcam
2. **Phase de déploiement** : Placez vos Quadrimons sur l'arène
3. **Phase de combat** : Les Quadrimons s'affrontent automatiquement
4. **Fin de tour** : Passez la main à l'adversaire

## 📁 Structure du projet

```
Quadrimon/
├── App/                          # Code source principal
│   ├── main.cpp                  # Point d'entrée
│   ├── mainwindow.h/.cpp         # Contrôleur principal
│   ├── myglwidget.h/.cpp         # Rendu OpenGL
│   ├── quadrimon.h/.cpp          # Classe Quadrimon
│   ├── joueur.h/.cpp             # Classe Joueur
│   ├── combat.h/.cpp             # Logique de combat
│   ├── terrain.h/.cpp            # Gestion des terrains
│   ├── capsuleterrain.h/.cpp     # Capsules de terrain
│   ├── mainwindow.ui             # Interface Qt
│   ├── Ressource.qrc             # Ressources Qt
│   └── App.pro                   # Fichier de projet Qt
├── Cards/                        # Images des cartes
│   ├── card*.png                 # Cartes de jeu
│   └── infocard*.png            # Informations des cartes
├── Textures/                     # Textures 3D
│   ├── texture*.jpeg            # Textures des Quadrimons
│   ├── backgroundtexture*.jpeg  # Arrière-plans
│   ├── fire.jpeg                # Texture type Feu
│   ├── water.jpeg              # Texture type Eau
│   └── plant.jpeg              # Texture type Plante
├── Diagramme_classes_Mohaman-Bello_Mediene.drawio.png
├── RAPPORT PROJET QUADRIMON.pdf
└── README.md
```

## 👥 Auteurs

- **Yanis MEDIENE** - Développement principal, interface, OpenGL, logique de jeu, reconnaissance d'images

## ⚠️ Limitations connues

### Problèmes d'affichage
- Certains Quadrimons peuvent assombrir la scène (ex: Olaf)
- Performances ralenties sur certains environnements Windows
- Animations parfois lentes selon la configuration

### Reconnaissance d'images
- Efficacité non optimale à 100%
- Difficultés avec les images comportant beaucoup de reflets
- Problèmes quand le nom de la carte n'est pas visible

### Solutions recommandées
- Utiliser un éclairage uniforme pour la capture de cartes
- Éviter les reflets sur les cartes
- S'assurer que le nom de la carte est visible
- Utiliser un environnement Linux/macOS pour de meilleures performances

## 📄 Licence

Ce projet a été réalisé dans le cadre d'un projet académique.

## 🔗 Liens utiles

- [Documentation Qt](https://doc.qt.io/)
- [Documentation OpenCV](https://docs.opencv.org/)
- [Documentation OpenGL](https://www.opengl.org/documentation/)

---

*Projet réalisé dans le cadre d'un cours de programmation orientée objet*
