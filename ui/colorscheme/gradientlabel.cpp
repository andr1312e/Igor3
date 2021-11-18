#include "gradientlabel.h"
#include <QDebug>


GradientLabel::GradientLabel(const QVector<int> &doubleHandlesVector, const QVector<int> &tripleHandlesVector, const QVector<int> &quadrupleHandlesVector, const QVector<QColor> &doubleHandlesColorVector, const QVector<QColor> &tripleHandlesColorVector, const QVector<QColor> &quadrupleHandlesColorVector, QWidget *widget)
    : QLabel(widget)
    , m_doubleHandlesVector(doubleHandlesVector)
    , m_tripleHandlesVector(tripleHandlesVector)
    , m_quadrupleHandlesVector(quadrupleHandlesVector)
    , m_doubleHandlesColorVector(doubleHandlesColorVector)
    , m_tripleHandlesColorVector(tripleHandlesColorVector)
    , m_quadrupleHandlesColorVector(quadrupleHandlesColorVector)
    , colorsVector(tripleHandlesColorVector)
    , rangesVector(tripleHandlesVector)
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

void GradientLabel::SetColors(QVector<QColor> &colors)
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
        colorsVector=m_doubleHandlesColorVector;
        rangesVector=m_doubleHandlesVector;
        break;
    case TripleHandles:
        colorsVector=m_tripleHandlesColorVector;
        rangesVector=m_tripleHandlesVector;
        break;
    case QuadrupleHandles:
        colorsVector=m_quadrupleHandlesColorVector;
        rangesVector=m_quadrupleHandlesVector;
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
