import re
import sys
from collections import Counter

# Vérification argument
if len(sys.argv) < 2:
    print("Usage : python analyse.py <fichier_source>")
    sys.exit(1)

# Nom du fichier récupéré depuis la console
filename = sys.argv[1]

# Regex :
# cherche game.xxx
pattern = r'game\.[a-zA-Z_][a-zA-Z0-9_]*'

# Lecture du fichier
with open(filename, "r", encoding="utf-8") as file:
    content = file.read()

# Recherche de toutes les occurrences
matches = re.findall(pattern, content)

# Comptage
counter = Counter(matches)

# Affichage trié
for item, count in sorted(counter.items()):
    print(f"{item:<20} {count}")
