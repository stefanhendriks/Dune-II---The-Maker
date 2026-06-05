#!/bin/bash

# On cherche tous les dossiers qui commencent par "step_"
for dir in step_*/; do
    
    # On vérifie si la recherche a trouvé au moins un dossier
    if [ -d "$dir" ]; then
        echo "----------------------------------------"
        echo "📁 Entrée dans le dossier : ${dir%/}"
        (cd "$dir" && make clean)
    else
        echo "❌ Aucun dossier commençant par 'step_' n'a été trouvé."
        exit 1
    fi
done

echo "----------------------------------------"
echo "✅ Nettoyage terminé !"
