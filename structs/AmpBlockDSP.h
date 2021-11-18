#ifndef AMPBLOCKDSP_H
#define AMPBLOCKDSP_H

#include <QVector>
#include <QtGlobal>

#include "structs/POIDistSpeedPortraitHeader.h"

struct AmpBlockDSP {
    POIDistSpeedPortraitHeader header;
    QVector<qreal> data;
};


#endif // AMPBLOCKDSP_H
