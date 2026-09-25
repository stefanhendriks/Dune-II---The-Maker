/**
 * @file cPack.h
 *
 * Dune 2 - The Maker
 *
 * original @author Mira.
 * This modified file is now for Stefan Hendriks & the D2TM Team
 * 
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <algorithm>


#include <SDL3/SDL.h>

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
class ReaderPack {
public:
    //! Create and read PackFile archive filename
    ReaderPack(const std::string &filename);
    ~ReaderPack();
    //! get raw ressources from PackFile by this index in the archive
    SDL_IOStream *getData(unsigned int index);
    //! get index from ressource named fileId
    int getIndexFromName(const std::string &fileId) const;
    //! print all files in Pack
    void displayPackFile();
    //! get the number of files in the pack
    int getNumberOfFiles() const {
        return fileInPak;
    }
private:
    std::vector<FileInPack> fileInPack;
    bool readHeader();
    void readFileLines();
    void readDataIntoMemory();
    std::unique_ptr<char[]> fileInMemory = nullptr;
    std::unique_ptr<SDL_IOStream, decltype(&SDL_CloseIO)> rfp{nullptr, SDL_CloseIO};  //rfp as readFilePack
    std::string fpName;
    int fileInPak = 0;
    uint64_t sizeInMemory =0;
};

class WriterPack {
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
    std::unique_ptr<SDL_IOStream, decltype(&SDL_CloseIO)> wfp{nullptr, SDL_CloseIO}; //wfp as writeFilePack
    std::vector<FileInPack> fileInPack;
};

