# Composants Réutilisables Diatony

Ce dossier contient tous les composants UI réutilisables extraits de l'ancienne interface.

## 🎨 Base (`/base/`)

### `DiatonyLookAndFeel.h`
- **Usage** : Thème visuel complet de l'application
- **Fonctionnalités** : 
  - Couleurs cohérentes pour tous les composants JUCE
  - Styles personnalisés pour boutons, combo boxes, etc.
- **Utilisation** : `setLookAndFeel(&diatonyLookAndFeel)`

### `BaseZone.h`
- **Usage** : Classe de base pour créer des zones avec en-tête
- **Fonctionnalités** :
  - En-tête avec titre personnalisable
  - Méthodes virtuelles `paintContent()` et `resizeContent()`
  - Style uniforme avec coins arrondis
- **Utilisation** : Hériter de `BaseZone` et implémenter les méthodes virtuelles

### `OutlineTextButton.h`
- **Usage** : Bouton avec contour uniquement (sans remplissage)
- **Fonctionnalités** :
  - Couleurs personnalisables (texte + bordure)
  - Épaisseur de bordure configurable
  - Gestion automatique des états (hover, pressed, disabled)
- **Utilisation** : 
  ```cpp
  OutlineTextButton button("Texte", textColor, borderColor, thickness);
  ```

## 💬 Dialogs (`/dialogs/`)

### `ToastComponent.h`
- **Usage** : Notifications temporaires non-intrusives
- **Fonctionnalités** :
  - Positionnement automatique (coin supérieur droit)
  - Animation de disparition avec fondu
  - Durée configurable
- **Utilisation** :
  ```cpp
  toast->showMessage("Message", 2000); // 2 secondes
  ```

### `DiatonyAlertWindow.h`
- **Usage** : Fenêtres modales avec style personnalisé
- **Fonctionnalités** :
  - Méthode statique `show()` pour usage simple
  - Callback personnalisé au clic
  - Style cohérent avec l'application
- **Utilisation** :
  ```cpp
  DiatonyAlertWindow::show("Titre", "Message", "OK", callback);
  ```

## 📊 Panels (`/panels/`)

### `StatusPanel.h`
- **Usage** : Panneau de statut avec zones séparées
- **Fonctionnalités** :
  - Zone de statut de génération
  - Zone de statut de lecture
  - Auto-effacement des messages
- **Utilisation** :
  ```cpp
  statusPanel->setGenerationStatus("Génération terminée");
  statusPanel->setPlaybackStatus("En lecture");
  ```

## 🔧 Intégration

Pour utiliser ces composants dans votre nouvelle interface :

1. **Inclure le header** : `#include "../ui_reusable/path/Component.h"`
2. **Appliquer le thème** : `setLookAndFeel(&diatonyLookAndFeel)` dans votre constructeur
3. **Nettoyer** : `setLookAndFeel(nullptr)` dans votre destructeur

## 📁 Archive

L'ancienne interface complète est disponible dans `/src/ui_archive/` pour référence. 