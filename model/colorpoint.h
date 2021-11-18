#ifndef COLORPOINT_H
#define COLORPOINT_H

#include <QColor>
#include <QJsonObject>
#include <QPoint>

class ColorPoint
{
public:
    ColorPoint(QPoint p);
    ~ColorPoint();

    void SaveState(QJsonObject &saveState);

    void LoadState(QJsonObject &loadState);

    QColor color;
    QPoint point;
    qreal angle; // в радианах
    bool secondaryOn;
};

#endif // COLORPOINT_H
