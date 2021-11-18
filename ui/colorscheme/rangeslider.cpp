#include "rangeslider.h"
#include <QDebug>


RangeSlider::RangeSlider(const QVector<int> &doubleHandlesVector, const QVector<int> &tripleHandlesVector, const QVector<int> &quadrupleHandlesVector, QWidget *parent)
    : QWidget(parent)
    , m_doubleHandlesVector(doubleHandlesVector)
    , m_tripleHandlesVector(tripleHandlesVector)
    , m_quadrupleHandlesVector(quadrupleHandlesVector)
    , m_currentMarkSelected(RangeSliderMarkSelected::FirstMark)
    , m_pointCounts(RangeSliderPointsCount::DoubleHandles)
{
    setMouseTracking(true);
    SetDefaultRanges(m_pointCounts);
}

void RangeSlider::paintEvent(QPaintEvent* paintEvent)
{
    qDebug()<<m_handlesValue;
    Q_UNUSED(paintEvent);
    QPainter painter(this);

    // Background
    QRect backgroundRect(m_leftRightMargin, (height() - m_sliderBarHeight) / 2, width() - m_leftRightMargin * 2, m_sliderBarHeight);

    QPen pen(Qt::gray, 0.8); // background Rect
    painter.setPen(pen);
    QBrush backgroundBrush(Qt::gray);
    painter.setBrush(backgroundBrush);
    painter.drawRoundedRect(backgroundRect, 1, 1);

    // First value handle rect
    pen.setColor(Qt::darkGray);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    QBrush handleBrush(QColor(0xFA, 0xFA, 0xFA));
    painter.setBrush(handleBrush);
    for (int index = 0; index < m_handlesValue.count(); ++index) {
        painter.drawRoundedRect(GetHandleRect(index), 2, 2);
    }

    QFont font;
    font.setFamily("Calibri");
    font.setPixelSize(20);
    painter.setPen(QPen(QColor(145, 145, 145, 200), 1, Qt::SolidLine, Qt::FlatCap));
    painter.setFont(font);

    for (int index = 0; index < m_handlesValue.count(); ++index) {
        int textMargin;
        if (index % 2 == 0)
        {
            textMargin = -20;
        }
        else
        {
            textMargin = 20;
        }
        QRect currentRect(GetHandleRect(index));
        currentRect.moveBottom(currentRect.bottom() + textMargin);
        QRect boundingRect;
        painter.drawText(currentRect, Qt::AlignCenter, QString::number(m_handlesValue.at(index)), &boundingRect);
        painter.drawText(boundingRect, Qt::AlignCenter, QString::number(m_handlesValue.at(index)));
    }

}

QRect RangeSlider::GetHandleRect(int index) const
{
    float percentage = (m_handlesValue.at(index)) * 1.0 / (m_maximumRange - m_minimumRange);
    int aValue = percentage * validLength() + m_leftRightMargin + m_handleSideLength * index;

    return QRect(aValue, (height() - m_handleSideLength) / 2., m_handleSideLength, m_handleSideLength);
}

void RangeSlider::mousePressEvent(QMouseEvent* mousePressEvent)
{
    if(mousePressEvent->buttons() && Qt::LeftButton)
    {
        qDebug()<< "localPos" << mousePressEvent->localPos();
        float localXCoordinate = mousePressEvent->localPos().x() ;
        for (int hangleIndex = 0; hangleIndex < m_handlesValue.count(); ++hangleIndex) {
            QRectF currentRect = GetHandleRect(hangleIndex);
            qDebug()<< "currentRect" << hangleIndex << " = "<<currentRect;
            int handleRectXCoordinate =  currentRect.x();
            if (currentRect.contains(mousePressEvent->localPos()))
            {
                m_currentMarkSelected = static_cast<RangeSliderMarkSelected>(hangleIndex);
                qDebug()<< "selected" << m_currentMarkSelected;
                m_HandleDelta = localXCoordinate - (handleRectXCoordinate + m_handleSideLength / 2.0);
                qDebug()<< "m_HandleDelta" << m_HandleDelta;
                return;
            }
        }
        m_currentMarkSelected = NoMarkSelected;
    }
}

