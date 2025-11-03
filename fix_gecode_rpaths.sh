#!/bin/bash

# Script pour corriger les RPATHs des dylibs Gecode dans un bundle
# Usage: ./fix_gecode_rpaths.sh <FRAMEWORKS_DIR>

if [ $# -ne 1 ]; then
    echo "Usage: $0 <FRAMEWORKS_DIR>"
    echo "Example: $0 MyApp.app/Contents/Frameworks"
    exit 1
fi

FRAMEWORKS_DIR="$1"

if [ ! -d "$FRAMEWORKS_DIR" ]; then
    echo "❌ Le dossier $FRAMEWORKS_DIR n'existe pas"
    exit 1
fi

echo "🔧 Correction des RPATHs des dylibs Gecode dans $FRAMEWORKS_DIR"
echo ""

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

# Pour chaque dylib Gecode
for lib in "${GECODE_LIBS[@]}"; do
    LIB_PATH="$FRAMEWORKS_DIR/$lib"
    
    if [ ! -f "$LIB_PATH" ]; then
        echo "⚠️  $lib non trouvée, skip"
        continue
    fi
    
    echo "📦 Traitement de $lib..."
    
    # 1. Changer son propre ID pour utiliser @rpath
    install_name_tool -id "@rpath/$lib" "$LIB_PATH"
    
    # 2. Changer toutes les références vers d'autres dylibs Gecode pour utiliser @rpath
    for dep_lib in "${GECODE_LIBS[@]}"; do
        # Vérifier si cette dylib référence dep_lib
        if otool -L "$LIB_PATH" | grep -q "$dep_lib"; then
            # Trouver le chemin absolu actuel
            OLD_PATH=$(otool -L "$LIB_PATH" | grep "$dep_lib" | awk '{print $1}')
            
            # Le remplacer par @rpath
            install_name_tool -change "$OLD_PATH" "@rpath/$dep_lib" "$LIB_PATH" 2>/dev/null
        fi
    done
    
    echo "   ✓ $lib corrigée"
done

echo ""
echo "✅ Tous les RPATHs Gecode ont été corrigés dans $FRAMEWORKS_DIR"

