
#include <SDL2/SDL_mixer.h>
#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include <unordered_map>

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

class ReaderPack;

class DataPack {
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
    // for memory management
    std::unordered_map<int, SDL_Surface *> surfaceCache;
    std::unordered_map<int, Mix_Music *> musicCache;
    std::unordered_map<int, Mix_Chunk *> sampleCache;
};
