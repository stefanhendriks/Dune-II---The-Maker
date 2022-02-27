
#include <assert.h>


#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

// **********************
//
// ReaderPack
//
// **********************
class ReaderPack
{
public:
    ReaderPack(const std::string &filename);
    ~ReaderPack();
    SDL_RWops * getData(int index);
    int getIndexFromName(const std::string &filename);
    void listpackFile();
private:
    std::vector<std::pair<uint32_t, uint32_t>> getIndex;  //alone for performency 
    std::vector<std::pair<std::string, uint32_t>> getName;
    bool readHeader();
    void readFileLines();
    void readDataIntoMemory();
    char *fileInMemory = nullptr;
    SDL_RWops *rfp;  //wfp as readFilePack
    std::string fpName;
    int fileInPak = 0;
    uint64_t sizeInMemory =0;
    const int fileNameSize = 40;
};

ReaderPack::ReaderPack(const std::string &filename)
{
    fpName = filename;
    if (readHeader()){

        //we know the buffer size : 40+4+4 per file with fileInPak file
        sizeInMemory = SDL_RWsize(rfp) - 4 - (40+4+4)*fileInPak;
        fileInMemory = new char[sizeInMemory];
        readFileLines();
    }
    readDataIntoMemory();
}

ReaderPack::~ReaderPack()
{
    getIndex.clear();
    getName.clear();
    SDL_RWclose(rfp);
    delete[] fileInMemory;
}

void ReaderPack::readFileLines()
{
    for(int i=0; i<fileInPak; i++ ) {
        //nom
        char fileID[fileNameSize]{'\0'};
        SDL_RWread(rfp, fileID, fileNameSize, 1);
        // index offset
        uint32_t offsetFile;
        SDL_RWread(rfp, reinterpret_cast<char*>(&offsetFile), sizeof(offsetFile), 1);
        // index size
        uint32_t sizeFile;
        SDL_RWread(rfp, reinterpret_cast<char*>(&sizeFile), sizeof(sizeFile), 1);

        getName.push_back(std::pair(std::string(fileID), sizeFile));
        getIndex.push_back(std::pair(offsetFile, sizeFile));
    }
}

bool ReaderPack::readHeader()
{
    rfp= SDL_RWFromFile(fpName.c_str(),"rb");
    char title[4];
    SDL_RWread(rfp, &title, 4, 1);
    if (strcmp(title,"D2TM") !=0) {
        return false;
    } else {
        uint16_t nbrFiles;
        SDL_RWread(rfp, reinterpret_cast<char*>(&nbrFiles), sizeof(nbrFiles), 1);
        fileInPak = nbrFiles;
        //SDL_RWwrite    SDL_RWread(rw, buf, sizeof (buf), 1);
        //std::cout << "Il y a " << nbrFiles << std::endl;
        return true;    
    }
}

int ReaderPack::getIndexFromName(const std::string &filename) {
    auto it = std::find_if( getName.begin(), getName.end(),
    [&filename](const std::pair<std::string, int>& element){ return element.first == filename;} );
    if (it != getName.end())
        return std::distance(getName.begin(), it);
    else
        return -1;
}

SDL_RWops* ReaderPack::getData(int index) {
    return SDL_RWFromMem( &fileInMemory[getIndex[index].first], getIndex[index].second);
};

void ReaderPack::listpackFile() {
    assert(getIndex.size() == getName.size() );
    for (auto i=0; i<getIndex.size(); i++ ) {
        std::cout << '[' << getName[i].first << "] = " << getName[i].second << std::endl;
    }
}

void ReaderPack::readDataIntoMemory()
{
    uint16_t firstData = 6 + (40+4+4) * fileInPak;
    SDL_RWseek(rfp, firstData, RW_SEEK_SET );
    SDL_RWread(rfp, fileInMemory , sizeInMemory, 1);
}

// **********************
//
// WriterPack
//
// **********************
class WriterPack
{
public:
    WriterPack(const std::string &packName);
    ~WriterPack();
    bool addFile(const std::string &fileName);
    bool writePackFiles();
    void listpackFile();
private:
    void writeHeader();
    void writeFileLines();
    void copyFile();
    int numberFile = 0;
    SDL_RWops *wfp;  //wfp as writeFilePack
    std::map<std::string, uint32_t> mNameSize;
    const int fileNameSize = 40;
};

WriterPack::WriterPack(const std::string &packName)
{
    wfp = SDL_RWFromFile(packName.c_str(),"wb");
}

