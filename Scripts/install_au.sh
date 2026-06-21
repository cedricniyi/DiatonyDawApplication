#!/bin/bash
# Script pour installer l'Audio Unit depuis le dossier de build vers ~/Library
# Usage: ./Scripts/install_au.sh [debug|release|debug-intel|release-intel]

set -e

BUILD_TYPE="${1:-debug}"
AU_INSTALL_DIR="$HOME/Library/Audio/Plug-Ins/Components"

# Déterminer le chemin du build selon le type
case "$BUILD_TYPE" in
    debug)
        BUILD_DIR="cmake-build-debug/DiatonyDawApplication_artefacts/Debug/AU"
        ;;
    release)
        BUILD_DIR="cmake-build-release/DiatonyDawApplication_artefacts/Release/AU"
        ;;
    debug-intel)
        BUILD_DIR="cmake-build-debug-intel/DiatonyDawApplication_artefacts/Debug/AU"
        ;;
    release-intel)
        BUILD_DIR="cmake-build-release-intel/DiatonyDawApplication_artefacts/Release/AU"
        ;;
    *)
        echo "❌ Type de build inconnu: $BUILD_TYPE"
        echo "   Usage: $0 [debug|release|debug-intel|release-intel]"
        exit 1
        ;;
esac

AU_BUNDLE="$BUILD_DIR/DiatonyDawApplication.component"

# Vérifications
if [ ! -d "$AU_BUNDLE" ]; then
    echo "❌ Bundle AU non trouvé: $AU_BUNDLE"
    echo "   Avez-vous compilé avec: cmake --build cmake-build-${BUILD_TYPE}"
    exit 1
fi

echo "🔌 Installation de l'AU ($BUILD_TYPE)..."
echo "   Source: $AU_BUNDLE"
echo "   Destination: $AU_INSTALL_DIR/"

# Supprimer l'ancienne version si elle existe
if [ -d "$AU_INSTALL_DIR/DiatonyDawApplication.component" ]; then
    echo "   🗑️  Suppression de l'ancienne version..."
    rm -rf "$AU_INSTALL_DIR/DiatonyDawApplication.component"
fi

# Copier le nouveau bundle
echo "   📦 Copie du bundle..."
cp -r "$AU_BUNDLE" "$AU_INSTALL_DIR/"

# Vider le cache AU (optionnel mais recommandé)
echo "   🧹 Vidage du cache AudioUnit..."
rm -rf "$HOME/Library/Caches/AudioUnitCache/" 2>/dev/null || true

echo ""
echo "✅ AU installé avec succès!"
echo ""
echo "📝 Prochaines étapes:"
echo "   1. Redémarrer votre DAW (Ableton, Logic, etc.)"
echo "   2. Rescanner les plugins si nécessaire"
echo "   3. Chercher 'DiatonyDawApplication' dans les Audio Units"


