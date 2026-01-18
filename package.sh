#!/bin/bash

VERSION="v1.0.1"

# ========================================
# PARAMÈTRE : Architecture (arm ou intel)
# ========================================
ARCH="${1:-arm}"

case "$ARCH" in
    arm|ARM|arm64)
        PLATFORM="macOS-ARM64"
        GECODE_LIB_DIR="/opt/homebrew/opt/gecode/lib"
        DIATONY_DYLIB="${PWD}/Diatony/out/diatony.dylib"
        DIATONY_DYLIB_NAME="diatony.dylib"
        PLATFORM_DESC="macOS ARM64 (M1/M2/M3)"
        # Détecte automatiquement debug ou release pour ARM
        if [ -d "cmake-build-release/DiatonyDawApplication_artefacts/Release" ]; then
            BUILD_DIR="cmake-build-release/DiatonyDawApplication_artefacts/Release"
            BUILD_TYPE="Release"
        elif [ -d "cmake-build-debug/DiatonyDawApplication_artefacts/Debug" ]; then
            BUILD_DIR="cmake-build-debug/DiatonyDawApplication_artefacts/Debug"
            BUILD_TYPE="Debug"
        else
            echo "❌ Aucun build ARM trouvé."
            echo "   Compilez d'abord avec: cmake --preset release && cmake --build cmake-build-release"
            exit 1
        fi
        ;;
    intel|INTEL|x86_64|x64)
        PLATFORM="macOS-Intel"
        GECODE_LIB_DIR="${PWD}/Diatony/gecode-dylibs"
        DIATONY_DYLIB="${PWD}/Diatony/out/diatony-intel.dylib"
        DIATONY_DYLIB_NAME="diatony-intel.dylib"
        PLATFORM_DESC="macOS Intel (x86_64)"
        # Détecte automatiquement debug ou release pour Intel
        if [ -d "cmake-build-release-intel/DiatonyDawApplication_artefacts/Release" ]; then
            BUILD_DIR="cmake-build-release-intel/DiatonyDawApplication_artefacts/Release"
            BUILD_TYPE="Release"
        elif [ -d "cmake-build-debug-intel/DiatonyDawApplication_artefacts/Debug" ]; then
            BUILD_DIR="cmake-build-debug-intel/DiatonyDawApplication_artefacts/Debug"
            BUILD_TYPE="Debug"
        else
            echo "❌ Aucun build Intel trouvé."
            echo "   Compilez d'abord avec: cmake --preset release-intel && cmake --build cmake-build-release-intel"
            exit 1
        fi
        ;;
    *)
        echo "❌ Architecture inconnue: $ARCH"
        echo "   Usage: $0 [arm|intel]"
        echo "   Exemples:"
        echo "     $0 arm    → Package pour Mac ARM (M1/M2/M3)"
        echo "     $0 intel  → Package pour Mac Intel"
        exit 1
        ;;
esac

echo "📦 Packaging ${PLATFORM} depuis ${BUILD_TYPE}..."
echo "   GECODE_LIB_DIR: $GECODE_LIB_DIR"
echo "   DIATONY_DYLIB: $DIATONY_DYLIB"
echo ""

# Vérifier que la dylib Diatony existe
if [ ! -f "$DIATONY_DYLIB" ]; then
    echo "❌ Dylib Diatony non trouvée: $DIATONY_DYLIB"
    echo "   Compilez d'abord avec make dans Diatony/c++/"
    exit 1
fi

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
# FONCTION: Signer un bundle
# ========================================
# Note: Les dylibs et rpaths sont déjà configurés par CMakeLists.txt (POST_BUILD)
# Cette fonction ne fait que re-signer le bundle après copie
sign_bundle() {
    local BUNDLE_PATH="$1"
    
    echo "  🔐 Signature du bundle..."
    codesign --force --deep --sign - "$BUNDLE_PATH"
    
    echo "  ✅ Bundle signé avec succès"
}

