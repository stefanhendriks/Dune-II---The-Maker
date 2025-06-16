#include <assert.h>


#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
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
    SDL_RWops * getData(int index);
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

ReaderPack::ReaderPack(const std::string &filename)
{
    fpName = filename;
    if (readHeader()){
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
        char fileID[fileNameSize]{'\0'};
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
        if (SDL_RWread(rfp.get(), reinterpret_cast<char*>(&offsetFile), sizeof(offsetFile), 1) != 1) {
            std::cerr << "Failed to read file offset for file " << i << std::endl;
            return;
        } 
        // index size
        uint32_t sizeFile;
        if (SDL_RWread(rfp.get(), reinterpret_cast<char*>(&sizeFile), sizeof(sizeFile), 1) != 1) {
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
    char title[4];
    if (SDL_RWread(rfp.get(), &title, 4, 1) != 1 || strcmp(title, "D2TM") != 0) {
        std::cerr << "Invalid file format: " << fpName << std::endl;
        rfp.reset();
        return false;
    }

    uint16_t nbrFiles;
    if (SDL_RWread(rfp.get(), reinterpret_cast<char*>(&nbrFiles), sizeof(nbrFiles), 1) !=1) {
        std::cerr << "Failed to read number of files: " << fpName << std::endl;
        rfp.reset();
        return false;
    }
    fileInPak = nbrFiles;
    return true;    
}

int ReaderPack::getIndexFromName(const std::string &fileId) const {
    auto it = std::find_if( fileInPack.begin(), fileInPack.end(),
        [&fileId](const FileInPack& element){ return element.fileId == fileId;} );
    if (it != fileInPack.end())
        return std::distance(fileInPack.begin(), it);
    else
        return -1;
}

SDL_RWops* ReaderPack::getData(int index) {
    if (index < 0 || index >= fileInPack.size()) {
        std::cerr << "Invalid index: " << index << std::endl;
        return nullptr;
    }    
    return SDL_RWFromMem( &fileInMemory[fileInPack[index].fileOffset], fileInPack[index].fileSize );
};

void ReaderPack::displayPackFile() {
    std::cout << "------------------------------" << std::endl;
    std::cout << "File(s) stored in archive " << fpName << std::endl;
    std::cout << "[Name] -> [ext] -> size -> offset" << std::endl;
    std::cout << "------------------------------" << std::endl;
    for (auto i=0; i<fileInPack.size(); i++ ) {
        std::cout << '[' << fileInPack[i].fileId << "] -> ["<< fileInPack[i].extension << "] -> " << std::setw(8) << fileInPack[i].fileSize << " -> " << std::setw(8) << fileInPack[i].fileOffset << std::endl;
    }
    std::cout << "EOF" << std::endl;
}

void ReaderPack::readDataIntoMemory()
{
    uint16_t firstData = titleSize + nbrFilesSize + (fileNameSize+extensionSize+offsetSize) * fileInPak;
    SDL_RWseek(rfp.get(), firstData, RW_SEEK_SET );
    SDL_RWread(rfp.get(), fileInMemory.get() , sizeInMemory, 1);
}

// **********************
//
// WriterPack create a pack file from scratch
//
// **********************
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
    } else
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
    SDL_RWwrite(wfp.get(), reinterpret_cast<const char*>(&nbFiles), sizeof(u_int16_t), 1);
}


