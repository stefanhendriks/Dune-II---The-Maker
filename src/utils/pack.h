#pragma once

#include <string>
#include <vector>
#include <memory>
#include <algorithm>


#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

struct FileInPack {
    std::string name;
    std::string fileId;
    std::string extension;
    uint32_t fileOffset;
    uint32_t fileSize;
};

// **********************
//
// ReaderPack : read a pack file
//
// **********************
class ReaderPack
{
public:
    //! Create and read PackFile archive filename
    ReaderPack(const std::string &filename);
    ~ReaderPack();
    //! get raw ressources from PackFile by this index in the archive
    SDL_RWops * getData(unsigned int index);
    //! get index from ressource named fileId
    int getIndexFromName(const std::string &fileId) const;
    //! print all files in Pack
    void displayPackFile();
    //! get the number of files in the pack
    int getNumberOfFiles() const { return fileInPak; }
private:
    std::vector<FileInPack> fileInPack;
    bool readHeader();
    void readFileLines();
    void readDataIntoMemory();
    std::unique_ptr<char[]> fileInMemory = nullptr; 
    std::unique_ptr<SDL_RWops, decltype(&SDL_RWclose)> rfp{nullptr, SDL_RWclose};  //rfp as readFilePack
    std::string fpName;
    int fileInPak = 0;
    uint64_t sizeInMemory =0;
};

class WriterPack
{
public:
    //! Create the archive skeleton
    WriterPack(const std::string &packName);
    ~WriterPack();
    //! add individual file fileName in archive and rename it by fileId 
    bool addFile(const std::string &fileName, const std::string &fileId);
    //! Create PackFile after add all files in archive
    bool writePackFilesOnDisk();
    //! for debug: display files in pack
    void displayPackFile();
private:
    void writeHeader();
    void writeFileLines();
    void copyFile();
    int numberFile = 0;
    std::unique_ptr<SDL_RWops, decltype(&SDL_RWclose)> wfp{nullptr, SDL_RWclose}; //wfp as writeFilePack
    std::vector<FileInPack> fileInPack;
};


// **********************
//
// DataPack
//
// A DataPack is composed by an header and data 
//
// The header is organized like this
// - title: on 4 bytes
// - number of files: on 2 bytes (uint16_t)
// One line for each file that contains
// - its identifier on 40 bytes
// - its extension on 4 bytes
// - its position in the data : offset on 4 bytes (uint32_t)
// - the size of the file : sizeFile on 4 bytes (uint32_t) 
//
// Data are organized like this
// file0 offset: 0          to    0+sizeFile0 <-- offset1
// file1 offset: offset1    to    offset1 + sizeFile1 <--- offset2
// file2 offset: offset2    to    offset2 + sizeFile2 <--- offset3
// file3 offset: offset3    to    offset3 + sizeFile3 <--- offset4
// ...
//
// **********************
class DataPack
{
public:
    // read pack packName and load it on memory
    DataPack(const std::string &packName);
    ~DataPack();
    // return a surface from his index in pack
    SDL_Surface *getSurface(int index);
    // return a surface from his name
    SDL_Surface *getSurface(const std::string &name);
    // return a music from his index in pack
    Mix_Music *getMusic(int index);
    // return a music from his name
    Mix_Music *getMusic(const std::string &name);
    // return a sample from his index in pack
    Mix_Chunk *getSample(int index);
    // return a sample from his name
    Mix_Chunk *getSample(const std::string &name);
    // for debug
    void displayPackFile();
private:
    std::unique_ptr<ReaderPack> reader;
};
