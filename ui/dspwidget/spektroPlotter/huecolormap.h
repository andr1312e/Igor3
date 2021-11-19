#ifndef HUECOLORMAP_H
#define HUECOLORMAP_H
#include "qwt_color_map.h"

class HueColorMap: public QwtColorMap
{
public:
    // class backported from Qwt 6.2

    HueColorMap();

    ~HueColorMap();

    virtual QRgb rgb( const QwtInterval &interval, double value ) const;

    virtual unsigned char colorIndex( const QwtInterval &, double ) const;

private:
    void UpdateTable();

    int d_hue1, d_hue2, d_saturation, d_value;
    QRgb d_rgbMin, d_rgbMax, d_rgbTable[360];
};
#endif // HUECOLORMAP_H
