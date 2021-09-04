#include "d2tmh.h"
#include "cAllegroDataRepository.h"

cAllegroDataRepository::cAllegroDataRepository() {
    for (int i = 0; i < MAX_BITMAPS; i++) {
        m_data[i].bitmap = nullptr;
        m_data[i].ownsIt = false;
    }
}

/**
 * Delete this before unloading Allegro!
 */
cAllegroDataRepository::~cAllegroDataRepository() {
    for (int i = 0; i < MAX_BITMAPS; i++) {
        if (m_data[i].bitmap) {
            if (m_data[i].ownsIt) {
                destroy_bitmap(m_data[i].bitmap);
            } else {
                m_data[i].bitmap = nullptr;
            }
        }
    }
}

int cAllegroDataRepository::findAvailableSlot() {
    for (int i = 0; i < MAX_BITMAPS; i++) {
        if (m_data[i].bitmap) continue;
        return i;
    }
    return -1;
}

int cAllegroDataRepository::loadBitmap(char *file) {
    int slot = findAvailableSlot();
    if (slot < 0) {
        // unable to find available slot
        return -1;
    }

    BITMAP *pBitmap = load_bitmap(file, nullptr);
    if (!pBitmap) {
        // unable to load
        return -2;
    }

    m_data[slot].bitmap = pBitmap;
    m_data[slot].ownsIt = true;
    return slot;
}

bool cAllegroDataRepository::loadBitmapAt(int index, const char *file) {
    if (index < 0) return false;
    if (index >= MAX_BITMAPS) return false;

    if (m_data[index].bitmap) {
        char msg[255];
        sprintf(msg, "cAllegroDataRepository::loadBitmapAt() Unable to load bitmap at %s index %d, because it has already been taken.", file, index);
        logbook(msg);

        // already taken!
        return false;
    }

    BITMAP *pBitmap = load_bitmap(file, nullptr);
    if (!pBitmap) {
        char msg[255];
        sprintf(msg, "cAllegroDataRepository::loadBitmapAt() Unable to load bitmap %s at index %d, cannot find it or invalid format.", file, index);
        logbook(msg);
        // unable to load
        return false;
    }

    if (DEBUGGING) {
        char msg[255];
        sprintf(msg, "cAllegroDataRepository::loadBitmapAt() Loaded bitmap %s at index %d", file, index);
        logbook(msg);
    }

    m_data[index].bitmap = pBitmap;
    m_data[index].ownsIt = true;
    return true;
}

bool cAllegroDataRepository::loadBitmapFromDataFileAt(int index, BITMAP *bmp) {
    if (index < 0) return false;
    if (index >= MAX_BITMAPS) return false;

    if (m_data[index].bitmap) {
        char msg[255];
        sprintf(msg, "cAllegroDataRepository::loadBitmapFromDataFileAt() Unable to load bitmap at index %d, because it has already been taken.", index);
        logbook(msg);

        // already taken!
        return false;
    }

    if (!bmp) {
        char msg[255];
        sprintf(msg, "cAllegroDataRepository::loadBitmapFromDataFileAt() Unable to load bitmap at index %d, the provided bmp is null!", index);
        logbook(msg);
        // unable to load
        return false;
    }

    if (DEBUGGING) {
        char msg[255];
        sprintf(msg, "cAllegroDataRepository::loadBitmapFromDataFileAt() Loaded bitmap from pointer at index %d", index);
        logbook(msg);
    }

    m_data[index].bitmap = bmp;
    m_data[index].ownsIt = false;
    return true;
}

sBitmap * cAllegroDataRepository::getBitmapAt(int index) {
    if (index < 0 || index >= MAX_BITMAPS) return nullptr;
    if (!m_data[index].bitmap) return nullptr; // don't return bitmap stuff when nothing initialized

    return &m_data[index];
}

bool cAllegroDataRepository::loadBitmapFromDataFileGfxDataAt(int index, int indexAtDatafile) {
    return loadBitmapFromDataFileAt(index, (BITMAP *)gfxdata[indexAtDatafile].dat);
}
