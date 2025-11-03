#!/bin/bash

echo "🔍 Vérification de la version de l'AU installé"
echo ""

BUILD_AU="cmake-build-debug/DiatonyDawApplication_artefacts/Debug/AU/DiatonyDawApplication.component/Contents/MacOS/DiatonyDawApplication"
INSTALLED_AU="$HOME/Library/Audio/Plug-Ins/Components/DiatonyDawApplication.component/Contents/MacOS/DiatonyDawApplication"

if [ ! -f "$BUILD_AU" ]; then
    echo "❌ AU de build introuvable. Compilez d'abord !"
    exit 1
fi

if [ ! -f "$INSTALLED_AU" ]; then
    echo "❌ AU installé introuvable dans ~/Library/Audio/Plug-Ins/Components/"
    exit 1
fi

echo "📅 Dates de modification :"
echo "   Build    : $(stat -f "%Sm" -t "%Y-%m-%d %H:%M:%S" "$BUILD_AU")"
echo "   Installé : $(stat -f "%Sm" -t "%Y-%m-%d %H:%M:%S" "$INSTALLED_AU")"
echo ""

BUILD_HASH=$(shasum "$BUILD_AU" | awk '{print $1}')
INSTALLED_HASH=$(shasum "$INSTALLED_AU" | awk '{print $1}')

echo "🔐 Checksums :"
echo "   Build    : $BUILD_HASH"
echo "   Installé : $INSTALLED_HASH"
echo ""

if [ "$BUILD_HASH" = "$INSTALLED_HASH" ]; then
    echo "✅ Les versions correspondent parfaitement !"
    echo ""
    echo "📦 Vérification des dylibs :"
    DYLIB_COUNT=$(ls -1 "$HOME/Library/Audio/Plug-Ins/Components/DiatonyDawApplication.component/Contents/Frameworks/" | wc -l)
    echo "   Nombre de dylibs : $DYLIB_COUNT (attendu: 10)"
    
    if [ "$DYLIB_COUNT" -eq 10 ]; then
        echo "   ✅ Toutes les dylibs présentes"
    else
        echo "   ⚠️  Dylibs manquantes !"
    fi
else
    echo "⚠️  Les versions ne correspondent PAS !"
    echo ""
    echo "Solution : Recompilez l'AU avec :"
    echo "   cmake --build cmake-build-debug --target DiatonyDawApplication_AU"
fi


