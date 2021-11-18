#ifndef POIDISTSPEEDPORTRAIT_H
#define POIDISTSPEEDPORTRAIT_H

#include "structs/POIDistSpeedPortraitHeader.h"

struct POIDistSpeedPortrait     {
    struct POIDistSpeedPortraitHeader POI_DSP_Header;
    short data[2*2512];
};

#endif // POIDISTSPEEDPORTRAIT_H
