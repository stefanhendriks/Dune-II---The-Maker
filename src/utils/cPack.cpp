/**
 * @file cPack.cpp
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

#include "cPack.h"
#include "include/cAssert.h"
#include "utils/Log.h"


#include <string>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
#include <array>
#include <bit>
#include <string_view>
#include <memory>

const int fileNameSize = 40;
const int titleSize = 4;    // "D2TM" (pack without header) or "D2TH" (pack with author and version)
const int authorSize = 15;  // NUL padded text, only in "D2TH" packs
const int versionSize = 5;  // decimal digits, NUL terminated (4+1), only in "D2TH" packs
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
        sizeInMemory = SDL_GetIOSize(rfp.get()) - headerSize - (fileNameSize+extensionSize+offsetSize)*fileInPak;
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
        std::array<char, fileNameSize> fileID {};
        if (SDL_ReadIO(rfp.get(), fileID.data(), fileNameSize) != fileNameSize) {
            std::cerr << "Failed to read file ID for file " << i << std::endl;
            return;
        }
        // file extension
        std::array<char, 4> rawExt {};
        if (SDL_ReadIO(rfp.get(), rawExt.data(), extensionSize) != extensionSize) {
            std::cerr << "Failed to read extension for file " << i << std::endl;
            return;
        }

        // index offset
        uint32_t offsetFile;
        if (SDL_ReadIO(rfp.get(), reinterpret_cast<char *>(&offsetFile), sizeof(offsetFile)) != sizeof(offsetFile)) {
            std::cerr << "Failed to read file offset for file " << i << std::endl;
            return;
        }
        // index size
        uint32_t sizeFile;
        if (SDL_ReadIO(rfp.get(), reinterpret_cast<char *>(&sizeFile), sizeof(sizeFile)) != sizeof(sizeFile)) {
            std::cerr << "Failed to read file size for file " << i << std::endl;
            return;
        }

        FileInPack tmp;
        tmp.name = std::string(fileID.data());
        tmp.fileId = std::string(fileID.data());
        tmp.extension = std::string(rawExt.data());
        tmp.fileSize = sizeFile;
        tmp.fileOffset = offsetFile;
        const auto inserted = nameToIndex.emplace(tmp.fileId, static_cast<int>(fileInPack.size()));
        if (!inserted.second) {
            // lookups by name keep returning the first entry
            std::cerr << "Warning: duplicate entry name '" << tmp.fileId << "' in " << fpName
                      << " (index " << fileInPack.size() << " ignored for name lookup, first is index "
                      << inserted.first->second << ")" << std::endl;
            Logger::warn(COMP_INIT, "ReaderPack", "Duplicate entry name '{}' in {} (index {} ignored for name lookup, first is index {})",
                         tmp.fileId, fpName, fileInPack.size(), inserted.first->second);
        }
        fileInPack.push_back(tmp);
    }
}

bool ReaderPack::readHeader()
{
    rfp.reset(SDL_IOFromFile(fpName.c_str(), "rb"));
    if (!rfp) {
        std::cerr << "Failed to open file: " << fpName << " - " << SDL_GetError() << std::endl;
        return false;
    }
    std::array<char, 4> title {};
    if (SDL_ReadIO(rfp.get(), title.data(), 4) != 4) {
        std::cerr << "Invalid file format: " << fpName << std::endl;
        rfp.reset();
        return false;
    }
    const std::string_view titleView(title.data(), title.size());
    headerSize = titleSize + nbrFilesSize;
    if (titleView == "D2TH") {
        std::array<char, authorSize> rawAuthor {};
        std::array<char, versionSize> rawVersion {};
        if (SDL_ReadIO(rfp.get(), rawAuthor.data(), authorSize) != authorSize ||
                SDL_ReadIO(rfp.get(), rawVersion.data(), versionSize) != versionSize) {
            std::cerr << "Failed to read pack header: " << fpName << std::endl;
            rfp.reset();
            return false;
        }
        rawAuthor.back() = '\0';
        rawVersion.back() = '\0';
        author = std::string(rawAuthor.data());
        version = static_cast<unsigned int>(std::strtoul(rawVersion.data(), nullptr, 10));
        headerSize += authorSize + versionSize;
    } else if (titleView != "D2TM") {
        // "D2TM": pack without header, author stays empty and version 0
        std::cerr << "Invalid file format: " << fpName << std::endl;
        rfp.reset();
        return false;
    }

    uint16_t nbrFiles;
    if (SDL_ReadIO(rfp.get(), reinterpret_cast<char *>(&nbrFiles), sizeof(nbrFiles)) != sizeof(nbrFiles)) {
        std::cerr << "Failed to read number of files: " << fpName << std::endl;
        rfp.reset();
        return false;
    }
    fileInPak = nbrFiles;
    Logger::info(COMP_INIT, "ReaderPack", "Pack {}: author '{}', version {}, {} files", fpName, author, version, fileInPak);
    return true;
}

int ReaderPack::getIndexFromName(const std::string &fileId) const
{
    auto it = nameToIndex.find(fileId);
    return it != nameToIndex.end() ? it->second : -1;
}

std::vector<std::string> ReaderPack::getNames() const
{
    std::vector<std::string> names;
    names.reserve(fileInPack.size());
    for (const FileInPack &file : fileInPack) {
        names.push_back(file.fileId);
    }
    return names;
}

SDL_IOStream *ReaderPack::getData(unsigned int index)
{
    if (index >= fileInPack.size()) {
        std::cerr << "Invalid index: " << index << std::endl;
        return nullptr;
    }
    return SDL_IOFromConstMem( &fileInMemory[fileInPack[index].fileOffset], fileInPack[index].fileSize );
}

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
    const uint32_t firstData = headerSize + (fileNameSize+extensionSize+offsetSize) * fileInPak;
    SDL_SeekIO(rfp.get(), firstData, SDL_IO_SEEK_SET );
    SDL_ReadIO(rfp.get(), fileInMemory.get(), sizeInMemory);
}

// **********************
//
// WriterPack create a pack file from scratch
//
// **********************


WriterPack::WriterPack(const std::string &packName)
{
    wfp.reset(SDL_IOFromFile(packName.c_str(), "wb"));
}

WriterPack::~WriterPack()
{
    fileInPack.clear();
}

void WriterPack::setHeader(const std::string &packAuthor, unsigned int packVersion)
{
    author = packAuthor.substr(0, authorSize - 1);
    version = packVersion;
    hasHeader = true;
}

bool WriterPack::addFile(const std::string &fileName, const std::string &fileId)
{
    if (fileId.size() >= fileNameSize) {
        std::cerr << "Entry name '" << fileId << "' is longer than " << fileNameSize - 1 << " characters" << std::endl;
        return false;
    }
    SDL_IOStream *file = SDL_IOFromFile(fileName.c_str(),"rb");
    if (file!=nullptr) {

        FileInPack tmp;
        tmp.name = fileName;
        tmp.fileId = fileId;
        std::string ext = fileName.substr(fileName.find_last_of('.') + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
        tmp.extension = ext.substr(0, 3);
        tmp.fileSize = SDL_GetIOSize(file);
        tmp.fileOffset = 0;
        fileInPack.push_back(tmp);

        SDL_CloseIO(file);
        return true;
    }
    else
        return false;
}

void WriterPack::writeHeader()
{
    const std::string_view str = hasHeader ? "D2TH" : "D2TM";
    if (SDL_WriteIO(wfp.get(), str.data(), str.size()) != str.size()) {
        std::cerr << "Couldn't fully write string" << std::endl;
    }
    if (hasHeader) {
        std::array<char, authorSize + versionSize> buffer {};
        std::ranges::copy(author, buffer.begin());
        const std::string versionText = std::to_string(version % 10000);
        std::ranges::copy(versionText, buffer.begin() + authorSize);
        SDL_WriteIO(wfp.get(), buffer.data(), buffer.size());
    }
    uint16_t nbFiles = fileInPack.size();
    SDL_WriteIO(wfp.get(), reinterpret_cast<const char *>(&nbFiles), sizeof(uint16_t));
}


void WriterPack::writeFileLines()
{
    uint32_t offset = 0;
    for (const auto &tmp : fileInPack) {
        // Create a temporary buffer to group the data
        std::array<char, fileNameSize + extensionSize + sizeof(uint32_t) + sizeof(uint32_t)> buffer {};
        // Copy fileId to buffer (limited to fileNameSize)
        std::copy_n(tmp.fileId.begin(), std::min<size_t>(tmp.fileId.size(), fileNameSize), buffer.begin());
        // Copy extension to buffer (limited to extensionSize)
        std::copy_n(tmp.extension.begin(), std::min<size_t>(tmp.extension.size(), extensionSize), buffer.begin() + fileNameSize);
        // Copy offset into buffer
        const auto offsetBytes = std::bit_cast<std::array<char, sizeof(uint32_t)>>(offset);
        std::ranges::copy(offsetBytes, buffer.begin() + fileNameSize + extensionSize);
        // Copy fileSize to buffer
        const auto fileSizeBytes = std::bit_cast<std::array<char, sizeof(uint32_t)>>(tmp.fileSize);
        std::ranges::copy(fileSizeBytes, buffer.begin() + fileNameSize + extensionSize + sizeof(uint32_t));
        // Write the buffer in one operation
        if (SDL_WriteIO(wfp.get(), buffer.data(), buffer.size()) != buffer.size()) {
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
        std::unique_ptr<SDL_IOStream, decltype(&SDL_CloseIO)> wf(SDL_IOFromFile(tmp.name.c_str(), "rb"), SDL_CloseIO);
        if (!wf) {
            std::cerr << "Failed to open file: " << tmp.name << " - " << SDL_GetError() << std::endl;
            continue;
        }
        Sint64 res_size = SDL_GetIOSize(wf.get());
        if (res_size <= 0) {
            std::cerr << "Invalid file size for: " << tmp.name << std::endl;
            continue;
        }

        std::vector<char> buffer(res_size);
        Sint64 nb_read = SDL_ReadIO(wf.get(), buffer.data(), res_size);
        if (nb_read != res_size) {
            std::cerr << "Failed to read file: " << tmp.name << std::endl;
            continue;
        }

        Sint64 nb_written = SDL_WriteIO(wfp.get(), buffer.data(), res_size);
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
