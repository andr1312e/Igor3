#ifndef RAWBLOCKDSP_H
#define RAWBLOCKDSP_H

#include "structs/POIDistSpeedPortraitHeader.h"

struct RawBlockDSP {
    POIDistSpeedPortraitHeader header;
    short *data;
};

#endif // RAWBLOCKDSP_H