void RangeSlider::mouseMoveEvent(QMouseEvent* mouseMoveEvent)
{
    if(mouseMoveEvent->buttons() & Qt::LeftButton)
    {
        int counter = 0;
        if (m_currentMarkSelected != NoMarkSelected) {
            float posValue = mouseMoveEvent->localPos().x() ;

            QPair<int, int> currentLimit = limits();
            auto &currentHandle = m_handlesValue[m_currentMarkSelected];

            if(posValue - m_HandleDelta + m_handleSideLength / 2. > currentLimit.second) {
                if (m_currentMarkSelected == SecondMark && m_pointCounts == DoubleHandles ||
                        m_currentMarkSelected == ThirdMark  && m_pointCounts == TripleHandles ||
                        m_currentMarkSelected == FourthMark ) {
                    currentHandle = m_maximumRange;
                    //qDebug() << counter++ << currentHandle << markSelected;
                }
                else {
                    currentHandle = m_handlesValue.at(m_currentMarkSelected + 1) - 1;
                    //qDebug() << counter++ << currentHandle << markSelected;
                }
            }
            else if (posValue - m_HandleDelta - m_handleSideLength / 2. < currentLimit.first){
                if (m_currentMarkSelected == FirstMark) {
                    currentHandle = m_minimumRange;
                    //qDebug() << counter++ << currentHandle << markSelected;
                }
                else {
                    currentHandle = m_handlesValue.at(m_currentMarkSelected - 1) + 1;
                    //qDebug() << counter++ << currentHandle << markSelected;
                }
            }
            else {
                currentHandle = (posValue - m_HandleDelta - m_leftRightMargin - m_handleSideLength / 2 - m_handleSideLength * m_currentMarkSelected) * 1.0 / validLength() * (m_maximumRange - m_minimumRange) + m_minimumRange;
                //qDebug() << counter++ << currentHandle << markSelected;
                if (m_currentMarkSelected != FirstMark) {
                    if (currentHandle <= m_handlesValue.at(m_currentMarkSelected - 1)) {
                        currentHandle = m_handlesValue.at(m_currentMarkSelected - 1) + 1;
                        //qDebug() << counter++ << currentHandle << markSelected;
                    }
                }
            }
            Q_EMIT ToRangesChange(m_handlesValue);
            update();
        }
    }
}

void RangeSlider::mouseReleaseEvent(QMouseEvent* mouseReleaseEvent)
{
    Q_UNUSED(mouseReleaseEvent);


    m_currentMarkSelected = NoMarkSelected;
}

QSize RangeSlider::minimumSizeHint() const
{
    return QSize(5 * m_handleSideLength * m_handlesValue.count() + m_leftRightMargin * 2, m_leftRightMargin * m_handleSideLength);
}

void RangeSlider::SetRangesVector(const QVector<int> &ranges)
{
    int rangesCount=ranges.count();
    switch (rangesCount) {
    case 2:
    case 3:
    case 4:
    {
        m_handlesValue=ranges;
        m_pointCounts=static_cast<RangeSliderPointsCount>(rangesCount);
        break;
    }
    default:
        qFatal("Недопустимое значение RangeSlider::SetPoints");
    }
    update();
}

void RangeSlider::SetDefaultRanges(RangeSliderPointsCount count)
{
    m_pointCounts=count;
    switch (m_pointCounts) {
    case DoubleHandles:
        m_handlesValue=m_doubleHandlesVector;
        break;
    case TripleHandles:
        m_handlesValue=m_tripleHandlesVector;
        break;
    case QuadrupleHandles:
        m_handlesValue=m_quadrupleHandlesVector;
        break;
    default:
        qFatal("Недопустимое значение RangeSlider::SetDefaultRanges");
    }
    update();
}


int RangeSlider::validLength() const
{
    int len = width();
    return len - m_leftRightMargin * 2 - m_handleSideLength * m_handlesValue.count();
}

QPair<int, int> RangeSlider::limits() const
{
    int max = 1.0 * validLength() + m_leftRightMargin;
    QPair <float, float> result;
    switch (m_currentMarkSelected)
    {
    case FirstMark :
    {
        QRectF rightHandle = GetHandleRect(m_currentMarkSelected + 1);
        result.first = m_leftRightMargin;
        result.second = rightHandle.x() ;
        break;
    }
    case SecondMark :
    {
        QRectF leftHandle(GetHandleRect(m_currentMarkSelected - 1)), rightHandle( (m_pointCounts > DoubleHandles) ? GetHandleRect(m_currentMarkSelected + 1) : QRectF());
        result.first =  leftHandle.right();
        result.second = ( m_pointCounts > DoubleHandles ? rightHandle.x() : max + m_handleSideLength * m_handlesValue.count());
        break;
    }
    case ThirdMark :
    {
        QRectF leftHandle(GetHandleRect(m_currentMarkSelected - 1)), rightHandle( (m_pointCounts > TripleHandles) ? GetHandleRect(m_currentMarkSelected + 1) : QRectF());
        result.first = leftHandle.right() ;
        result.second = ( m_pointCounts > TripleHandles ? rightHandle.x() : max + m_handleSideLength * m_handlesValue.count());
        break;
    }
    case FourthMark :
    {
        QRectF leftHandle(GetHandleRect(m_currentMarkSelected - 1));
        result.first =leftHandle.right() ;
        result.second = max + m_handleSideLength * m_handlesValue.count();
        break;
    }
    case NoMarkSelected :
    {
        break;
    }
    default :
    {
        qFatal("Поведение не определено");
    }
    }
    return result;
}
