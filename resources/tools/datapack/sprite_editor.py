import sys
import os
import tkinter as tk
from tkinter import messagebox, filedialog
from PIL import Image

class SpriteEditor:
    # Constantes pour la clarté et la maintenance
    PROTECTED_RANGES = [
        (144, 150), (160, 166), (176, 182), (192, 198),
        (208, 214), (224, 230), (240, 246)
    ]
    ZOOM_DEFAULT = 20
    ZOOM_MAX = 25
    ZOOM_MIN = 10
    PALETTE_CELL_SIZE = 20

    def __init__(self, root, image_path):
        self.root = root
        self.root.title(f"D2TM Sprite Editor - {image_path}")
        self.root.geometry("1920x1080")
        self.image_path = image_path
        
        try:
            # Restriction aux fichiers PNG seulement
            if not image_path.lower().endswith('.png'):
                messagebox.showerror("Erreur", "Ce programme ne supporte que les fichiers PNG.")
                sys.exit(1)

            # Ouverture de l'image avec PIL
            self.image = Image.open(image_path)
            # On s'assure que l'image est en mode palette (P) pour accéder aux index
            if self.image.mode != 'P':
                print("Note: Conversion de l'image en mode palette (8-bit).")
                self.image = self.image.convert('P')
        except Exception as e:
            messagebox.showerror("Erreur", f"Impossible d'ouvrir l'image : {e}")
            sys.exit(1)

        self.width, self.height = self.image.size
        self.zoom = self.ZOOM_DEFAULT
        self.zoom_var = tk.IntVar(value=self.ZOOM_DEFAULT) # Variable pour le slider de zoom
        self.current_color_index = 1
        self.undo_stack = []
        self.current_undo_data = None
        
        self.setup_ui()
        self.draw_sprite()
        self.draw_palette()
        self._update_rgb_entries()

    def setup_ui(self):
        """Initialise l'interface utilisateur."""
        self.main_frame = tk.Frame(self.root)
        self.main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Zone de gauche : Canvas pour le Sprite
        left_frame = tk.Frame(self.main_frame)
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        tk.Label(left_frame, text="Edition du Sprite").pack()

        # Container pour le Canvas et ses barres de défilement
        canvas_container = tk.Frame(left_frame)
        canvas_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        self.v_scroll = tk.Scrollbar(canvas_container, orient=tk.VERTICAL)
        self.v_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.h_scroll = tk.Scrollbar(canvas_container, orient=tk.HORIZONTAL)
        self.h_scroll.pack(side=tk.BOTTOM, fill=tk.X)

        self.sprite_canvas = tk.Canvas(
            canvas_container, 
            bg="#222222",
            xscrollcommand=self.h_scroll.set,
            yscrollcommand=self.v_scroll.set
        )
        self.sprite_canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        # Définir la zone de défilement totale dès le début (utilise la valeur initiale du zoom)
        self.sprite_canvas.config(scrollregion=(0, 0, self.width * self.zoom, self.height * self.zoom))

        self.h_scroll.config(command=self.on_scroll_h)
        self.v_scroll.config(command=self.on_scroll_v)
        self.sprite_canvas.bind("<Configure>", lambda e: self.draw_sprite())

        self.sprite_canvas.bind("<Button-1>", self.on_sprite_press)
        self.sprite_canvas.bind("<B1-Motion>", self.on_sprite_click)
        self.sprite_canvas.bind("<ButtonRelease-1>", self.on_sprite_release)
        self.sprite_canvas.bind("<Button-3>", self.on_sprite_right_click)
        
        self.root.bind("<Control-z>", self.undo)

        # Zone de droite : Palette
        right_frame = tk.Frame(self.main_frame)
        right_frame.pack(side=tk.RIGHT, fill=tk.Y, padx=10)
        
        tk.Label(right_frame, text="Palette (256 couleurs)").pack()
        
        self.palette_canvas = tk.Canvas(right_frame, width=8 * self.PALETTE_CELL_SIZE, height=32 * self.PALETTE_CELL_SIZE, bg="white")
        self.palette_canvas.pack()
        self.palette_canvas.bind("<Button-1>", self.on_palette_click)

        self.selected_label = tk.Label(right_frame, text=f"Index sélectionné : {self.current_color_index}")
        self.selected_label.pack(pady=10)
        
        # Champs de modification RGB
        rgb_frame = tk.Frame(right_frame)
        rgb_frame.pack(pady=5)
        
        tk.Label(rgb_frame, text="R:").grid(row=0, column=0)
        self.r_entry = tk.Entry(rgb_frame, width=5)
        self.r_entry.grid(row=0, column=1, padx=2)

        tk.Label(rgb_frame, text="G:").grid(row=1, column=0)
        self.g_entry = tk.Entry(rgb_frame, width=5)
        self.g_entry.grid(row=1, column=1, padx=2)

        tk.Label(rgb_frame, text="B:").grid(row=2, column=0)
        self.b_entry = tk.Entry(rgb_frame, width=5)
        self.b_entry.grid(row=2, column=1, padx=2)
        
        self.apply_color_btn = tk.Button(right_frame, text="Appliquer Couleur", command=self.update_palette_color)
        self.apply_color_btn.pack(pady=5)

        # Contrôle du Zoom
        zoom_frame = tk.Frame(right_frame)
        zoom_frame.pack(pady=5)
        tk.Label(zoom_frame, text="Zoom:").pack(side=tk.LEFT)
        self.zoom_slider = tk.Scale(
            zoom_frame,
            from_=self.ZOOM_MIN, to=self.ZOOM_MAX, orient=tk.HORIZONTAL,
            variable=self.zoom_var,
            command=self.on_zoom_change,
            resolution=1
        )
        self.zoom_slider.pack(side=tk.LEFT, padx=5)

        save_btn = tk.Button(right_frame, text="Sauvegarder l'image", command=self.save_image)
        save_btn.pack(side=tk.BOTTOM, pady=20)

    def _get_color_from_palette(self, index):
        """Récupère la couleur hexadécimale depuis la palette PIL."""
        palette = self.image.getpalette() # Liste [R,G,B, R,G,B, ...]
        if not palette: return "#000000"
        r = palette[index * 3]
        g = palette[index * 3 + 1]
        b = palette[index * 3 + 2]
        return f'#{r:02x}{g:02x}{b:02x}'

    def _draw_pixel_to_canvas(self, canvas, grid_x, grid_y, size, index, tag, outline="#111111"):
        """Dessine un pixel (rect simple ou motif spécial pour l'index 223)."""
        x1, y1 = grid_x * size, grid_y * size
        x2, y2 = x1 + size, y1 + size
        
        if index == 223:
            # Motif de transparence : gris clair avec une croix grise (pour index 223 et 0)
            canvas.create_rectangle(x1, y1, x2, y2, fill="#D3D3D3", outline=outline, tags=tag)
            canvas.create_line(x1, y1, x2, y2, fill="#808080", tags=tag)
            canvas.create_line(x1, y2, x2, y1, fill="#808080", tags=tag)
        elif index == 0:
            # Motif de transparence : gris clair avec une croix grise (pour index 223 et 0)
            canvas.create_rectangle(x1, y1, x2, y2, fill="#000000", outline=outline, tags=tag)
            canvas.create_line(x1, y1, x2, y2, fill="#AAAAAA", tags=tag)
            canvas.create_line(x1, y2, x2, y1, fill="#AAAAAA", tags=tag)
        else:
            color = self._get_color_from_palette(index)
            canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline=outline, tags=tag)

    def on_scroll_h(self, *args):
        """Gère le défilement horizontal et redessine les pixels visibles."""
        self.sprite_canvas.xview(*args)
        self.draw_sprite()

    def on_zoom_change(self, val):
        """Met à jour le zoom et redessine le sprite."""
        self.zoom = int(val)
        # Mettre à jour la scrollregion avec le nouveau zoom
        self.sprite_canvas.config(scrollregion=(0, 0, self.width * self.zoom, self.height * self.zoom))
        self.draw_sprite()

    def on_scroll_v(self, *args):
        """Gère le défilement vertical et redessine les pixels visibles."""
        self.sprite_canvas.yview(*args)
        self.draw_sprite()

    def draw_sprite(self):
        """Dessine seulement les pixels visibles sur le canvas de gauche (Culling)."""
        self.sprite_canvas.delete("all")
        
        # Coordonnées réelles du viewport dans le canvas (tenant compte du scroll)
        x_left = self.sprite_canvas.canvasx(0)
        y_top = self.sprite_canvas.canvasy(0)
        
        # Dimensions visibles du widget
        width = self.sprite_canvas.winfo_width()
        height = self.sprite_canvas.winfo_height()
        
        # Au démarrage winfo renvoie 1, on attend l'affichage réel
        if width <= 1 or height <= 1:
            return

        x_right = x_left + width
        y_bottom = y_top + height

        # Conversion des coordonnées écran en index de grille image
        start_x = max(0, int(x_left // self.zoom))
        end_x = min(self.width, int(x_right // self.zoom) + 1)
        start_y = max(0, int(y_top // self.zoom))
        end_y = min(self.height, int(y_bottom // self.zoom) + 1)

        pixels = self.image.load()
        for y in range(start_y, end_y):
            for x in range(start_x, end_x): # self.zoom est maintenant mis à jour par le slider
                self._draw_pixel_to_canvas(self.sprite_canvas, x, y, self.zoom, pixels[x, y], f"pixel_{x}_{y}")

    def draw_palette(self):
        """Dessine la grille 8x32 des couleurs de la palette."""
        self.palette_canvas.delete("all")
        size = self.PALETTE_CELL_SIZE
        for i in range(256):
            gx, gy = i % 8, i // 8
            tag = f"pal_{i}"
            is_selected = (i == self.current_color_index)
            outline = "red" if is_selected else "black"
            
            self._draw_pixel_to_canvas(self.palette_canvas, gx, gy, size, i, tag, outline=outline)
            
            if is_selected:
                # Forcer l'épaisseur de la bordure pour la sélection
                x1, y1 = gx * size, gy * size
                self.palette_canvas.create_rectangle(x1, y1, x1+size, y1+size, outline="red", width=2, tags=tag)

    def _is_index_protected(self, index):
        """Vérifie si l'index appartient aux plages réservées (Team Colors)."""
        return any(start <= index <= end for start, end in self.PROTECTED_RANGES)

    def _set_entry_value(self, entry, value, state=tk.NORMAL):
        """Utilitaire pour mettre à jour un champ de saisie proprement."""
        entry.config(state=tk.NORMAL)
        entry.delete(0, tk.END)
        entry.insert(0, str(value))
        entry.config(state=state)

    def _update_rgb_entries(self):
        """Met à jour les champs R, G, B avec les valeurs de l'index actuel."""
        palette = self.image.getpalette()
        if palette:
            r = palette[self.current_color_index * 3]
            g = palette[self.current_color_index * 3 + 1]
            b = palette[self.current_color_index * 3 + 2]

            is_protected = self._is_index_protected(self.current_color_index)
            state = tk.DISABLED if is_protected else tk.NORMAL

            self._set_entry_value(self.r_entry, r, state)
            self._set_entry_value(self.g_entry, g, state)
            self._set_entry_value(self.b_entry, b, state)

            self.apply_color_btn.config(state=state)
            
            if is_protected:
                self.selected_label.config(text=f"Index {self.current_color_index} (Protégé - Team Color)")
            else:
                self.selected_label.config(text=f"Index sélectionné : {self.current_color_index}")

    def update_palette_color(self):
        """Applique les valeurs des entrées R, G, B à la palette de l'image."""
        if self._is_index_protected(self.current_color_index):
            return

        try:
            r_raw = int(self.r_entry.get())
            g_raw = int(self.g_entry.get())
            b_raw = int(self.b_entry.get())
        except ValueError:
            messagebox.showerror("Erreur", "Les valeurs RGB doivent être des entiers.")
            return

        # Limiteur : clamp les valeurs entre 0 et 255 (plus élégant)
        clamp = lambda n: max(0, min(255, n))
        r, g, b = map(clamp, [r_raw, g_raw, b_raw])

        # On ré-affiche les valeurs (si elles ont été bridées)
        self._update_rgb_entries()

        palette = list(self.image.getpalette())
        palette[self.current_color_index * 3] = r
        palette[self.current_color_index * 3 + 1] = g
        palette[self.current_color_index * 3 + 2] = b
        self.image.putpalette(palette)
        self.draw_palette()
        self.draw_sprite()

    def on_palette_click(self, event):
        """Sélectionne une couleur dans la palette."""
        col, row = event.x // self.PALETTE_CELL_SIZE, event.y // self.PALETTE_CELL_SIZE
        if 0 <= col < 8 and 0 <= row < 32:
            new_index = row * 8 + col
            if new_index == 0:
                return # L'index 0 (souvent transparence) n'est pas sélectionnable
            
            self.current_color_index = new_index
            self.selected_label.config(text=f"Index sélectionné : {self.current_color_index}")
            self.draw_palette()
            self._update_rgb_entries()

    def on_sprite_right_click(self, event):
        """Récupère l'index de couleur du pixel sous la souris (Pipette)."""
        canvas_x = self.sprite_canvas.canvasx(event.x)
        canvas_y = self.sprite_canvas.canvasy(event.y)
        x, y = int(canvas_x // self.zoom), int(canvas_y // self.zoom)
        
        if 0 <= x < self.width and 0 <= y < self.height:
            index = self.image.getpixel((x, y))
            if index == 0:
                return # On ignore l'index 0 à la pipette
                
            # On récupère l'index directement depuis l'image PIL
            self.current_color_index = index
            self.selected_label.config(text=f"Index sélectionné : {self.current_color_index}")
            self.draw_palette()
            self._update_rgb_entries()

    def on_sprite_press(self, event):
        """Démarre l'enregistrement d'un nouveau tracé pour l'historique."""
        self.current_undo_data = {}
        self.on_sprite_click(event)

    def on_sprite_release(self, event):
        """Ferme le tracé en cours et l'ajoute à la pile d'annulation."""
        if self.current_undo_data:
            self.undo_stack.append(self.current_undo_data)
        self.current_undo_data = None

    def undo(self, event=None):
        """Annule le dernier tracé en restaurant pixel par pixel."""
        if self.undo_stack:
            changes = self.undo_stack.pop()
            for (x, y), old_index in changes.items():
                self.image.putpixel((x, y), old_index)
                tag = f"pixel_{x}_{y}"
                self.sprite_canvas.delete(tag)
                self._draw_pixel_to_canvas(self.sprite_canvas, x, y, self.zoom, old_index, tag)

    def on_sprite_click(self, event):
        """Modifie le pixel sous la souris sur le sprite."""
        # Conversion des coordonnées de l'événement en coordonnées réelles du canvas
        # (prend en compte le décalage du scroll)
        canvas_x = self.sprite_canvas.canvasx(event.x)
        canvas_y = self.sprite_canvas.canvasy(event.y)
        x, y = int(canvas_x // self.zoom), int(canvas_y // self.zoom)
        
        if 0 <= x < self.width and 0 <= y < self.height:
            old_index = self.image.getpixel((x, y))
            if old_index == self.current_color_index:
                return

            # Enregistrement du pixel avant modification (une seule fois par tracé)
            if self.current_undo_data is not None and (x, y) not in self.current_undo_data:
                self.current_undo_data[(x, y)] = old_index

            self.image.putpixel((x, y), self.current_color_index)
            # On redessine le pixel spécifique (on supprime l'ancien car 223 utilise plusieurs objets)
            tag = f"pixel_{x}_{y}"
            self.sprite_canvas.delete(tag)
            self._draw_pixel_to_canvas(self.sprite_canvas, x, y, self.zoom, self.current_color_index, tag)

    def save_image(self):
        """Sauvegarde l'image modifiée."""
        ext = ".png"

        path = filedialog.asksaveasfilename(
            initialfile=os.path.basename(self.image_path),
            defaultextension=ext,
            filetypes=[("PNG", "*.png")]
        )
        
        if path:
            # L'objet self.image est maintenu en mode 'P' (Palette).
            # Pillow sauvegarde donc automatiquement en 8 bits indexés avec la palette actuelle.
            try:
                self.image.save(path)
                messagebox.showinfo("Succès", f"Image sauvegardée en format indexé 8-bit dans :\n{path}")
            except Exception as e:
                messagebox.showerror("Erreur", f"Échec de la sauvegarde : {e}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 sprite_editor.py <image.png>")
        sys.exit(1)
    root = tk.Tk()
    app = SpriteEditor(root, sys.argv[1])
    root.mainloop()