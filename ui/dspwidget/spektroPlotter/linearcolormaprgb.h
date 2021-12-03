#ifndef LINEARCOLORMAPRGB_H
#define LINEARCOLORMAPRGB_H
#include <qwt_color_map.h>
#include <QVector>
using ColorsMap = QVector<QPair<float, QColor>>;

class LinearColorMapRGB: public QwtLinearColorMap
{
public:
    explicit  LinearColorMapRGB();
    explicit LinearColorMapRGB(ColorsMap colorMap);
    ~LinearColorMapRGB();
};
#endif // LINEARCOLORMAPRGB_H
