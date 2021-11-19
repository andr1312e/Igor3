#ifndef COLORRANGES_H
#define COLORRANGES_H

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVector>

struct ColorRanges {
    ColorRanges() :
        pointsForCirclePalette(),
        colorsForGradientLabel(),
        rangesForRangeSlider(),
        defaultPoints({QPoint(80,160), QPoint(250,40), QPoint(420,140), QPoint(410, 260)}),
        defaultColors({QColor("#13ed1b"), QColor("#f8f008"), QColor("#fb0505"), QColor("#e6199b")})
    {}
    ColorRanges(QVector<QPoint> points, QVector<QColor> colors, QVector<int> ranges) :
        pointsForCirclePalette(points),
        colorsForGradientLabel(colors),
        rangesForRangeSlider(ranges),
        defaultPoints({QPoint(80,160), QPoint(250,40), QPoint(420,140), QPoint(410, 260)}),
        defaultColors({QColor("#13ed1b"), QColor("#f8f008"), QColor("#fb0505"), QColor("#e6199b")})
    {}
    void defaultInit(const quint8 &pointsNumber) {
        pointsForCirclePalette.clear();
        colorsForGradientLabel.clear();
        rangesForRangeSlider.clear();
        for (quint8 index = 0; index != pointsNumber; index++ ) {
            pointsForCirclePalette.push_back(defaultPoints.at(index));
            colorsForGradientLabel.push_back(defaultColors.at(index));
        }
        if (pointsNumber == 2) {
            rangesForRangeSlider << 0 << 100;
        }
        else if (pointsNumber == 3) {
            rangesForRangeSlider << 0 << 50 << 100;
        }
        else if (pointsNumber == 4) {
            rangesForRangeSlider << 0 << 33 << 66 << 100;
        }
    }
    void pop_back() {
        if (pointsForCirclePalette.size() == 2)
            return;
        pointsForCirclePalette.pop_back();
        colorsForGradientLabel.pop_back();
        rangesForRangeSlider.pop_back();
    }

    void push_back() {//240 - 240
        const qint16 vectorSize = colorsForGradientLabel.size();
        if (vectorSize == 4)
            return;
        QSet<QString> currentColorsSet;
        for (quint8 index = 0; index != colorsForGradientLabel.size(); index++) {
            currentColorsSet.insert(colorsForGradientLabel.at(index).name());
        }

        quint8 indexForPushing = 0;
        while (indexForPushing != defaultColors.size()) {
            auto itemColorSet = currentColorsSet.find(defaultColors.at(indexForPushing).name());
            if (itemColorSet != currentColorsSet.end())
                indexForPushing++;
            else
                break;
        }
        if (indexForPushing != defaultColors.size()) {
            colorsForGradientLabel.push_back(defaultColors.at(indexForPushing));
            pointsForCirclePalette.push_back(defaultPoints.at(indexForPushing));
        }
        else {
            colorsForGradientLabel.push_back(QColor("#808080"));
            pointsForCirclePalette.push_back(QPoint(239,235));
        }

        int lastElement = *(rangesForRangeSlider.end() - 1),
                preLastElement = *(rangesForRangeSlider.end() - 2),
                newEndElement = 100;
        if (lastElement > 90) {
            if ((lastElement - preLastElement) < 10) {
                lastElement -= 10;
                preLastElement -= 10;
            }
            else {
                lastElement -= 10;
            }
        }
        rangesForRangeSlider[rangesForRangeSlider.size() - 1] = lastElement;
        rangesForRangeSlider[rangesForRangeSlider.size() - 2] = preLastElement;
        rangesForRangeSlider.push_back(newEndElement);
    }

    QVector<QPoint> pointsForCirclePalette;
    QVector<QColor> colorsForGradientLabel;
    QVector<int> rangesForRangeSlider;

private:
    QVector<QPoint> defaultPoints;
    QVector<QColor> defaultColors;

};

#endif // COLORRANGES_H
