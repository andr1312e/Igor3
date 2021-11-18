#ifndef GRADIENTLABEL_H
#define GRADIENTLABEL_H

#include <QLabel>
#include <QPainter>

#include "enums/rangesliderpointscount.h"

class GradientLabel : public QLabel {
    Q_OBJECT
public:
    GradientLabel(const QVector<int> &doubleHandlesVector, const QVector<int> &tripleHandlesVector, const QVector<int> &quadrupleHandlesVector, const QVector<QColor> &doubleHandlesColorVector, const QVector<QColor> &tripleHandlesColorVector, const QVector<QColor> &quadrupleHandlesColorVector, QWidget *widget);
    ~GradientLabel();
    const QVector<QColor> &GetColors() const;
    const QVector<int>& GetRanges() const;
    void SetRangesAndColors(const QVector<QColor> &colors, const QVector<int> &ranges);
    void SetColors(QVector<QColor> &colors);
public  Q_SLOTS:
    void OnRangesChange(QVector<int> &ranges);
    void SetDefaultRanges(RangeSliderPointsCount count);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    const QVector<int> m_doubleHandlesVector;
    const QVector<int> m_tripleHandlesVector;
    const QVector<int> m_quadrupleHandlesVector;
    const QVector<QColor> m_doubleHandlesColorVector;
    const QVector<QColor> m_tripleHandlesColorVector;
    const QVector<QColor> m_quadrupleHandlesColorVector;
private:
    QVector<QColor> colorsVector;
    QVector<int> rangesVector;

};

#endif // GRADIENTLABEL_H
