#include "pack.h"
#include <assert.h>


#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
#include <memory>

const int fileNameSize = 40;
const int titleSize = 4;    // "D2TM"
const int offsetSize = 4+4; // two uint32_t
const int nbrFilesSize = 2; // one uint16_t
const int extensionSize = 4; // "WAV", "JPG", ...

// **********************
//
// struct fileInPack
//
// **********************





ReaderPack::ReaderPack(const std::string &filename)
{
    fpName = filename;
    if (readHeader()) {
        sizeInMemory = SDL_RWsize(rfp.get()) - titleSize - nbrFilesSize - (fileNameSize+offsetSize)*fileInPak;
        fileInMemory = std::make_unique<char[]>(sizeInMemory);
        readFileLines();
    }
    readDataIntoMemory();
}

ReaderPack::~ReaderPack()
{
    fileInPack.clear();
}

void ReaderPack::readFileLines()
{
    for(int i=0; i<fileInPak; i++ ) {
        // file name
        char fileID[fileNameSize] {'\0'};
        if (SDL_RWread(rfp.get(), fileID, fileNameSize, 1) != 1) {
            std::cerr << "Failed to read file ID for file " << i << std::endl;
            return;
        }
        // file extension
        char rawExt[4] = {0};
        if (SDL_RWread(rfp.get(), rawExt, extensionSize, 1) != 1) {
            std::cerr << "Failed to read extension for file " << i << std::endl;
            return;
        }

        // index offset
        uint32_t offsetFile;
        if (SDL_RWread(rfp.get(), reinterpret_cast<char *>(&offsetFile), sizeof(offsetFile), 1) != 1) {
            std::cerr << "Failed to read file offset for file " << i << std::endl;
            return;
        }
        // index size
        uint32_t sizeFile;
        if (SDL_RWread(rfp.get(), reinterpret_cast<char *>(&sizeFile), sizeof(sizeFile), 1) != 1) {
            std::cerr << "Failed to read file size for file " << i << std::endl;
            return;
        }

        FileInPack tmp;
        tmp.name = std::string(fileID);
        tmp.fileId = std::string(fileID);
        tmp.extension = std::string(rawExt);
        tmp.fileSize = sizeFile;
        tmp.fileOffset = offsetFile;
        fileInPack.push_back(tmp);
    }
}

bool ReaderPack::readHeader()
{
    rfp.reset(SDL_RWFromFile(fpName.c_str(), "rb"));
    if (!rfp) {
        std::cerr << "Failed to open file: " << fpName << " - " << SDL_GetError() << std::endl;
        return false;
    }
    char title[4+1]={'\0'};
    if (SDL_RWread(rfp.get(), &title, 4, 1) != 1 || strcmp(title, "D2TM") != 0) {
        std::cerr << "Invalid file format: " << fpName << std::endl;
        rfp.reset();
        return false;
    }

    uint16_t nbrFiles;
    if (SDL_RWread(rfp.get(), reinterpret_cast<char *>(&nbrFiles), sizeof(nbrFiles), 1) !=1) {
        std::cerr << "Failed to read number of files: " << fpName << std::endl;
        rfp.reset();
        return false;
    }
    fileInPak = nbrFiles;
    return true;
}

int ReaderPack::getIndexFromName(const std::string &fileId) const
{
    auto it = std::find_if( fileInPack.begin(), fileInPack.end(),
    [&fileId](const FileInPack& element) {
        return element.fileId == fileId;
    } );
    if (it != fileInPack.end())
        return std::distance(fileInPack.begin(), it);
    else
        return -1;
}

SDL_RWops *ReaderPack::getData(unsigned int index)
{
    if (index >= fileInPack.size()) {
        std::cerr << "Invalid index: " << index << std::endl;
        return nullptr;
    }
    return SDL_RWFromMem( &fileInMemory[fileInPack[index].fileOffset], fileInPack[index].fileSize );
};

void ReaderPack::displayPackFile()
{
    std::cout << "------------------------------" << std::endl;
    std::cout << "File(s) stored in archive " << fpName << std::endl;
    std::cout << "[Name] -> [ext] -> size -> offset" << std::endl;
    std::cout << "------------------------------" << std::endl;
    for (unsigned int i=0; i<fileInPack.size(); i++ ) {
        std::cout << '[' << fileInPack[i].fileId << "] -> ["<< fileInPack[i].extension << "] -> " << std::setw(8) << fileInPack[i].fileSize << " -> " << std::setw(8) << fileInPack[i].fileOffset << std::endl;
    }
    std::cout << "EOF" << std::endl;
}