void WriterPack::writeFileLines()
{
    uint32_t offset = 0;
    for (const auto& tmp : fileInPack) {
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
    for (const auto& tmp : fileInPack) {
        std::cout << "\t[" << tmp.fileId << "] -> [" << tmp.extension << "] -> " << std::setw(8) << tmp.fileSize << std::endl;
    }
    std::cout << "EOF" << std::endl;
}

void WriterPack::copyFile()
{
    for (const auto& tmp : fileInPack) {
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
    // for debug
    void displayPackFile();
private:
    std::unique_ptr<ReaderPack> reader;
};

DataPack::DataPack(const std::string &packName)
{
    reader = std::make_unique<ReaderPack>(packName);
}

DataPack::~DataPack()
{
    reader.reset();
}

void DataPack::displayPackFile()
{
    reader->displayPackFile();
}

SDL_Surface *DataPack::getSurface(int index)
{
    if (index < 0 || index >= reader->getNumberOfFiles()) {
        std::cerr << "Invalid index: " << index << std::endl;
        return nullptr;
    }
    SDL_RWops *tmp = reader->getData(index);
    SDL_Surface *out = SDL_LoadBMP_RW(tmp, SDL_TRUE);
    if (!out) {
        printf("Failed to load image %i : %s\n", index, SDL_GetError());
    }
    return out;
}

Mix_Music *DataPack::getMusic(int index)
{
    if (index < 0 || index >= reader->getNumberOfFiles()) {
        std::cerr << "Invalid index: " << index << std::endl;
        return nullptr;
    }
    SDL_RWops *tmp = reader->getData(index);
    Mix_Music *out = Mix_LoadMUS_RW(tmp, SDL_TRUE);
    if (!out) {
        printf("Failed to load music %i : %s\n", index, SDL_GetError());
    }
    return out;
}

SDL_Surface *DataPack::getSurface(const std::string &name)
{
    int index = reader->getIndexFromName(name);
    return this->getSurface(index);
}

Mix_Music *DataPack::getMusic(const std::string &name)
{
    int index = reader->getIndexFromName(name);
    return this->getMusic(index);
}

int main(int argc, char ** argv)
{
    // First we create a pak file from scratch
    if (1) {
        // write pak file.
    	WriterPack test("test1.pak");
        //
        test.addFile("test1.bmp","test1");
        test.addFile("test2.bmp","test2");
        test.addFile("test3.bmp","test3");
        //
        test.writePackFilesOnDisk();
    }    

    if (1) {
        // write pak file with audio file.
    	WriterPack test("audio1.pak");
        //
        test.addFile("DUNE1_4.mid","audio1");
        test.addFile("DUNE1_5.mid","audio2");
        test.addFile("DUNE1_6.mid","audio3");
        test.addFile("DUNE1_7.mid","audio4");
        //
        test.writePackFilesOnDisk();
    } 

    bool quit = false;
    SDL_Event event;
 
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    //Initialize SDL2_mixer to play sound
    if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
    {
        printf("SDL2_mixer could not be initialized!\n"
               "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    SDL_Window * window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);

    //-----------------------------------------------------------------
    //load with DataPack
    //-----------------------------------------------------------------
    DataPack dataRead("test1.pak");
    SDL_Surface *surface1 = dataRead.getSurface(0);
    SDL_Surface *surface2 = dataRead.getSurface("test2");
    SDL_Surface *surface3 = dataRead.getSurface(2);
    dataRead.displayPackFile();
    //-----------------------------------------------------------------

    //convert SDL_Surface to SDL_Texture
    SDL_Texture * texture1 = SDL_CreateTextureFromSurface(renderer, surface1);    // GPU memory
    SDL_Texture * texture2 = SDL_CreateTextureFromSurface(renderer, surface2);    // GPU memory
    SDL_Texture * texture3 = SDL_CreateTextureFromSurface(renderer, surface3);    // GPU memory
    SDL_FreeSurface(surface1);
    SDL_FreeSurface(surface2);
    SDL_FreeSurface(surface3);

    SDL_Color color = { 255, 255, 255 };
    
    SDL_Texture * texture = texture1;

    //-----------------------------------------------------------------
    //load with DataPack and play it
    //-----------------------------------------------------------------
    DataPack dataAudio("audio1.pak");
    Mix_Music * music = dataAudio.getMusic(2);
    Mix_PlayMusic( music, -1 );
    dataAudio.displayPackFile();

    while (!quit)
    {
        SDL_WaitEvent(&event);
 
        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_a){
                    texture = texture1;
		        }
                if (event.key.keysym.sym == SDLK_z){
                    texture = texture2;
		        }
                if (event.key.keysym.sym == SDLK_e){
                    texture = texture3;
		        }
                if (event.key.keysym.sym == SDLK_q){
                    Mix_FreeMusic(music);
                    music = dataAudio.getMusic("audio1");
                    Mix_PlayMusic( music, -1 );
		        }
                if (event.key.keysym.sym == SDLK_s){
                    Mix_FreeMusic(music);
                    music = dataAudio.getMusic("audio2");
                    Mix_PlayMusic( music, -1 );
		        }
                if (event.key.keysym.sym == SDLK_d){
                    Mix_FreeMusic(music);
                    music = dataAudio.getMusic("audio3");
                    Mix_PlayMusic( music, -1 );
		        }
                if (event.key.keysym.sym == SDLK_f){
                    Mix_FreeMusic(music);
                    music = dataAudio.getMusic("audio4");
                    Mix_PlayMusic( music, -1 );
		        }                
                if (event.key.keysym.sym == SDLK_ESCAPE){
                    quit = true;
		        }
                break;
        }
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    Mix_HaltMusic();
    Mix_FreeMusic(music);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
 
    return 0;
}

