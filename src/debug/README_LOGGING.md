# Guide d'utilisation des logs de debug

## ValueTreeLogger - Niveaux de verbosité

Le système de logging pour les ValueTree dispose de 3 niveaux de verbosité configurables:

### 1. **MINIMAL** - Logs critiques uniquement
- Affiche seulement les ajouts/suppressions d'enfants dans le ValueTree
- Idéal pour suivre la structure sans être noyé dans les détails
- Ignorer tous les changements de propriétés

**Exemple de sortie:**
```
[Piece Model State] +Enfant: Section
[Piece Model State] -Enfant: Section (index: 0)
```

### 2. **NORMAL** (par défaut) - Équilibre entre détail et lisibilité
- Affiche les changements de propriétés importantes
- Affiche les ajouts/suppressions d'enfants
- Pas de dumps XML automatiques
- Format concis sur une ligne

**Exemple de sortie:**
```
[Piece Model State] testProperty = Hello from Piece!
[Piece Model State] +Enfant: Section
[Selection Context State] selectionType = Section
[Selection Context State] selectedElementId = Section_0
[Selection Context State] Selection: Section #0
```

### 3. **VERBOSE** - Tous les détails + XML
- Affiche tout ce que NORMAL affiche
- Ajoute des dumps XML complets du ValueTree après chaque changement
- Ajoute des résumés détaillés de l'état
- Utile pour débugger des problèmes complexes

**Exemple de sortie:**
```
[Piece Model State] tonalityNote = 5

=== ETAT PIECE (XML) ===
<?xml version="1.0" encoding="UTF-8"?>
<Section id="0" name="Nouvelle Section" tonalityNote="5" ...>
  <Progression id="0"/>
</Section>
--- Resume: 1 enfant(s), 5 propriete(s) [Progression]
========================
```

## Configuration du niveau de log

### Option 1: Niveau global (recommandé)
Dans votre fichier de build ou au début de `ValueTreeLogger.h`, définissez:

```cpp
// Pour activer le mode MINIMAL
#define VALUE_TREE_LOG_LEVEL LogLevel::MINIMAL

// Pour activer le mode NORMAL (défaut si non défini)
#define VALUE_TREE_LOG_LEVEL LogLevel::NORMAL

// Pour activer le mode VERBOSE
#define VALUE_TREE_LOG_LEVEL LogLevel::VERBOSE
```

### Option 2: Niveau par logger
Lors de la création d'un logger, passez le niveau souhaité:

```cpp
// Logger minimal pour l'UI State (peu intéressant en général)
auto uiLogger = std::make_unique<ValueTreeLogger>("UI State", LogLevel::MINIMAL);

// Logger normal pour la Piece (bon équilibre)
auto pieceLogger = std::make_unique<ValueTreeLogger>("Piece Model State", LogLevel::NORMAL);

// Logger verbose pour debug intensif d'une section spécifique
auto sectionLogger = std::make_unique<ValueTreeLogger>("Section Debug", LogLevel::VERBOSE);
```

### Option 3: Changement à la volée
Vous pouvez changer le niveau d'un logger existant:

```cpp
myLogger->setLogLevel(LogLevel::VERBOSE);  // Activer le mode verbose temporairement
// ... faire des tests ...
myLogger->setLogLevel(LogLevel::NORMAL);   // Revenir au mode normal
```

## Logs dans les composants UI

Les logs dans les composants UI (Zone1, Zone2, SectionEditor, etc.) ont été commentés par défaut.

Pour les activer temporairement pendant le debug:
1. Décommentez les lignes `// DBG(...)` dans le fichier concerné
2. Recompilez
3. Recommentez après le debug

**Exemple dans Zone1.cpp:**
```cpp
// Log optionnel: décommenter si nécessaire pour le debug
// DBG("[Zone1] Note de base " << noteName.toStdString() << " selectionnee");
```

## Recommandations

### Pour le développement quotidien:
- Utilisez `LogLevel::NORMAL` pour le ValueTreeLogger
- Gardez les logs UI commentés

### Pour débugger un problème spécifique:
1. Activez `LogLevel::VERBOSE` sur le logger concerné
2. Décommentez les logs UI pertinents
3. Reproduisez le problème
4. Analysez les logs
5. Repassez en mode `NORMAL` et recommentez les logs UI

### Pour une démo ou présentation:
- Utilisez `LogLevel::MINIMAL` pour éviter le bruit
- Tous les logs UI commentés

## Format des logs

Tous les logs suivent maintenant un format cohérent:
```
[Nom du composant] Action: détails
```

Exemples:
- `[Piece Model State] tonalityNote = 5`
- `[SectionEditor] syncZones: currentSectionState INVALIDE`
- `[Zone1] setSelectedBaseNote: note=5`

Ce format facilite le filtrage et la recherche dans les logs.

