import sys
import os
import tkinter as tk
from tkinter import messagebox, filedialog
from PIL import Image

class SpriteEditor:
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
        self.zoom = 20  # Facteur de zoom pour l'aspect pixel art
        self.current_color_index = 0
        
        self.setup_ui()
        self.draw_sprite()
        self.draw_palette()

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
        self.h_scroll.config(command=self.sprite_canvas.xview)
        self.v_scroll.config(command=self.sprite_canvas.yview)

        self.sprite_canvas.bind("<Button-1>", self.on_sprite_click)
        self.sprite_canvas.bind("<B1-Motion>", self.on_sprite_click)
        self.sprite_canvas.bind("<Button-3>", self.on_sprite_right_click)

        # Zone de droite : Palette
        right_frame = tk.Frame(self.main_frame)
        right_frame.pack(side=tk.RIGHT, fill=tk.Y, padx=10)
        
        tk.Label(right_frame, text="Palette (256 couleurs)").pack()
        
        self.palette_canvas = tk.Canvas(right_frame, width=8 * 20, height=32 * 20, bg="white")
        self.palette_canvas.pack()
        self.palette_canvas.bind("<Button-1>", self.on_palette_click)

        self.selected_label = tk.Label(right_frame, text=f"Index sélectionné : {self.current_color_index}")
        self.selected_label.pack(pady=10)
        
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
            # Motif de transparence : gris clair avec une croix grise
            canvas.create_rectangle(x1, y1, x2, y2, fill="#D3D3D3", outline=outline, tags=tag)
            canvas.create_line(x1, y1, x2, y2, fill="#808080", tags=tag)
            canvas.create_line(x1, y2, x2, y1, fill="#808080", tags=tag)
        else:
            color = self._get_color_from_palette(index)
            canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline=outline, tags=tag)

    def draw_sprite(self):
        """Dessine la grille de pixels sur le canvas de gauche."""
        self.sprite_canvas.delete("all")
        pixels = self.image.load()
        for y in range(self.height):
            for x in range(self.width):
                self._draw_pixel_to_canvas(self.sprite_canvas, x, y, self.zoom, pixels[x, y], f"pixel_{x}_{y}")
                
        # Mise à jour de la zone de défilement après le dessin
        self.sprite_canvas.config(scrollregion=self.sprite_canvas.bbox("all"))

    def draw_palette(self):
        """Dessine la grille 8x32 des couleurs de la palette."""
        self.palette_canvas.delete("all")
        size = 20
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

    def on_palette_click(self, event):
        """Sélectionne une couleur dans la palette."""
        col, row = event.x // 20, event.y // 20
        if 0 <= col < 8 and 0 <= row < 32:
            self.current_color_index = row * 8 + col
            self.selected_label.config(text=f"Index sélectionné : {self.current_color_index}")
            self.draw_palette()

    def on_sprite_right_click(self, event):
        """Récupère l'index de couleur du pixel sous la souris (Pipette)."""
        canvas_x = self.sprite_canvas.canvasx(event.x)
        canvas_y = self.sprite_canvas.canvasy(event.y)
        x, y = int(canvas_x // self.zoom), int(canvas_y // self.zoom)
        
        if 0 <= x < self.width and 0 <= y < self.height:
            # On récupère l'index directement depuis l'image PIL (mode P)
            self.current_color_index = self.image.getpixel((x, y))
            self.selected_label.config(text=f"Index sélectionné : {self.current_color_index}")
            self.draw_palette()

    def on_sprite_click(self, event):
        """Modifie le pixel sous la souris sur le sprite."""
        # Conversion des coordonnées de l'événement en coordonnées réelles du canvas
        # (prend en compte le décalage du scroll)
        canvas_x = self.sprite_canvas.canvasx(event.x)
        canvas_y = self.sprite_canvas.canvasy(event.y)
        x, y = int(canvas_x // self.zoom), int(canvas_y // self.zoom)
        
        if 0 <= x < self.width and 0 <= y < self.height:
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