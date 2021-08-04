#ifndef D2TM_CALLEGRODATAREPOSITORY_H
#define D2TM_CALLEGRODATAREPOSITORY_H

#include "allegroh.h"

struct sBitmap {
    BITMAP  *bitmap;    // loaded from Allegro
    bool ownsIt; // if set to true, this bitmap is created from the repository, and thus must also be destroyed
};

static const int D2TM_BITMAP_ICON_POWER = 0;

class cAllegroDataRepository {

public:
    static const int MAX_BITMAPS = 500;

    cAllegroDataRepository();
    ~cAllegroDataRepository();

    /**
     * Loads bitmap and returns its index
     * @param file
     * @return
     */
    int loadBitmap(char *file);
    bool loadBitmapAt(int index, char *file);
    bool loadBitmapFromDataFileAt(int index, BITMAP *bmp);
    bool loadBitmapFromDataFileGfxDataAt(int index, int indexAtDataFile);

    sBitmap * getBitmapAt(int index);

private:
    int findAvailableSlot();

    sBitmap m_data[MAX_BITMAPS];
};


#endif //D2TM_CALLEGRODATAREPOSITORY_H