void ReaderPack::readDataIntoMemory()
{
    uint16_t firstData = titleSize + nbrFilesSize + (fileNameSize+extensionSize+offsetSize) * fileInPak;
    SDL_RWseek(rfp.get(), firstData, RW_SEEK_SET );
    SDL_RWread(rfp.get(), fileInMemory.get(), sizeInMemory, 1);
}

// **********************
//
// WriterPack create a pack file from scratch
//
// **********************


WriterPack::WriterPack(const std::string &packName)
{
    wfp.reset(SDL_RWFromFile(packName.c_str(), "wb"));
}

WriterPack::~WriterPack()
{
    fileInPack.clear();
}

bool WriterPack::addFile(const std::string &fileName, const std::string &fileId)
{
    SDL_RWops *file = SDL_RWFromFile(fileName.c_str(),"rb");
    if (file!=nullptr) {

        FileInPack tmp;
        tmp.name = fileName;
        tmp.fileId = fileId;
        std::string ext = fileName.substr(fileName.find_last_of('.') + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
        tmp.extension = ext.substr(0, 3);
        tmp.fileSize = SDL_RWsize(file);
        tmp.fileOffset = 0;
        fileInPack.push_back(tmp);

        SDL_RWclose(file);
        return true;
    }
    else
        return false;
}

void WriterPack::writeHeader()
{
    const char *str = "D2TM";
    size_t len = SDL_strlen(str);
    if (SDL_RWwrite(wfp.get(), str, 1, len) != len) {
        printf("Couldn't fully write string\n");
    }
    u_int16_t nbFiles = fileInPack.size();
    SDL_RWwrite(wfp.get(), reinterpret_cast<const char *>(&nbFiles), sizeof(u_int16_t), 1);
}


void WriterPack::writeFileLines()
{
    uint32_t offset = 0;
    for (const auto &tmp : fileInPack) {
        // Create a temporary buffer to group the data
        char buffer[fileNameSize + extensionSize + sizeof(uint32_t) + sizeof(uint32_t)] = {0};
        // Copy fileId to buffer (limited to fileNameSize)
        strncpy(buffer, tmp.fileId.c_str(), fileNameSize);
        // Copy extension to buffer (limited to extensionSize)
        strncpy(buffer + fileNameSize, tmp.extension.c_str(), extensionSize);
        // Copy offset into buffer
        memcpy(buffer + fileNameSize + extensionSize, &offset, sizeof(uint32_t));
        // Copy fileSize to buffer
        memcpy(buffer + fileNameSize + extensionSize + sizeof(uint32_t), &tmp.fileSize, sizeof(uint32_t));
        // Write the buffer in one operation
        if (SDL_RWwrite(wfp.get(), buffer, 1, sizeof(buffer)) != sizeof(buffer)) {
            std::cerr << "Failed to write file line for: " << tmp.fileId << std::endl;
        }
        // Update the offset for the next file
        offset += tmp.fileSize;
    }
}

void WriterPack::displayPackFile()
{
    std::cout << "------------------------------" << std::endl;
    std::cout << "File(s) stored in archive " << std::endl;
    std::cout << "[fileId] -> [extension] -> size" << std::endl;
    std::cout << "------------------------------" << std::endl;
    for (const auto &tmp : fileInPack) {
        std::cout << "\t[" << tmp.fileId << "] -> [" << tmp.extension << "] -> " << std::setw(8) << tmp.fileSize << std::endl;
    }
    std::cout << "EOF" << std::endl;
}

void WriterPack::copyFile()
{
    for (const auto &tmp : fileInPack) {
        std::unique_ptr<SDL_RWops, decltype(&SDL_RWclose)> wf(SDL_RWFromFile(tmp.name.c_str(), "rb"), SDL_RWclose);
        if (!wf) {
            std::cerr << "Failed to open file: " << tmp.name << " - " << SDL_GetError() << std::endl;
            continue; // Passer au fichier suivant
        }
        Sint64 res_size = SDL_RWsize(wf.get());
        if (res_size <= 0) {
            std::cerr << "Invalid file size for: " << tmp.name << std::endl;
            continue;
        }

        std::vector<char> buffer(res_size);
        Sint64 nb_read = SDL_RWread(wf.get(), buffer.data(), 1, res_size);
        if (nb_read != res_size) {
            std::cerr << "Failed to read file: " << tmp.name << std::endl;
            continue;
        }

        Sint64 nb_written = SDL_RWwrite(wfp.get(), buffer.data(), 1, res_size);
        if (nb_written != res_size) {
            std::cerr << "Failed to write file: " << tmp.name << std::endl;
        }
    }
}

bool WriterPack::writePackFilesOnDisk()
{
    writeHeader();
    writeFileLines();
    copyFile();
    displayPackFile();
    return true;
}