WriterPack::~WriterPack()
{
    SDL_RWclose(wfp);
    mNameSize.clear();
}

bool WriterPack::addFile(const std::string &fileName)
{
    SDL_RWops *file = SDL_RWFromFile(fileName.c_str(),"rb");
    if (file!=nullptr) {
        mNameSize[fileName] = SDL_RWsize(file);
        SDL_RWclose(file);
        return true;
    } else
        return false;
}

void WriterPack::writeHeader()
{
	const char *str = "D2TM";
    size_t len = SDL_strlen(str);
    if (SDL_RWwrite(wfp, str, 1, len) != len) {
        printf("Couldn't fully write string\n");
    }
    u_int16_t nbFiles = mNameSize.size();
    SDL_RWwrite(wfp, reinterpret_cast<const char*>(&nbFiles), sizeof(u_int16_t), 1);
}


void WriterPack::writeFileLines()
{
    uint32_t offset = 0;
    for (const auto& [key, value] : mNameSize) {
        char fileID[fileNameSize]{'\0'};
        strcpy(fileID,key.c_str());
        SDL_RWwrite(wfp, fileID, 1, fileNameSize) != fileNameSize;
        // position du fichier dans les datas
        SDL_RWwrite(wfp, reinterpret_cast<const char*>(&offset), sizeof(uint32_t), 1);
        offset += value;
        // taille du fichier dans les datas
        SDL_RWwrite(wfp, reinterpret_cast<const char*>(&value), sizeof(uint32_t), 1);     
    }
}

void WriterPack::listpackFile()
{
    for (const auto& [key, value] : mNameSize) {
        std::cout << '[' << key << "] = " << value << std::endl;
    }
}

void WriterPack::copyFile()
{
    for (const auto& [key, value] : mNameSize) {
        SDL_RWops *wf = SDL_RWFromFile(key.c_str(),"rb");
        Sint64 res_size = SDL_RWsize(wf);
        char* res = (char*)malloc(res_size + 1);

        Sint64 nb_read_total = 0, nb_read = 1;
        char* buf = res;
        while (nb_read_total < res_size && nb_read != 0) {
            nb_read = SDL_RWread(wf, buf, 1, (res_size - nb_read_total));
            SDL_RWwrite(wfp,buf,1,(res_size - nb_read_total));
            nb_read_total += nb_read;
            buf += nb_read;
        }
    }
}

bool WriterPack::writePackFiles()
{
    writeHeader();
    writeFileLines();
    copyFile();
    return true;
}


int main(int argc, char ** argv)
{
    if (1) {
        // write pak file.
    	WriterPack test("test1.pak");
        //
        test.addFile("test1.bmp");
        test.addFile("test2.bmp");
        test.addFile("test3.bmp");
        //
        test.listpackFile();
        //
        test.writePackFiles();
    }    

    bool quit = false;
    SDL_Event event;
 
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);

    // load texture normal mode
    //SDL_Surface * surface = SDL_LoadBMP("test.bmp");                            // CPU memory

    //load with pack
    ReaderPack testR("test1.pak");

    SDL_RWops *rwm1 = testR.getData(0);
    SDL_RWops *rwm2 = testR.getData(1);
    SDL_RWops *rwm3 = testR.getData(2);

    // convert file to SDL_Surface
    SDL_Surface *surface1 = SDL_LoadBMP_RW(rwm1, SDL_TRUE);
    if (!surface1) {
    printf("Failed to load image 1 : %s\n", SDL_GetError());
    }
    SDL_Surface *surface2 = SDL_LoadBMP_RW(rwm2, SDL_TRUE);
    if (!surface2) {
    printf("Failed to load image 2 : %s\n", SDL_GetError());
    }
    SDL_Surface *surface3 = SDL_LoadBMP_RW(rwm3, SDL_TRUE);
    if (!surface3) {
    printf("Failed to load image 3 : %s\n", SDL_GetError());
    }

    //convert SDL_Surface to SDL_Texture
    SDL_Texture * texture1 = SDL_CreateTextureFromSurface(renderer, surface1);    // GPU memory
    SDL_Texture * texture2 = SDL_CreateTextureFromSurface(renderer, surface2);    // GPU memory
    SDL_Texture * texture3 = SDL_CreateTextureFromSurface(renderer, surface3);    // GPU memory
    SDL_FreeSurface(surface1);
    SDL_FreeSurface(surface2);
    SDL_FreeSurface(surface3);

    SDL_Color color = { 255, 255, 255 };
    
    SDL_Texture * texture = texture1;

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
                break;
        }
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
 
    return 0;
}