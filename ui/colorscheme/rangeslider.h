#ifndef RANGESLIDER_H
#define RANGESLIDER_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QRectF>

#include "enums/rangesliderpointscount.h"
#include "enums/SliderHandleSelected.h"

class RangeSlider : public QWidget
{
    Q_OBJECT
public:
    RangeSlider(const QVector<int> &DoubleHandlesVector, const QVector<int> &tripleHandlesVector, const QVector<int> &quadrupleHandlesVector, QWidget *parent);
Q_SIGNALS:
    void ToRangesChange(QVector<int> &ranges);
public:
    QSize minimumSizeHint() const override;
    void SetRangesVector(const QVector<int> &ranges);
    void SetDefaultRanges(RangeSliderPointsCount count);

protected:
    void paintEvent(QPaintEvent* paintEvent) override;
    void mousePressEvent(QMouseEvent* mousePressEvent) override;
    void mouseMoveEvent(QMouseEvent* mouseMoveEvent) override;
    void mouseReleaseEvent(QMouseEvent* mouseReleaseEvent) override;
    QRect GetHandleRect(int index) const;

private:
    const int m_handleSideLength = 12;
    const int m_sliderBarHeight = 6;
    const int m_leftRightMargin = 10;
    const int m_minimumRange = 0;
    const int m_maximumRange = 100;

    const QVector<int> m_doubleHandlesVector;
    const QVector<int> m_tripleHandlesVector;
    const QVector<int> m_quadrupleHandlesVector;

private:

    int validLength() const;
    QPair<int, int> limits() const;

    RangeSliderMarkSelected m_currentMarkSelected;
    RangeSliderPointsCount m_pointCounts;

    float m_HandleDelta=0;

    QVector<int> m_handlesValue={};
};


#endif // RANGESLIDER_H
