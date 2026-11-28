import sys
import os
from collections import deque
import tkinter as tk
from tkinter import messagebox, filedialog
from PIL import Image

class SpriteEditor:
    # Constants for clarity and maintenance
    PROTECTED_RANGES = [
        (144, 150), (160, 166), (176, 182), (192, 198),
        (208, 214), (224, 230), (240, 246)
    ]
    ZOOM_DEFAULT = 20
    ZOOM_MAX = 25
    ZOOM_MIN = 10
    PALETTE_CELL_SIZE = 20
    SCREEN_SIZE = "1920x1080"

    def __init__(self, root, image_path):
        self.root = root
        self.root.geometry(self.SCREEN_SIZE)

        self.image_path = image_path
        self.image = None
        self.width = 0
        self.height = 0
        self.zoom = self.ZOOM_DEFAULT
        self.zoom_var = tk.IntVar(value=self.ZOOM_DEFAULT) # Variable pour le slider de zoom
        self.current_color_index = 1
        self.selected_cells = set()
        self.undo_stack = []
        self.redo_stack = []
        self.current_undo_data = None

        if not self.load_image(image_path):
            sys.exit(1)
        
        self.setup_ui()
        self.draw_sprite()
        self.draw_palette()
        self._update_rgb_entries()

    def load_image(self, image_path):
        """Load an image and normalize it to a complete 256-color palette."""
        try:
            if not image_path.lower().endswith('.png'):
                messagebox.showerror("Error", "This program only supports PNG files.")
                return False

            image = Image.open(image_path)
            if image.mode != 'P':
                print("Note: Image converted to palette mode (8-bit).")
                image = image.convert('P')

            palette = image.getpalette() or []
            if image.mode == 'P' and len(palette) < 256 * 3:
                palette.extend([255] * ((256 * 3) - len(palette)))
                image.putpalette(palette)
        except Exception as e:
            messagebox.showerror("Error", f"Unable to open image: {e}")
            return False

        self.image_path = image_path
        self.image = image
        self.width, self.height = self.image.size
        self.root.title(f"D2TM Sprite Editor - {self.image_path}")
        self.undo_stack.clear()
        self.redo_stack.clear()
        self.selected_cells.clear()
        self.current_undo_data = None
        return True

    def setup_ui(self):
        """Initializes the user interface."""
        self.create_menus()

        self.main_frame = tk.Frame(self.root)
        self.main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Left area: Canvas for the Sprite
        left_frame = tk.Frame(self.main_frame)
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        tk.Label(left_frame, text="Sprite editor").pack()

        # Container for the Canvas and its scroll bars
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
        
        # Define the total scroll area from the start
        self.sprite_canvas.config(scrollregion=(0, 0, self.width * self.zoom, self.height * self.zoom))

        self.h_scroll.config(command=self.on_scroll_h)
        self.v_scroll.config(command=self.on_scroll_v)
        self.sprite_canvas.bind("<Configure>", lambda e: self.draw_sprite())

        self.sprite_canvas.bind("<Button-1>", self.on_sprite_press)
        self.sprite_canvas.bind("<B1-Motion>", self.on_sprite_click)
        self.sprite_canvas.bind("<ButtonRelease-1>", self.on_sprite_release)
        self.sprite_canvas.bind("<Button-3>", self.on_sprite_right_click)
        
        self.root.bind("<Control-z>", self.undo)
        self.root.bind("<Control-y>", self.redo)
        self.root.bind("<Escape>", self.clear_selection)

        # Right Zone : Palette
        right_frame = tk.Frame(self.main_frame)
        right_frame.pack(side=tk.RIGHT, fill=tk.Y, padx=10)
        
        tk.Label(right_frame, text="Palette (256 colours)").pack()
        
        self.palette_canvas = tk.Canvas(right_frame, width=8 * self.PALETTE_CELL_SIZE, height=32 * self.PALETTE_CELL_SIZE, bg="white")
        self.palette_canvas.pack()
        self.palette_canvas.bind("<Button-1>", self.on_palette_click)

        self.selected_label = tk.Label(right_frame, text=f"Index selection : {self.current_color_index}")
        self.selected_label.pack(pady=10)
        
        # RGB modification fields
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
        
        self.apply_color_btn = tk.Button(rgb_frame, text="Apply", command=self.update_palette_color)
        self.apply_color_btn.grid(row=0, column=2, rowspan=3, padx=10)

        # Zoom control
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

        # Zone index overriding
        replace_frame = tk.Frame(right_frame)
        replace_frame.pack(pady=10, fill=tk.X)

        tk.Label(replace_frame, text="A:").grid(row=0, column=0, padx=2)
        self.entry_a = tk.Entry(replace_frame, width=3)
        self.entry_a.grid(row=0, column=1, padx=2)

        tk.Label(replace_frame, text="B:").grid(row=0, column=2, padx=2)
        self.entry_b = tk.Entry(replace_frame, width=3)
        self.entry_b.grid(row=0, column=3, padx=2)

        self.change_btn = tk.Button(replace_frame, text="A->B", command=self.replace_index)
        self.change_btn.grid(row=0, column=4, padx=5)

        save_btn = tk.Button(right_frame, text="Save image", command=self.save_image)
        save_btn.pack(side=tk.BOTTOM, pady=20)

    def create_menus(self):
        """Create the top menu bar."""
        menu_bar = tk.Menu(self.root)

        file_menu = tk.Menu(menu_bar, tearoff=0)
        file_menu.add_command(label="Ouvrir...", command=self.open_image_dialog)
        file_menu.add_command(label="Sauvegarder...", command=self.save_image)
        file_menu.add_separator()
        file_menu.add_command(label="Quitter", command=self.root.destroy)
        menu_bar.add_cascade(label="Fichier", menu=file_menu)

        edit_menu = tk.Menu(menu_bar, tearoff=0)
        edit_menu.add_command(label="Annuler", command=self.undo, accelerator="Ctrl+Z")
        edit_menu.add_command(label="Rétablir", command=self.redo, accelerator="Ctrl+Y")
        menu_bar.add_cascade(label="Edition", menu=edit_menu)

        palette_menu = tk.Menu(menu_bar, tearoff=0)
        palette_menu.add_command(label="Remplacer une couleur...", command=self.replace_index_dialog)
        menu_bar.add_cascade(label="Palette", menu=palette_menu)

        self.root.config(menu=menu_bar)

    def open_image_dialog(self):
        """Open a PNG file and refresh the editor state."""
        path = filedialog.askopenfilename(
            title="Select a PNG sprite",
            filetypes=[("PNG", "*.png")]
        )
        if not path:
            return

        if not self.load_image(path):
            return

        self.sprite_canvas.config(scrollregion=(0, 0, self.width * self.zoom, self.height * self.zoom))
        self.draw_sprite()
        self.draw_palette()
        self._update_rgb_entries()

    def _get_color_from_palette(self, index):
        """Retrieves the hexadecimal colour from the PIL palette."""
        palette = self.image.getpalette() # Liste [R,G,B, R,G,B, ...]
        if not palette: return "#000000"
        r = palette[index * 3]
        g = palette[index * 3 + 1]
        b = palette[index * 3 + 2]
        return f'#{r:02x}{g:02x}{b:02x}'

    def _draw_pixel_to_canvas(self, canvas, grid_x, grid_y, size, index, tag, outline="#111111"):
        """Draw a pixel (simple rectangle or special pattern for index 223)."""
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

    def _draw_selection_border(self, x, y):
        """Draw selection border for one sprite cell."""
        x1, y1 = x * self.zoom, y * self.zoom
        x2, y2 = x1 + self.zoom, y1 + self.zoom
        self.sprite_canvas.create_rectangle(
            x1,
            y1,
            x2,
            y2,
            outline="#00ffff",
            width=2,
            tags=f"selection_{x}_{y}"
        )

    def _is_shift_pressed(self, event):
        """Return True when SHIFT is held during a mouse event."""
        return bool(event.state & 0x0001)

    def select_connected_cells(self, start_x, start_y):
        """Select all 8-connected cells sharing the same palette index."""
        if not (0 <= start_x < self.width and 0 <= start_y < self.height):
            return

        target_index = self.image.getpixel((start_x, start_y))
        queue = deque([(start_x, start_y)])
        visited = {(start_x, start_y)}
        added = False

        while queue:
            x, y = queue.popleft()
            if self.image.getpixel((x, y)) != target_index:
                continue

            if (x, y) not in self.selected_cells:
                self.selected_cells.add((x, y))
                added = True

            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    if dx == 0 and dy == 0:
                        continue

                    nx, ny = x + dx, y + dy
                    if 0 <= nx < self.width and 0 <= ny < self.height and (nx, ny) not in visited:
                        visited.add((nx, ny))
                        queue.append((nx, ny))

        if added:
            self.draw_sprite()

    def clear_selection(self, event=None):
        """Clear all selected cells and redraw the sprite."""
        if self.selected_cells:
            self.selected_cells.clear()
            self.draw_sprite()

    def on_scroll_h(self, *args):
        """Manage horizontal scrolling and redraw visible pixels."""
        self.sprite_canvas.xview(*args)
        self.draw_sprite()

    def on_zoom_change(self, val):
        """Met à jour le zoom et redessine le sprite."""
        self.zoom = int(val)
        # Mettre à jour la scrollregion avec le nouveau zoom
        self.sprite_canvas.config(scrollregion=(0, 0, self.width * self.zoom, self.height * self.zoom))
        self.draw_sprite()

    def on_scroll_v(self, *args):
        """Manage vertical scrolling and redraw visible pixels."""
        self.sprite_canvas.yview(*args)
        self.draw_sprite()

    def draw_sprite(self):
        """Draw only the pixels visible on the left canvas (Culling)."""
        self.sprite_canvas.delete("all")
        
        # Actual coordinates of the viewport within the canvas (taking scrolling into account)
        x_left = self.sprite_canvas.canvasx(0)
        y_top = self.sprite_canvas.canvasy(0)
        
        # Visible dimensions of the widget
        width = self.sprite_canvas.winfo_width()
        height = self.sprite_canvas.winfo_height()
        
        # At startup, winfo returns 1; we are waiting for the actual display.
        if width <= 1 or height <= 1:
            return

        x_right = x_left + width
        y_bottom = y_top + height

        # Converting screen coordinates to image grid index
        start_x = max(0, int(x_left // self.zoom))
        end_x = min(self.width, int(x_right // self.zoom) + 1)
        start_y = max(0, int(y_top // self.zoom))
        end_y = min(self.height, int(y_bottom // self.zoom) + 1)

        pixels = self.image.load()
        for y in range(start_y, end_y):
            for x in range(start_x, end_x): # self.zoom is now update by the slider
                self._draw_pixel_to_canvas(self.sprite_canvas, x, y, self.zoom, pixels[x, y], f"pixel_{x}_{y}")

        for x, y in self.selected_cells:
            if start_x <= x < end_x and start_y <= y < end_y:
                self._draw_selection_border(x, y)

    def draw_palette(self):
        """Draw the 8x32 grid of colours from the palette."""
        self.palette_canvas.delete("all")
        size = self.PALETTE_CELL_SIZE
        for i in range(256):
            gx, gy = i % 8, i // 8
            tag = f"pal_{i}"
            is_selected = (i == self.current_color_index)
            outline = "red" if is_selected else "black"
            
            self._draw_pixel_to_canvas(self.palette_canvas, gx, gy, size, i, tag, outline=outline)
            
            if is_selected:
                # Force border thickness for selection
                x1, y1 = gx * size, gy * size
                self.palette_canvas.create_rectangle(x1, y1, x1+size, y1+size, outline="red", width=2, tags=tag)

    def _is_index_protected(self, index):
        """Checks if the index belongs to the reserved ranges (Team Colours)."""
        return any(start <= index <= end for start, end in self.PROTECTED_RANGES)

    def _set_entry_value(self, entry, value, state=tk.NORMAL):
        """Utility for updating an input field cleanly."""
        entry.config(state=tk.NORMAL)
        entry.delete(0, tk.END)
        entry.insert(0, str(value))
        entry.config(state=state)

    def _update_rgb_entries(self):
        """Updates the R, G, B fields with the values ​​of the current index."""
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
                self.selected_label.config(text=f"Index {self.current_color_index} (Protected - Team Color)")
            else:
                self.selected_label.config(text=f"Index Selection : {self.current_color_index}")

    def update_palette_color(self):
        """Applies the values ​​of the R, G, B inputs to the image palette."""
        if self._is_index_protected(self.current_color_index):
            return

        try:
            r_raw = int(self.r_entry.get())
            g_raw = int(self.g_entry.get())
            b_raw = int(self.b_entry.get())
        except ValueError:
            messagebox.showerror("Error", "RGB values ​​must be integers.")
            return

        # Limiter: clamps values ​​between 0 and 255 (more elegant)
        clamp = lambda n: max(0, min(255, n))
        r, g, b = map(clamp, [r_raw, g_raw, b_raw])

        # The values ​​are redisplayed (if they have been limited).
        self._update_rgb_entries()

        palette = list(self.image.getpalette())
        palette[self.current_color_index * 3] = r
        palette[self.current_color_index * 3 + 1] = g
        palette[self.current_color_index * 3 + 2] = b
        self.image.putpalette(palette)
        self.draw_palette()
        self.draw_sprite()

    def replace_index(self):
        """Remplace toutes les occurrences de l'index A par l'index B."""
        try:
            idx_a = int(self.entry_a.get())
            idx_b = int(self.entry_b.get())
        except ValueError:
            messagebox.showerror("Error", "A and B must be integers (0-255).")
            return

        if not (0 <= idx_a <= 255 and 0 <= idx_b <= 255):
            messagebox.showerror("Error", "Indexes must be between 0 and 255.")
            return

        self._replace_index_values(idx_a, idx_b)

    def replace_index_dialog(self):
        """Open a single-page dialog to replace one color index by another."""
        dialog = tk.Toplevel(self.root)
        dialog.title("Remplacement de couleur")
        dialog.resizable(False, False)
        dialog.transient(self.root)
        dialog.grab_set()

        form = tk.Frame(dialog, padx=12, pady=12)
        form.pack(fill=tk.BOTH, expand=True)

        tk.Label(form, text="Ancienne couleur (0-255):").grid(row=0, column=0, sticky="w", pady=(0, 6))
        old_entry = tk.Entry(form, width=6)
        old_entry.grid(row=0, column=1, sticky="w", padx=(8, 0), pady=(0, 6))

        tk.Label(form, text="Nouvelle couleur (0-255):").grid(row=1, column=0, sticky="w")
        new_entry = tk.Entry(form, width=6)
        new_entry.grid(row=1, column=1, sticky="w", padx=(8, 0))

        def apply_replace():
            try:
                idx_a = int(old_entry.get())
                idx_b = int(new_entry.get())
            except ValueError:
                messagebox.showerror("Error", "A and B must be integers (0-255).", parent=dialog)
                return

            self._replace_index_values(idx_a, idx_b)
            dialog.destroy()

        action_row = tk.Frame(form)
        action_row.grid(row=2, column=0, columnspan=2, sticky="e", pady=(12, 0))
        tk.Button(action_row, text="Changer tout", command=apply_replace).pack(side=tk.LEFT)
        tk.Button(action_row, text="Annuler", command=dialog.destroy).pack(side=tk.LEFT, padx=(8, 0))

        old_entry.focus_set()

    def _replace_index_values(self, idx_a, idx_b):
        """Internal helper to replace all occurrences of index A by B."""
        if not (0 <= idx_a <= 255 and 0 <= idx_b <= 255):
            messagebox.showerror("Error", "Indexes must be between 0 and 255.")
            return

        if idx_a == idx_b:
            return

        changes = {}
        pixels = self.image.load()
        for y in range(self.height):
            for x in range(self.width):
                if pixels[x, y] == idx_a:
                    changes[(x, y)] = idx_a
                    self.image.putpixel((x, y), idx_b)

        if changes:
            self.undo_stack.append(changes)
            self.redo_stack.clear()
            self.draw_sprite()

    def apply_color_to_selection(self, new_index):
        """Apply the selected palette index to all selected cells."""
        if not self.selected_cells:
            return

        changes = {}
        for (x, y) in self.selected_cells:
            if 0 <= x < self.width and 0 <= y < self.height:
                old_index = self.image.getpixel((x, y))
                if old_index != new_index:
                    changes[(x, y)] = old_index
                    self.image.putpixel((x, y), new_index)

        if changes:
            self.undo_stack.append(changes)
            self.redo_stack.clear()
            self.draw_sprite()

    def on_palette_click(self, event):
        """Select a colour from the palette."""
        col, row = event.x // self.PALETTE_CELL_SIZE, event.y // self.PALETTE_CELL_SIZE
        if 0 <= col < 8 and 0 <= row < 32:
            new_index = row * 8 + col
            if new_index == 0:
                return # L'index 0 (souvent transparence) n'est pas sélectionnable
            
            self.current_color_index = new_index
            self.selected_label.config(text=f"Index selection : {self.current_color_index}")
            self.apply_color_to_selection(new_index)
            self.draw_palette()
            self._update_rgb_entries()

    def on_sprite_right_click(self, event):
        """Retrieves the colour index of the pixel under the mouse (Eyedropper)."""
        canvas_x = self.sprite_canvas.canvasx(event.x)
        canvas_y = self.sprite_canvas.canvasy(event.y)
        x, y = int(canvas_x // self.zoom), int(canvas_y // self.zoom)
        
        if 0 <= x < self.width and 0 <= y < self.height:
            index = self.image.getpixel((x, y))
            if index == 0:
                return # The index 0 is ignored when using the pipette.
                
            # On récupère l'index directement depuis l'image PIL
            self.current_color_index = index
            self.selected_label.config(text=f"Index sélectionné : {self.current_color_index}")
            self.draw_palette()
            self._update_rgb_entries()

    def on_sprite_press(self, event):
        """Starts recording a new track for the history."""
        if self._is_shift_pressed(event):
            self.current_undo_data = None
            self.on_sprite_click(event)
            return

        self.current_undo_data = {}
        self.on_sprite_click(event)

    def on_sprite_release(self, event):
        """Closes the current path and adds it to the undo stack."""
        if self.current_undo_data:
            self.undo_stack.append(self.current_undo_data)
            self.redo_stack.clear()
        self.current_undo_data = None

    def undo(self, event=None):
        """Undoes the last trace by restoring pixel by pixel."""
        if self.undo_stack:
            changes = self.undo_stack.pop()
            redo_changes = {}
            for (x, y), old_index in changes.items():
                redo_changes[(x, y)] = self.image.getpixel((x, y))
                self.image.putpixel((x, y), old_index)
                tag = f"pixel_{x}_{y}"
                self.sprite_canvas.delete(tag)
                self._draw_pixel_to_canvas(self.sprite_canvas, x, y, self.zoom, old_index, tag)
            self.redo_stack.append(redo_changes)

    def redo(self, event=None):
        """Reapply the most recently undone change set."""
        if self.redo_stack:
            changes = self.redo_stack.pop()
            undo_changes = {}
            for (x, y), new_index in changes.items():
                undo_changes[(x, y)] = self.image.getpixel((x, y))
                self.image.putpixel((x, y), new_index)
                tag = f"pixel_{x}_{y}"
                self.sprite_canvas.delete(tag)
                self._draw_pixel_to_canvas(self.sprite_canvas, x, y, self.zoom, new_index, tag)
            self.undo_stack.append(undo_changes)

    def on_sprite_click(self, event):
        """Modify the pixel under the mouse on the sprite."""
        # Converting event coordinates into actual canvas coordinates
        # (takes into account the scroll offset)
        canvas_x = self.sprite_canvas.canvasx(event.x)
        canvas_y = self.sprite_canvas.canvasy(event.y)
        x, y = int(canvas_x // self.zoom), int(canvas_y // self.zoom)
        
        if 0 <= x < self.width and 0 <= y < self.height:
            if self._is_shift_pressed(event):
                self.select_connected_cells(x, y)
                return

            old_index = self.image.getpixel((x, y))
            if old_index == self.current_color_index:
                return

            # Pixel recording before modification (only once per path)
            if self.current_undo_data is not None and (x, y) not in self.current_undo_data:
                self.current_undo_data[(x, y)] = old_index

            self.image.putpixel((x, y), self.current_color_index)
            # We redraw the specific pixel (we delete the old one because 223 uses multiple objects)
            tag = f"pixel_{x}_{y}"
            self.sprite_canvas.delete(tag)
            self._draw_pixel_to_canvas(self.sprite_canvas, x, y, self.zoom, self.current_color_index, tag)

    def save_image(self):
        """Save the modified image."""
        ext = ".png"

        path = filedialog.asksaveasfilename(
            initialfile=os.path.basename(self.image_path),
            defaultextension=ext,
            filetypes=[("PNG", "*.png")]
        )
        
        if path:
            # The self.image object is maintained in 'P' (Palette) mode.
            # Pillow therefore automatically saves in 8 bits indexed with the current palette.
            try:
                self.image.info["transparency"] = 223
                self.image.save(path)
                messagebox.showinfo("Success", f"Image saved in 8-bit indexed format in:\n{path}")
            except Exception as e:
                messagebox.showerror("Error", f"Error on saving : {e}")

if __name__ == "__main__":
    root = tk.Tk()

    if len(sys.argv) >= 2:
        image_path = sys.argv[1]
    else:
        root.withdraw()
        image_path = filedialog.askopenfilename(
            title="Select a PNG sprite",
            filetypes=[("PNG", "*.png")]
        )
        if not image_path:
            root.destroy()
            sys.exit(0)
        root.deiconify()

    app = SpriteEditor(root, image_path)
    root.mainloop()