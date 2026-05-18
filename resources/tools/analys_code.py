import os
import re
import tkinter as tk
from tkinter import ttk


class CppDependencyAnalyzer:

    def __init__(self, root_dir):
        self.root_dir = root_dir
        self.dependencies = {}
        self.class_scope_regex = re.compile(r"\b([A-Z][a-zA-Z0-9_]*)::")
        self.make_unique_regex = re.compile(
            r"std::make_unique<\s*([a-zA-Z0-9_]+)"
        )
        self.make_shared_regex = re.compile(
            r"std::make_shared<\s*([a-zA-Z0-9_]+)"
        )
        self.new_regex = re.compile(r"\bnew\s+([a-zA-Z0-9_]+)")

    def analyze(self):
        for root, _, files in os.walk(self.root_dir):
            for file in files:
                if file.endswith((".cpp", ".h", ".hpp")):
                    self._parse_file(os.path.join(root, file), file)
        return self.dependencies

    def _parse_file(self, file_path, file_name):
        current_class = os.path.splitext(file_name)[0]
        try:
            with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                for line in f:
                    if line.strip().startswith("//"):
                        continue
                    scope_match = self.class_scope_regex.search(line)
                    if scope_match:
                        current_class = scope_match.group(1)

                    created_objects = []
                    created_objects.extend(self.make_unique_regex.findall(line))
                    created_objects.extend(self.make_shared_regex.findall(line))
                    created_objects.extend(self.new_regex.findall(line))

                    for obj in created_objects:
                        if obj != current_class and not obj.isdigit():
                            if current_class not in self.dependencies:
                                self.dependencies[current_class] = set()
                            self.dependencies[current_class].add(obj)
        except Exception as e:
            print(f"Erreur de lecture : {e}")


class AnalyzerApp:

    def __init__(self, root, data):
        self.root = root
        self.data = data
        self.root.title("C++ Creator-Created Dependency Viewer")
        self.root.geometry("900x650")

        self._create_search_bar()
        self._create_scrollable_table()
        self._populate_table()

    def _create_search_bar(self):
        search_frame = ttk.Frame(self.root, padding=10)
        search_frame.pack(fill=tk.X)

        ttk.Label(search_frame, text="Filtrer par classe créatrice :").pack(
            side=tk.LEFT, padx=5
        )
        self.search_var = tk.StringVar()
        self.search_var.trace_add("write", self._filter_table)
        ttk.Entry(search_frame, textvariable=self.search_var, width=30).pack(
            side=tk.LEFT, padx=5
        )

    def _create_scrollable_table(self):
        # Conteneur principal pour le tableau et sa scrollbar
        outer_frame = ttk.Frame(self.root, padding=10)
        outer_frame.pack(fill=tk.BOTH, expand=True)

        # Création d'un Canvas pour permettre le défilement vertical
        self.canvas = tk.Canvas(outer_frame, borderwidth=0, highlightthickness=0)
        scrollbar = ttk.Scrollbar(
            outer_frame, orient="vertical", command=self.canvas.yview
        )

        # Cadre interne qui contiendra la grille de données
        self.table_frame = ttk.Frame(self.canvas)

        # Liaison du cadre au canvas
        self.table_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(
                scrollregion=self.canvas.bbox("all")
            ),
        )
        self.canvas_window = self.canvas.create_window(
            (0, 0), window=self.table_frame, anchor="nw"
        )

        # Ajuster la largeur du cadre interne lorsque le canvas change de taille
        self.canvas.bind(
            "<Configure>",
            lambda e: self.canvas.itemconfig(self.canvas_window, width=e.width),
        )
        self.canvas.configure(yscrollcommand=scrollbar.set)

        # Pack des éléments de défilement
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        # Liaison de la molette de la souris pour le confort de navigation
        self.canvas.bind_all("<MouseWheel>", self._on_mousewheel)

    def _on_mousewheel(self, event):
        # Gestion multiplateforme de la molette
        if event.num == 5 or event.delta == -120:
            self.canvas.yview_scroll(1, "units")
        elif event.num == 4 or event.delta == 120:
            self.canvas.yview_scroll(-1, "units")

    def _clear_table(self):
        for widget in self.table_frame.winfo_children():
            widget.destroy()

    def _draw_headers(self):
        # En-têtes du tableau
        style_h = {"font": ("Segoe UI", 10, "bold"), "background": "#dcdcdc"}

        lbl1 = tk.Label(
            self.table_frame,
            text="Classe Créatrice (A)",
            relief=tk.RIDGE,
            padx=10,
            pady=5,
            **style_h,
        )
        lbl1.grid(row=0, column=0, sticky="ew")

        lbl2 = tk.Label(
            self.table_frame,
            text="Ressources Créées (B, C, D...)",
            relief=tk.RIDGE,
            padx=10,
            pady=5,
            **style_h,
        )
        lbl2.grid(row=0, column=1, sticky="ew")

        self.table_frame.columnconfigure(0, weight=1, minsize=250)
        self.table_frame.columnconfigure(1, weight=3, minsize=550)

    def _populate_table(self):
        self._clear_table()
        self._draw_headers()

        search_term = self.search_var.get().lower()
        row_idx = 1

        for creator, created_set in sorted(self.data.items()):
            if search_term and search_term not in creator.lower():
                continue

            created_str = ", ".join(sorted(created_set))

            # Alternance des couleurs de fond pour la lisibilité (effet zèbre)
            bg_color = "#ffffff" if row_idx % 2 == 0 else "#f4f4f6"

            # Cellule de gauche (Classe créatrice)
            lbl_creator = tk.Label(
                self.table_frame,
                text=creator,
                font=("Consolas", 10, "bold"),
                anchor="nw",
                background=bg_color,
                relief=tk.FLAT,
                padx=10,
                pady=6,
            )
            lbl_creator.grid(row=row_idx, column=0, sticky="nsew")

            # Cellule de droite (Ressources avec retour à la ligne automatique via wraplength)
            lbl_created = tk.Label(
                self.table_frame,
                text=created_str,
                font=("Consolas", 10),
                anchor="nw",
                justify=tk.LEFT,
                background=bg_color,
                relief=tk.FLAT,
                padx=10,
                pady=6,
                wraplength=550,  # Force le retour à la ligne après 550 pixels
            )
            lbl_created.grid(row=row_idx, column=1, sticky="nsew")

            row_idx += 1

    def _filter_table(self, *args):
        self._populate_table()


if __name__ == "__main__":
    # --- CONFIGURATION ---
    PATH_TO_PROJECT = "./src"  # À adapter à vos dossiers
    # ---------------------

    if not os.path.exists(PATH_TO_PROJECT):
        print(f"Dossier '{PATH_TO_PROJECT}' introuvable.")
    else:
        analyzer = CppDependencyAnalyzer(PATH_TO_PROJECT)
        dependencies = analyzer.analyze()

        root = tk.Tk()
        app = AnalyzerApp(root, dependencies)
        root.mainloop()
