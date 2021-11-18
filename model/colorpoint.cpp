#include "colorpoint.h"

ColorPoint::ColorPoint(QPoint p)
{
    this->point = p;
    this->angle = 0;
    this->secondaryOn = false; // secondaries are off by default
    this->color = QColor();
}

ColorPoint::~ColorPoint()
{

}

void ColorPoint::SaveState(QJsonObject &saveState)
{
    saveState[QStringLiteral("secondary")] = this->secondaryOn;
    saveState[QStringLiteral("x")] = this->point.x();
    saveState[QStringLiteral("y")] = this->point.y();
}

void ColorPoint::LoadState(QJsonObject &loadState)
{
    this->secondaryOn = loadState["secondary"].toBool();
    this->point.setX(loadState["x"].toInt());
    this->point.setY(loadState["y"].toInt());
}
