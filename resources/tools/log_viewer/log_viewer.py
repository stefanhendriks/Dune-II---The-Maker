import tkinter as tk
from tkinter import ttk, filedialog, messagebox

class LogViewer:
    def __init__(self, root):
        self.root = root
        self.root.title("C++20 Log Analyzer - Professional Edition")
        self.root.geometry("1400x800")

        self.all_logs = []  
        self.components_list = ["ALL"]
        self.levels_list = ["ALL", "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"]
        
        self.setup_ui()
        self.setup_styles()

    def setup_ui(self):
        # Top Toolbar for Filters
        top_frame = ttk.Frame(self.root, padding="10")
        top_frame.pack(fill=tk.X, side=tk.TOP)

        # 1. File Loading
        ttk.Button(top_frame, text="Open Log File", command=self.open_file).grid(row=0, column=0, padx=5)

        # 2. Level Filter
        ttk.Label(top_frame, text="Level:").grid(row=0, column=1, padx=5)
        self.level_var = tk.StringVar(value="ALL")
        self.level_combo = ttk.Combobox(top_frame, textvariable=self.level_var, values=self.levels_list, state="readonly", width=10)
        self.level_combo.grid(row=0, column=2, padx=5)
        self.level_combo.bind("<<ComboboxSelected>>", lambda e: self.apply_filter())

        # 3. Component Filter
        ttk.Label(top_frame, text="Component:").grid(row=0, column=3, padx=5)
        self.comp_var = tk.StringVar(value="ALL")
        self.comp_combo = ttk.Combobox(top_frame, textvariable=self.comp_var, values=self.components_list, state="readonly", width=18)
        self.comp_combo.grid(row=0, column=4, padx=5)
        self.comp_combo.bind("<<ComboboxSelected>>", lambda e: self.apply_filter())

        # 4. Event Search
        ttk.Label(top_frame, text="Event Filter:").grid(row=0, column=5, padx=5)
        self.event_search_var = tk.StringVar()
        self.event_search_var.trace_add("write", lambda *args: self.apply_filter())
        ttk.Entry(top_frame, textvariable=self.event_search_var, width=20).grid(row=0, column=6, padx=5)

        # 5. Message Search
        ttk.Label(top_frame, text="Message Filter:").grid(row=0, column=7, padx=5)
        self.msg_search_var = tk.StringVar()
        self.msg_search_var.trace_add("write", lambda *args: self.apply_filter())
        ttk.Entry(top_frame, textvariable=self.msg_search_var, width=20).grid(row=0, column=8, padx=5)

        # Main Table (Treeview)
        self.columns = ("timestamp", "level", "component", "event", "message", "player", "house")
        self.tree = ttk.Treeview(self.root, columns=self.columns, show='headings')
        
        headers = ["Timestamp", "Level", "Component", "Event", "Message", "Player ID", "House"]
        widths = [80, 80, 120, 150, 500, 80, 80] 
        
        for col, head, w in zip(self.columns, headers, widths):
            self.tree.heading(col, text=head, command=lambda c=col: self.sort_by(c, False))
            # Align Event and Message to Left (West), others Center
            anchor_point = tk.W if col in ["event", "message"] else tk.CENTER
            self.tree.column(col, width=w, anchor=anchor_point)
        
        # Scrollbars
        v_scroll = ttk.Scrollbar(self.root, orient=tk.VERTICAL, command=self.tree.yview)
        h_scroll = ttk.Scrollbar(self.root, orient=tk.HORIZONTAL, command=self.tree.xview)
        self.tree.configure(yscrollcommand=v_scroll.set, xscrollcommand=h_scroll.set)
        
        v_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        h_scroll.pack(side=tk.BOTTOM, fill=tk.X)
        self.tree.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

    def setup_styles(self):
        self.tree.tag_configure("FATAL", background="#ffcccc", foreground="red")
        self.tree.tag_configure("ERROR", foreground="red")
        self.tree.tag_configure("WARN", foreground="#b36b00")
        self.tree.tag_configure("INFO", foreground="black")
        self.tree.tag_configure("DEBUG", foreground="gray")
        self.tree.tag_configure("TRACE", foreground="#aaaaaa")

    def open_file(self):
        file_path = filedialog.askopenfilename(filetypes=[("Log files", "*.txt *.log"), ("All files", "*.*")])
        if not file_path: return

        try:
            with open(file_path, "r", encoding="utf-8") as f:
                lines = f.readlines()
            
            self.all_logs = []
            comps = set()
            for line in lines:
                if not line.strip() or line.startswith("\\\\"): continue
                parts = line.strip().split('|')
                if len(parts) >= 7:
                    row = parts[:7]
                    self.all_logs.append(row)
                    comps.add(row[2])
            
            self.components_list = ["ALL"] + sorted(list(comps))
            self.comp_combo['values'] = self.components_list
            self.apply_filter()
        except Exception as e:
            messagebox.showerror("Error", f"Could not read file: {e}")

    def apply_filter(self):
        event_q = self.event_search_var.get().lower()
        msg_q = self.msg_search_var.get().lower()
        sel_level = self.level_var.get()
        sel_comp = self.comp_var.get()
        
        self.tree.delete(*self.tree.get_children())
        
        for row in self.all_logs:
            level_match = (sel_level == "ALL" or row[1] == sel_level)
            comp_match = (sel_comp == "ALL" or row[2] == sel_comp)
            
            # Focused search on Event (index 3) AND Message (index 4)
            event_match = (event_q == "" or event_q in str(row[3]).lower())
            msg_match = (msg_q == "" or msg_q in str(row[4]).lower())
            
            if level_match and comp_match and event_match and msg_match:
                self.tree.insert("", tk.END, values=row, tags=(row[1],))

    def sort_by(self, col, reverse):
        data = [(self.tree.set(k, col).lower(), k) for k in self.tree.get_children('')]
        data.sort(reverse=reverse)
        for index, (val, k) in enumerate(data):
            self.tree.move(k, '', index)
        self.tree.heading(col, command=lambda: self.sort_by(col, not reverse))

if __name__ == "__main__":
    root = tk.Tk()
    app = LogViewer(root)
    root.mainloop()