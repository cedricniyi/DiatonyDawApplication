#!/bin/bash

VERSION="v1.0.1"
PLATFORM="macOS-ARM64"
GECODE_LIB_DIR="/opt/homebrew/opt/gecode/lib"
DIATONY_DYLIB="${PWD}/Diatony/out/diatony.dylib"

# Détecte automatiquement debug ou release
if [ -d "cmake-build-release/DiatonyDawApplication_artefacts/Release" ]; then
    BUILD_DIR="cmake-build-release/DiatonyDawApplication_artefacts/Release"
    BUILD_TYPE="Release"
elif [ -d "cmake-build-debug/DiatonyDawApplication_artefacts/Debug" ]; then
    BUILD_DIR="cmake-build-debug/DiatonyDawApplication_artefacts/Debug"
    BUILD_TYPE="Debug"
else
    echo "❌ Aucun build trouvé. Vérifier les chemins."
    exit 1
fi

echo "📦 Packaging depuis ${BUILD_TYPE}..."

# Liste des dylibs Gecode
GECODE_LIBS=(
    "libgecodedriver.49.dylib"
    "libgecodeflatzinc.49.dylib"
    "libgecodefloat.49.dylib"
    "libgecodeint.49.dylib"
    "libgecodekernel.49.dylib"
    "libgecodeminimodel.49.dylib"
    "libgecodesearch.49.dylib"
    "libgecodeset.49.dylib"
    "libgecodesupport.49.dylib"
)

# ========================================
# FONCTION: Préparer un bundle avec dylibs
# ========================================
prepare_bundle() {
    local BUNDLE_PATH="$1"
    local BINARY_PATH="$2"
    
    echo "  📂 Création du dossier Frameworks..."
    mkdir -p "$BUNDLE_PATH/Contents/Frameworks"
    
    # Copier diatony.dylib
    echo "  📦 Copie de diatony.dylib..."
    cp "$DIATONY_DYLIB" "$BUNDLE_PATH/Contents/Frameworks/"
    
    # Copier les dylibs Gecode
    echo "  📦 Copie des dylibs Gecode..."
    for lib in "${GECODE_LIBS[@]}"; do
        cp "$GECODE_LIB_DIR/$lib" "$BUNDLE_PATH/Contents/Frameworks/"
    done
    
    # Corriger les références dans le binaire principal
    echo "  🔧 Correction des références dans le binaire..."
    for lib in "${GECODE_LIBS[@]}"; do
        install_name_tool -change "$GECODE_LIB_DIR/$lib" "@rpath/$lib" "$BINARY_PATH" 2>/dev/null
    done
    
    # Corriger les RPATHs des dylibs Gecode (inter-dépendances)
    echo "  🔧 Correction des RPATHs des dylibs Gecode..."
    
    # Trouver le chemin Cellar réel (le symlink opt pointe vers Cellar)
    GECODE_CELLAR_DIR=$(readlink -f "$GECODE_LIB_DIR" 2>/dev/null || echo "$GECODE_LIB_DIR")
    
    for lib in "${GECODE_LIBS[@]}"; do
        LIB_PATH="$BUNDLE_PATH/Contents/Frameworks/$lib"
        
        # Changer son propre ID
        install_name_tool -id "@rpath/$lib" "$LIB_PATH" 2>/dev/null
        
        # Changer les références vers d'autres dylibs Gecode (tous les chemins possibles)
        for dep_lib in "${GECODE_LIBS[@]}"; do
            # Chemin via /opt/homebrew/opt/
            install_name_tool -change "$GECODE_LIB_DIR/$dep_lib" "@rpath/$dep_lib" "$LIB_PATH" 2>/dev/null
            # Chemin via /opt/homebrew/Cellar/ (le vrai chemin)
            install_name_tool -change "$GECODE_CELLAR_DIR/$dep_lib" "@rpath/$dep_lib" "$LIB_PATH" 2>/dev/null
            # Chemin hardcodé au cas où (version spécifique)
            install_name_tool -change "/opt/homebrew/Cellar/gecode/6.2.0_1/lib/$dep_lib" "@rpath/$dep_lib" "$LIB_PATH" 2>/dev/null
        done
    done
    
    # Re-signer le bundle
    echo "  🔐 Signature du bundle..."
    codesign --force --deep --sign - "$BUNDLE_PATH"
    
    echo "  ✅ Bundle préparé avec succès"
}

# ========================================
# PACKAGE 1 : Audio Unit
# ========================================

AU_NAME="DiatonyDawApplication-${VERSION}-${PLATFORM}-AU"
mkdir -p "$AU_NAME"

echo "📂 Copie Audio Unit..."
cp -r "$BUILD_DIR/AU/DiatonyDawApplication.component" "$AU_NAME/"

