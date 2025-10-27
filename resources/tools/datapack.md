### How to create DataPack ?

## Prepare data
As of 1/10/2026, datapacks are uncompressed containers of files, placed end to end according to the following description, taken from the header of `utils/cDataPack.hpp`

```C++
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
```

# first step
- Place all your data files in a directory.
- Put the `main.cpp` file inside.
Currently, the DataPack manages image, sound, and music files compatible with the SD2 library and SDL2_Audio, i.e., everything that can be read by the `IMG_Load_RW`, `Mix_LoadMUS_RW`, and `Mix_LoadWAV_RW` functions.

# next step
- Open the `main.cpp` file and fill in the vector `std::vector<std::string> tmpFiles` in the order the files should be stored in the container.
- Modify `const std::string fileName = "data_pack.dat";` if you want to customize output name file.

# end step
- compile `main.cpp` an execute it.