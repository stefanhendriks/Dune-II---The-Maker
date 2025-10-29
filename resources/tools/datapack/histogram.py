import os
import sys
from PIL import Image

def compter_pixels_palette(chemin_image):
    """
    Ouvre une image, vérifie si elle est en mode 'P' (palette), 
    et compte le nombre de pixels pour chaque couleur de la palette.
    Affiche le résultat au format "XX : nombre pixels" sur 4 colonnes 
    avec le nombre de pixels sur 6 caractères.
    """
    try:
        # Ouvrir l'image
        img = Image.open(chemin_image)
    except FileNotFoundError:
        print(f"Erreur : Le fichier '{chemin_image}' est introuvable.")
        return
    except Exception as e:
        print(f"Erreur lors de l'ouverture de l'image : {e}")
        return

    # Vérifier le mode de l'image
    if img.mode != 'P':
        print(f"L'image n'est pas en mode 'P' (palette 8 bits). Son mode est : {img.mode}.")
        # Tenter la conversion si nécessaire (facultatif)
        # img = img.convert('P')
        # print("Conversion en mode 'P' effectuée.")
        # if img.mode != 'P':
        #    print("Échec de la conversion en mode 'P'.")
        #    return
    
    # 1. Obtenir l'histogramme
    # Pour une image en mode 'P', histogram() retourne une liste de 256 entiers,
    # où l'index est l'index de la couleur dans la palette (de 0 à 255) 
    # et la valeur est le nombre de pixels utilisant cet index.
    histogramme = img.histogram()
    
    # 2. Afficher les résultats
    print(f"--- Compte des pixels par index de palette pour '{chemin_image}' ---")
    
    # Définir le nombre de colonnes et la largeur d'affichage
    nb_colonnes = 4
    largeur_affichage = 25 # Longueur de chaque bloc "XX : NNNNNN" + espaces

    for index, count in enumerate(histogramme):
        # Formatage de l'index de la couleur (sur 2 chiffres)
        index_str = f"{index:02d}" 
        
        # Formatage du nombre de pixels (sur 6 caractères)
        count_str = f"{count:6d}" 
        
        # Construire la ligne d'affichage
        ligne = f"{index_str} : {count_str}"
        
        # Afficher le bloc avec des espaces pour l'alignement
        print(f"{ligne:<{largeur_affichage}}", end="")
        
        # Sauter de ligne après 'nb_colonnes' blocs
        if (index + 1) % nb_colonnes == 0:
            print()
            
    # S'assurer qu'il y a un saut de ligne final si la boucle ne s'est pas terminée sur un saut
    if len(histogramme) % nb_colonnes != 0:
        print()
        
    print("-------------------------------------------------------------------")


# --- Utilisation du script ---
if len(sys.argv) != 2:
   print("Usage: python3 main.py <packfile>")
   sys.exit(1)

nom_fichier = sys.argv[1]
compter_pixels_palette(nom_fichier)
