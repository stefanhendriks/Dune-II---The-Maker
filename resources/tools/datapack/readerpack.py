import struct
import os
import sys

from dataclasses import dataclass

@dataclass
class FileInPack:
    fileId: str
    extension: str
    offset: int
    size: int

class ReaderPack:
    def __init__(self, filename):
        self.filename = filename
        self.fileInPack = []
        self.fileInPak = 0
        self._read_header()

    def _read_header(self):
        with open(self.filename, 'rb') as f:
            title = f.read(4)
            self.fileInPak = int.from_bytes(f.read(2), 'little')
            self.header_size = 4 + 2 + self.fileInPak * 52
            for _ in range(self.fileInPak):
                fileId = f.read(40).decode('utf-8').rstrip('\x00')
                extension = f.read(4).decode('utf-8').rstrip('\x00')
                offset = int.from_bytes(f.read(4), 'little')
                size = int.from_bytes(f.read(4), 'little')
                self.fileInPack.append(FileInPack(fileId, extension, offset, size))

    def get_data(self, index):
        entry = self.fileInPack[index]
        with open(self.filename, 'rb') as f:
            f.seek(self.header_size + entry.offset)
            return f.read(entry.size)

    def get_index_from_name(self, fileId):
        for i, entry in enumerate(self.fileInPack):
            if entry.fileId == fileId:
                return i
        return -1

    def display_pack_file(self):
        for entry in self.fileInPack:
            print(f"{entry.fileId}.{entry.extension} - Offset: {entry.offset}, Size: {entry.size}")

    def get_number_of_files(self):
        return self.fileInPak

    def extract_all_files(self, output_dir):
        os.makedirs(output_dir, exist_ok=True)
        with open(self.filename, 'rb') as f:
            for entry in self.fileInPack:
                f.seek(self.header_size + entry.offset)
                data = f.read(entry.size)
                out_name = f"{entry.fileId}.{entry.extension.lower()}" if entry.extension else entry.fileId
                out_path = os.path.join(output_dir, out_name)
                with open(out_path, 'wb') as out_file:
                    out_file.write(data)
                    

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 main.py <packfile>")
        sys.exit(1)

    packfile = sys.argv[1]
    if not os.path.isfile(packfile):
        print(f"Erreur : fichier '{packfile}' introuvable.")
        sys.exit(1)

    reader = ReaderPack(packfile)
    print(f"Nombre de fichiers dans le pack : {reader.get_number_of_files()}")
    reader.display_pack_file()
    print("Extraction en cours...")
    reader.extract_all_files(".")
    print("Extraction terminée.")

if __name__ == "__main__":
    main()
