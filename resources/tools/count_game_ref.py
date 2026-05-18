#!/usr/bin/env python3
"""
Tkinter version to list files containing the word 'game.',
sorted by decreasing number of occurrences.
"""

import os
import re
import threading
import tkinter as tk
from tkinter import filedialog, ttk


class GameOccurrencesApp:

    def __init__(self, root):
        self.root = root
        self.root.title("Occurrence counter for 'game.'")
        self.root.geometry("800x500")
        self.root.minsize(600, 400)

        # Configuration du motif de recherche
        self.pattern = re.compile(r"\bgame\.\b")

        # Dossier par défaut : './src' par rapport au script
        default_dir = os.path.join(
            os.path.dirname(os.path.abspath(__file__)), "src"
        )
        self.src_dir_var = tk.StringVar(value=default_dir)

        self.setup_ui()

    def setup_ui(self):
        # --- Zone de sélection du dossier ---
        top_frame = ttk.Frame(self.root, padding="10")
        top_frame.pack(fill=tk.X)

        ttk.Label(top_frame, text="Source directory :").pack(
            side=tk.LEFT, padx=(0, 5)
        )
        entry_dir = ttk.Entry(
            top_frame, textvariable=self.src_dir_var, width=50
        )
        entry_dir.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)

        btn_browse = ttk.Button(
            top_frame, text="Browse...", command=self.browse_directory
        )
        btn_browse.pack(side=tk.LEFT, padx=5)

        self.btn_search = ttk.Button(
            top_frame, text="Start the analysis", command=self.start_search
        )
        self.btn_search.pack(side=tk.LEFT, padx=(10, 0))

        # --- Zone des résultats (Tableau) ---
        table_frame = ttk.Frame(self.root, padding="10")
        table_frame.pack(fill=tk.BOTH, expand=True)

        # Configuration des colonnes
        columns = ("occurrences", "file_path")
        self.tree = ttk.Treeview(
            table_frame, columns=columns, show="headings", selectmode="browse"
        )

        self.tree.heading("occurrences", text="occurrences", anchor=tk.E)
        self.tree.heading("file_path", text="file", anchor=tk.W)

        self.tree.column("occurrences", width=120, minwidth=80, anchor=tk.E)
        self.tree.column("file_path", width=550, minwidth=200, anchor=tk.W)

        # Barres de défilement
        scrollbar_y = ttk.Scrollbar(
            table_frame, orient=tk.VERTICAL, command=self.tree.yview
        )
        self.tree.configure(yscrollcommand=scrollbar_y.set)

        self.tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar_y.pack(side=tk.RIGHT, fill=tk.Y)

        # --- Barre de statut (Résumé) ---
        self.status_frame = ttk.Frame(self.root, padding="5", relief=tk.SUNKEN)
        self.status_frame.pack(fill=tk.X, side=tk.BOTTOM)

        self.lbl_status = ttk.Label(
            self.status_frame,
            text="Ready. Click on 'Start analysis' to begin.",
        )
        self.lbl_status.pack(side=tk.LEFT)

    def browse_directory(self):
        selected_dir = filedialog.askdirectory(
            initialdir=self.src_dir_var.get(), title="Choose the source folder"
        )
        if selected_dir:
            self.src_dir_var.set(selected_dir)

    def start_search(self):
        # Désactivation du bouton pour éviter les clics multiples pendant le scan
        self.btn_search.config(state=tk.DISABLED)
        self.lbl_status.config(text="Analysis in progress...")

        # Nettoyage de la table précédente
        for item in self.tree.get_children():
            self.tree.delete(item)

        # Lancement dans un thread pour ne pas bloquer l'IHM
        threading.Thread(target=self.run_search, daemon=True).start()

    def run_search(self):
        target_dir = self.src_dir_var.get()
        base_dir = os.path.dirname(os.path.abspath(__file__))

        results = []
        total_occurrences = 0

        if not os.path.exists(target_dir):
            self.root.after(
                0,
                self.update_ui_error,
                f"Erreur : The repertory '{target_dir}' didn't exist.",
            )
            return

        for root_dir, _, files in os.walk(target_dir):
            for filename in files:
                filepath = os.path.join(root_dir, filename)
                try:
                    with open(
                        filepath, "r", encoding="utf-8", errors="replace"
                    ) as f:
                        content = f.read()
                except OSError:
                    continue

                count = len(self.pattern.findall(content))
                if count > 0:
                    rel_path = os.path.relpath(filepath, base_dir)
                    results.append((count, rel_path))
                    total_occurrences += count

        # Tri décroissant selon le nombre d'occurrences
        results.sort(key=lambda x: x[0], reverse=True)

        # Mise à jour de l'interface graphique (via le thread principal)
        self.root.after(0, self.update_ui_results, results, total_occurrences)

    def update_ui_results(self, results, total_occurrences):
        for count, path in results:
            self.tree.insert("", tk.END, values=(count, path))

        # Mise à jour du résumé textuel en bas
        txt_status = f"Total occurrences : {total_occurrences} Files affected : {len(results)}"
        self.lbl_status.config(text=txt_status)
        self.btn_search.config(state=tk.NORMAL)

    def update_ui_error(self, message):
        self.lbl_status.config(text=message)
        self.btn_search.config(state=tk.NORMAL)


if __name__ == "__main__":
    root = tk.Tk()
    app = GameOccurrencesApp(root)
    root.mainloop()