echo "🔧 Préparation du bundle Audio Unit avec les dépendances..."
prepare_bundle "$AU_NAME/DiatonyDawApplication.component" "$AU_NAME/DiatonyDawApplication.component/Contents/MacOS/DiatonyDawApplication"

# README pour Audio Unit
cat > "$AU_NAME/README.txt" << 'EOF'
DiatonyDawApplication - Audio Unit Plugin
==========================================

Version: v1.0.1
Platform: macOS ARM64 (M1/M2/M3)

INSTALLATION
------------

1. Copier DiatonyDawApplication.component vers :
   /Library/Audio/Plug-Ins/Components/

   Commande Terminal :
   sudo cp -r DiatonyDawApplication.component /Library/Audio/Plug-Ins/Components/

2. Redémarrer votre DAW (Ableton Live, Logic Pro, GarageBand, etc.)

3. Rescanner les plugins dans votre DAW

4. Le plugin devrait apparaître dans la liste des Audio Units
   sous le nom "DiatonyDawApplication"

SUPPORT
-------
https://github.com/cedricniyi/DiatonyDawApplication

INFORMATIONS
-------
Développé dans le cadre d'un TFE à l'UCLouvain (2026)
EOF

# Zipper Audio Unit
echo "🗜️  Compression Audio Unit..."
zip -r "${AU_NAME}.zip" "$AU_NAME" > /dev/null
rm -rf "$AU_NAME"
echo "✅ Package Audio Unit créé: ${AU_NAME}.zip"

# ========================================
# PACKAGE 2 : Standalone
# ========================================

STANDALONE_NAME="DiatonyDawApplication-${VERSION}-${PLATFORM}-Standalone"
mkdir -p "$STANDALONE_NAME"

echo "📂 Copie Standalone..."
cp -r "$BUILD_DIR/Standalone/DiatonyDawApplication.app" "$STANDALONE_NAME/"

# Note: Le Standalone a déjà les dylibs copiées par CMake, mais on vérifie/recopie pour être sûr
echo "🔧 Vérification/Préparation du bundle Standalone..."
STANDALONE_FRAMEWORKS="$STANDALONE_NAME/DiatonyDawApplication.app/Contents/Frameworks"
if [ ! -f "$STANDALONE_FRAMEWORKS/diatony.dylib" ]; then
    echo "  ⚠️  Dylibs manquantes, ajout..."
    prepare_bundle "$STANDALONE_NAME/DiatonyDawApplication.app" "$STANDALONE_NAME/DiatonyDawApplication.app/Contents/MacOS/DiatonyDawApplication"
else
    echo "  ✅ Les dylibs sont déjà présentes"
    # Re-signer quand même
    codesign --force --deep --sign - "$STANDALONE_NAME/DiatonyDawApplication.app"
fi

# README pour Standalone
cat > "$STANDALONE_NAME/README.txt" << 'EOF'
DiatonyDawApplication - Standalone Application
===============================================

Version: v1.0.1
Platform: macOS ARM64 (M1/M2/M3)

INSTALLATION
------------

Option 1 : Exécution directe
   Double-cliquer sur DiatonyDawApplication.app

Option 2 : Installation dans Applications
   Déplacer DiatonyDawApplication.app vers /Applications/
   
   Commande Terminal :
   sudo cp -r DiatonyDawApplication.app /Applications/

PREMIÈRE UTILISATION
--------------------

Au premier lancement, macOS peut afficher un message de sécurité.
Si c'est le cas :
1. Clic droit sur DiatonyDawApplication.app
2. Sélectionner "Ouvrir"
3. Confirmer l'ouverture

UTILISATION
-----------
L'application standalone fonctionne de manière indépendante,
sans nécessiter de DAW. Idéale pour :
- Expérimentation rapide
- Génération de progressions harmoniques
- Export MIDI

SUPPORT
-------
https://github.com/cedricniyi/DiatonyDawApplication

INFORMATIONS
-------
Développé dans le cadre d'un TFE à l'UCLouvain (2026)
EOF

# Zipper Standalone
echo "🗜️  Compression Standalone..."
zip -r "${STANDALONE_NAME}.zip" "$STANDALONE_NAME" > /dev/null
rm -rf "$STANDALONE_NAME"
echo "✅ Package Standalone créé: ${STANDALONE_NAME}.zip"

# ========================================
# RÉSUMÉ
# ========================================

echo ""
echo "📋 Résumé des packages créés :"
ls -lh "${AU_NAME}.zip"
ls -lh "${STANDALONE_NAME}.zip"

echo ""
echo "🔍 Vérification des bundles (pour debug) :"
echo "   Pour vérifier le contenu du zip AU :"
echo "   unzip -l ${AU_NAME}.zip | grep Frameworks"
