# 1. On liste automatiquement tous les sous-dossiers qui commencent par "step_"
SUBDIRS := $(sort $(wildcard step_*/))

# Déclaration des cibles Phony (pour éviter les conflits si un fichier porte le même nom)
.PHONY: all clean $(SUBDIRS)

# Par défaut, on lance la compilation de tous les sous-dossiers
all: $(SUBDIRS)

# Règle pour entrer dans un sous-dossier et exécuter son Makefile
$(SUBDIRS):
	@echo "========================================"
	@echo "--> Entrée dans le dossier : $@"
	@echo "========================================"
	$(MAKE) -C $@

# Règle pour nettoyer tous les sous-dossiers d'un coup (via 'make clean')
clean:
	@for dir in $(SUBDIRS); do \
		echo "--> Nettoyage dans : $$dir"; \
		$(MAKE) -C $$dir clean; \
	done