#ifndef UI_COLORSSCHEME_GRADIENTINFO_GRADIENTLABEL_H
#define UI_COLORSSCHEME_GRADIENTINFO_GRADIENTLABEL_H

#include <QLabel>
#include <QPainter>

#include "enums/colorscheme/rangesliderpointscount.h"

#include "ui/colorscheme/basiccolors.h"

class GradientLabel : public QLabel {
    Q_OBJECT
public:
    explicit GradientLabel(QWidget *widget);
    ~GradientLabel();
    const QVector<QColor> &GetColors() const;
    const QVector<int>& GetRanges() const;
    void SetRangesAndColors(const QVector<QColor> &colors, const QVector<int> &ranges);
    void SetColors(const QVector<QColor> &colors);
public  Q_SLOTS:
    void OnRangesChange(QVector<int> &ranges);
    void SetDefaultRanges(RangeSliderPointsCount count);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QVector<QColor> colorsVector;
    QVector<int> rangesVector;

};

#endif // UI_COLORSSCHEME_GRADIENTINFO_GRADIENTLABEL_H