# ========================================
# FONCTION: Vérifier la qualité du bundle
# ========================================
verify_bundle() {
    local BUNDLE_PATH="$1"
    local BINARY_PATH="$2"
    local BUNDLE_NAME="$3"
    
    echo "  🔍 Vérification du bundle ${BUNDLE_NAME}..."
    
    local ERRORS=0
    
    # 1. Vérifier que diatony.dylib est présent
    if [ ! -f "$BUNDLE_PATH/Contents/Frameworks/${DIATONY_DYLIB_NAME}" ]; then
        echo "  ❌ ERREUR: ${DIATONY_DYLIB_NAME} manquant dans Frameworks/"
        ERRORS=$((ERRORS + 1))
    else
        echo "  ✅ ${DIATONY_DYLIB_NAME} présent"
    fi
    
    # 2. Vérifier que toutes les dylibs Gecode sont présentes
    for lib in "${GECODE_LIBS[@]}"; do
        if [ ! -f "$BUNDLE_PATH/Contents/Frameworks/$lib" ]; then
            echo "  ❌ ERREUR: $lib manquant dans Frameworks/"
            ERRORS=$((ERRORS + 1))
        fi
    done
    if [ $ERRORS -eq 0 ]; then
        echo "  ✅ Toutes les dylibs Gecode présentes (9/9)"
    fi
    
    # 3. Vérifier que le binaire n'a PAS de chemins absolus (seulement @rpath)
    echo "  🔍 Vérification des chemins dans le binaire..."
    
    # Chercher des chemins absolus problématiques (pas @rpath, @loader_path, @executable_path, ou frameworks système)
    BAD_PATHS=$(otool -L "$BINARY_PATH" 2>/dev/null | grep -E "^\s+/" | grep -v "@rpath" | grep -v "@loader_path" | grep -v "@executable_path" | grep -v "/System/" | grep -v "/usr/lib/" | grep -E "(gecode|diatony)" || true)
    
    if [ -n "$BAD_PATHS" ]; then
        echo "  ❌ ERREUR: Chemins absolus détectés dans le binaire:"
        echo "$BAD_PATHS"
        ERRORS=$((ERRORS + 1))
    else
        echo "  ✅ Binaire: toutes les références Gecode/Diatony utilisent @rpath"
    fi
    
    # 4. Vérifier les dylibs Gecode (inter-références entre dylibs BUNDLÉES uniquement)
    # On ne vérifie que les références vers les dylibs qu'on bundle (pas gecodegist, etc.)
    echo "  🔍 Vérification des inter-dépendances entre dylibs bundlées..."
    
    # Liste des noms de dylibs bundlées (sans le chemin)
    BUNDLED_LIBS=("diatony" "gecodedriver" "gecodeflatzinc" "gecodefloat" "gecodeint" "gecodekernel" "gecodeminimodel" "gecodesearch" "gecodeset" "gecodesupport")
    
    for lib in "${GECODE_LIBS[@]}"; do
        LIB_PATH="$BUNDLE_PATH/Contents/Frameworks/$lib"
        if [ -f "$LIB_PATH" ]; then
            # Pour chaque dylib bundlée, vérifier que ses références vers NOS dylibs utilisent @rpath
            for bundled in "${BUNDLED_LIBS[@]}"; do
                # Chercher des chemins absolus vers cette dylib bundlée
                BAD_REF=$(otool -L "$LIB_PATH" 2>/dev/null | grep -E "^\s+/" | grep -v "@rpath" | grep -v "@loader_path" | grep "$bundled" || true)
                if [ -n "$BAD_REF" ]; then
                    echo "  ❌ ERREUR: $lib a un chemin absolu vers $bundled:"
                    echo "$BAD_REF"
                    ERRORS=$((ERRORS + 1))
                fi
            done
        fi
    done
    
    if [ $ERRORS -eq 0 ]; then
        echo "  ✅ Dylibs bundlées: toutes les inter-dépendances utilisent @rpath"
    fi
    
    # Résumé
    echo ""
    if [ $ERRORS -gt 0 ]; then
        echo "  ⚠️  ATTENTION: $ERRORS problème(s) détecté(s) dans ${BUNDLE_NAME}"
        echo "  Le bundle pourrait ne pas fonctionner sur d'autres machines."
        return 1
    else
        echo "  ✅ ${BUNDLE_NAME} vérifié - QUALITÉ OK"
        return 0
    fi
}

# ========================================
# PACKAGE 1 : Audio Unit
# ========================================

AU_NAME="DiatonyDawApplication-${VERSION}-${PLATFORM}-AU"
mkdir -p "$AU_NAME"

