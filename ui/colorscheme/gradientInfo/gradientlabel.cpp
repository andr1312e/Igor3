#include "gradientlabel.h"
#include <QDebug>


GradientLabel::GradientLabel(QWidget *widget)
    : QLabel(widget)
{
    setGeometry(0,0,100,40);
}

GradientLabel::~GradientLabel()
{

}

const QVector<QColor> &GradientLabel::GetColors() const
{
    return colorsVector;
}

const QVector<int> &GradientLabel::GetRanges() const
{
    return rangesVector;
}

void GradientLabel::SetRangesAndColors(const QVector<QColor> &colors, const QVector<int> &ranges)
{
    colorsVector=colors;
    rangesVector=ranges;
    update();
}

void GradientLabel::SetColors(const QVector<QColor> &colors)
{
    colorsVector=colors;
    update();
}

void GradientLabel::OnRangesChange(QVector<int> &ranges)
{
    rangesVector=ranges;
    update();
}

void GradientLabel::SetDefaultRanges(RangeSliderPointsCount count)
{
    switch (count) {
    case DoubleHandles:
        colorsVector=doubleHandlesColorVector;
        rangesVector=doubleHandlesVector;
        break;
    case TripleHandles:
        colorsVector=tripleHandlesColorVector;
        rangesVector=tripleHandlesVector;
        break;
    case QuadrupleHandles:
        colorsVector=quadrupleHandlesColorVector;
        rangesVector=quadrupleHandlesVector;
        break;
    default:
        qFatal("Нет такого значения");
    }
    update();
}

void GradientLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    qDebug()<< "colSize " << colorsVector.count()  << " rSize " << rangesVector.count();
    if (colorsVector.count() == rangesVector.count())
    {
        QPainter painter(this);
        const QRect widgetRect(0, 0, this->size().width(), this->size().height());

        QLinearGradient backgroundGradient(widgetRect.topLeft(), widgetRect.topRight());
        for (int index = 0; index < colorsVector.size(); ++index) {
            backgroundGradient.setColorAt(rangesVector.at(index)/100.0 , colorsVector.at(index));
        }
        painter.fillRect(widgetRect, backgroundGradient);

    }
}
