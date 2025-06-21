#include "cAllegroDataRepository.h"

#include "d2tmc.h"

#include <allegro/datafile.h>
#include <allegro/gfx.h>
#include <fmt/core.h>

cAllegroDataRepository::cAllegroDataRepository()
{
    for (int i = 0; i < MAX_BITMAPS; i++) {
        m_data[i] = nullptr;
    }
}

/**
 * Delete this before unloading Allegro!
 */
cAllegroDataRepository::~cAllegroDataRepository()
{
    for (int i = 0; i < MAX_BITMAPS; i++) {
        m_data[i] = nullptr;
    }
}

int cAllegroDataRepository::findAvailableSlot()
{
    for (int i = 0; i < MAX_BITMAPS; i++) {
        if (m_data[i]) continue;
        return i;
    }
    return -1;
}

// int cAllegroDataRepository::loadBitmap(char *file) {
//     int slot = findAvailableSlot();
//     if (slot < 0) {
//         // unable to find available slot
//         return -1;
//     }

//     SDL_Surface *pBitmap = load_bitmap(file, nullptr);
//     if (!pBitmap) {
//         // unable to load
//         return -2;
//     }

//     m_data[slot].bitmap = pBitmap;
//     m_data[slot].ownsIt = true;
//     return slot;
// }

// bool cAllegroDataRepository::loadBitmapAt(int index, const char *file) {
//     if (index < 0) return false;
//     if (index >= MAX_BITMAPS) return false;

//     if (m_data[index].bitmap) {
//         logbook(fmt::format(
//             "cAllegroDataRepository::loadBitmapAt() Unable to load bitmap at {} index {}, because it has already been taken.", file, index));

//         // already taken!
//         return false;
//     }

//     SDL_Surface *pBitmap = load_bitmap(file, nullptr);
//     if (!pBitmap) {
//         logbook(fmt::format(
//             "cAllegroDataRepository::loadBitmapAt() Unable to load bitmap {} at index {}, cannot find it or invalid format.", file, index));
//         // unable to load
//         return false;
//     }

//     if (game.isDebugMode()) {
//         logbook(fmt::format("cAllegroDataRepository::loadBitmapAt() Loaded bitmap {} at index {}", file, index));
//     }

//     m_data[index].bitmap = pBitmap;
//     m_data[index].ownsIt = true;
//     return true;
// }

bool cAllegroDataRepository::loadBitmapFromDataFileAt(int index, SDL_Surface *bmp)
{
    if (index < 0) return false;
    if (index >= MAX_BITMAPS) return false;

    if (m_data[index]) {
        logbook(fmt::format(
                    "cAllegroDataRepository::loadBitmapFromDataFileAt() Unable to load bitmap at index {}, because it has already been taken.", index));

        // already taken!
        return false;
    }

    if (!bmp) {
        logbook(fmt::format(
                    "cAllegroDataRepository::loadBitmapFromDataFileAt() Unable to load bitmap at index {}, the provided bmp is null!", index));
        // unable to load
        return false;
    }

    if (game.isDebugMode()) {
        logbook(fmt::format(
                    "cAllegroDataRepository::loadBitmapFromDataFileAt() Loaded bitmap from pointer at index {}", index));
    }

    m_data[index] = bmp;
    return true;
}

SDL_Surface *cAllegroDataRepository::getBitmapAt(int index)
{
    if (index < 0 || index >= MAX_BITMAPS) return nullptr;
    return m_data[index];
}

bool cAllegroDataRepository::loadBitmapFromDataFileGfxDataAt(int index, int indexAtDatafile)
{
    return loadBitmapFromDataFileAt(index, gfxdata->getSurface(indexAtDatafile));
}
