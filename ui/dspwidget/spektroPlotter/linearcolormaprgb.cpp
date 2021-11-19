#include "linearcolormaprgb.h"

LinearColorMapRGB::LinearColorMapRGB()
    : QwtLinearColorMap( Qt::black, Qt::red, QwtColorMap::RGB )
{

}

LinearColorMapRGB::LinearColorMapRGB(ColorsMap colorMap)
    : QwtLinearColorMap( colorMap.first().second, colorMap.last().second, QwtColorMap::RGB )
{
    for (auto &item : colorMap) {
        addColorStop(item.first, item.second);
    }
    setMode(Mode::ScaledColors);
}

LinearColorMapRGB::~LinearColorMapRGB()
{

}