echo "📂 Copie Audio Unit..."
cp -r "$BUILD_DIR/AU/DiatonyDawApplication.component" "$AU_NAME/"

echo "🔧 Signature du bundle Audio Unit..."
sign_bundle "$AU_NAME/DiatonyDawApplication.component"

echo "🔍 Vérification de la qualité du bundle Audio Unit..."
verify_bundle "$AU_NAME/DiatonyDawApplication.component" "$AU_NAME/DiatonyDawApplication.component/Contents/MacOS/DiatonyDawApplication" "Audio Unit"

# README pour Audio Unit
cat > "$AU_NAME/README.txt" << EOF
DiatonyDawApplication - Audio Unit Plugin
==========================================

Version: ${VERSION}
Platform: ${PLATFORM_DESC}

PREMIÈRE INSTALLATION
----------------------------

1. Copier DiatonyDawApplication.component vers :
   ~/Library/Audio/Plug-Ins/Components/

   Commande Terminal :
   cp -r DiatonyDawApplication.component ~/Library/Audio/Plug-Ins/Components/

2. Redémarrer votre DAW (Ableton Live, Logic Pro, GarageBand, etc.)

3. Rescanner les plugins dans votre DAW (si vous n'apercevez pas le plugin)

4. Le plugin devrait apparaître dans la liste des Audio Units
   sous le nom "DiatonyDawApplication"

MISE À JOUR / RÉINSTALLATION
----------------------------

1. Fermer votre DAW complètement

2. Supprimer l'ancienne version :
   rm -rf ~/Library/Audio/Plug-Ins/Components/DiatonyDawApplication.component

3. (Optionnel) Vider le cache Audio Unit de macOS :
   rm -rf ~/Library/Caches/AudioUnitCache/

4. Copier la nouvelle version :
   cp -r DiatonyDawApplication.component ~/Library/Audio/Plug-Ins/Components/

5. Redémarrer votre DAW

DÉPANNAGE
---------

Si le plugin n'apparaît pas ou si vous voyez une ancienne version :
- Vérifiez qu'il n'y a qu'UNE seule copie du plugin installée
- Supprimez le cache : rm -rf ~/Library/Caches/AudioUnitCache/
- Redémarrez votre DAW

SUPPORT
-------
https://github.com/cedricniyi/DiatonyDawApplication

INFORMATIONS
------------
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

# Note: Le Standalone a déjà les dylibs copiées par CMake (POST_BUILD)
echo "🔧 Vérification du bundle Standalone..."
STANDALONE_FRAMEWORKS="$STANDALONE_NAME/DiatonyDawApplication.app/Contents/Frameworks"
if [ ! -f "$STANDALONE_FRAMEWORKS/${DIATONY_DYLIB_NAME}" ]; then
    echo "  ❌ ERREUR: Dylibs manquantes dans le bundle Standalone"
    echo "     Le build CMake n'a pas correctement bundlé les dépendances."
    echo "     Recompilez avec: cmake --build cmake-build-release"
    exit 1
fi

echo "🔧 Signature du bundle Standalone..."
sign_bundle "$STANDALONE_NAME/DiatonyDawApplication.app"

echo "🔍 Vérification de la qualité du bundle Standalone..."
verify_bundle "$STANDALONE_NAME/DiatonyDawApplication.app" "$STANDALONE_NAME/DiatonyDawApplication.app/Contents/MacOS/DiatonyDawApplication" "Standalone"

# README pour Standalone
cat > "$STANDALONE_NAME/README.txt" << EOF
DiatonyDawApplication - Standalone Application
===============================================

Version: ${VERSION}
Platform: ${PLATFORM_DESC}

INSTALLATION
------------

Option 1 : Exécution directe
   Double-cliquer sur DiatonyDawApplication.app

Option 2 : Installation dans Applications
   Déplacer DiatonyDawApplication.app vers /Applications/
   
   Commande Terminal :
   cp -r DiatonyDawApplication.app /Applications/

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
------------
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
echo "📋 Résumé des packages créés (${PLATFORM}):"
ls -lh "${AU_NAME}.zip"
ls -lh "${STANDALONE_NAME}.zip"

echo ""
echo "🔍 Vérification des bundles (pour debug) :"
echo "   Pour vérifier le contenu du zip AU :"
echo "   unzip -l ${AU_NAME}.zip | grep Frameworks"
