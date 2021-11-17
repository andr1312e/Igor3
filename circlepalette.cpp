#include "circlepalette.h"

#include <QApplication>
#include <QEvent>
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QtMath>
#include <QFileDialog>
#include <QJsonArray>
#include <QBoxLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QStandardItemModel>
#include <QGuiApplication>
#include <QFile>
#include <QJsonDocument>
#include <QProgressBar>
#include <QTimer>
#include <QPushButton>

#include <algorithm>

#define LIGHTING_ICON_MIDDLE_TO_TOP 7
#define LEFT_SHIFT 60
#define TOP_SHIFT 57


namespace
{

const int scHandleSideLength = 12;
const int scSliderBarHeight = 6;
const int scLeftRightMargin = 10;

}

RangeSlider::RangeSlider(QWidget* aParent)
    : QWidget(aParent),
      markSelected(SliderHandleSelected::FirstHandle),
      mMinimum(0),
      mMaximum(100),
      orientation(Qt::Horizontal)
{
    setMouseTracking(true);
    quint16 handlesNumber = type + 1;
    float step = mMaximum / (float)handlesNumber;

    for (quint16 index = 0; index != handlesNumber; index++) {
        handlesValue << (index * step);
    }
}

RangeSlider::RangeSlider(Qt::Orientation ori, Option t, QWidget* aParent)
    : QWidget(aParent),
      markSelected(SliderHandleSelected::FirstHandle),
      mMinimum(0),
      mMaximum(100),
      orientation(ori),
      type(t)
{
    setMouseTracking(true);
    quint16 handlesNumber = type;
    float step = mMaximum / (float)handlesNumber;

    for (quint16 index = 0; index != handlesNumber + 1; index++) {
        handlesValue << (index * step);
    }
}

void RangeSlider:: setRangeType(Option t) {
    type = t;

    quint16 handlesNumber = type;
    handlesValue.clear();

    float step = mMaximum / (float)handlesNumber;

    for (quint16 index = 0; index != handlesNumber + 1; index++) {
        handlesValue << (index * step);
    }

    update();
}

void RangeSlider::setRangeType(Option t, QVector<int> vector) {
    type = t;

    handlesValue.swap(vector);

    update();
}

void RangeSlider::paintEvent(QPaintEvent* aEvent)
{
    Q_UNUSED(aEvent);
    QPainter painter(this);

    // Background
    QRectF backgroundRect;
    if(orientation == Qt::Horizontal)
        backgroundRect = QRectF(scLeftRightMargin, (height() - scSliderBarHeight) / 2, width() - scLeftRightMargin * 2, scSliderBarHeight);
    else
        backgroundRect = QRectF((width() - scSliderBarHeight) / 2, scLeftRightMargin, scSliderBarHeight, height() - scLeftRightMargin*2);

    QPen pen(Qt::gray, 0.8); // background Rect
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Qt4CompatiblePainting);
    QBrush backgroundBrush(QColor(0xD0, 0xD0, 0xD0));
    painter.setBrush(backgroundBrush);
    painter.drawRoundedRect(backgroundRect, 1, 1);

    // First value handle rect
    pen.setColor(Qt::darkGray);
    pen.setWidth(0.5);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    QBrush handleBrush(QColor(0xFA, 0xFA, 0xFA));
    painter.setBrush(handleBrush);
    for (quint16 index = 0; index != handlesValue.size(); index++) {
        painter.drawRoundedRect(handleRect(index), 2, 2);
    }

    QFont font;
    font.setFamily("Calibri");
    font.setPixelSize(20);
    painter.setPen(QPen(QColor(145, 145, 145, 200), 1, Qt::SolidLine, Qt::FlatCap));
    painter.setFont(font);

    for (quint16 index = 0; index != handlesValue.size(); index++) {
        int offset;
        if (index % 2 == 0) {
            offset = -20;
        }
        else
            offset = 20;
        QRectF currentRect(handleRect(index));
        currentRect.moveBottom(currentRect.bottom() + offset);
        QRectF boundingRect;
        painter.drawText(currentRect, Qt::AlignCenter, QString::number(handlesValue.at(index)), &boundingRect);
        painter.drawText(boundingRect, Qt::AlignCenter, QString::number(handlesValue.at(index)));
    }

}

QRectF RangeSlider::handleRect(int index) const
{
    float percentage = (handlesValue.at(index)) * 1.0 / (mMaximum - mMinimum);
    int aValue = percentage * validLength() + scLeftRightMargin + scHandleSideLength * index;

    if(orientation == Qt::Horizontal)
        return QRectF(aValue, (height() - scHandleSideLength) / 2., scHandleSideLength, scHandleSideLength);
    else
        return QRectF((width() - scHandleSideLength) / 2., aValue, scHandleSideLength, scHandleSideLength);
}

void RangeSlider::mousePressEvent(QMouseEvent* aEvent)
{
    if(aEvent->buttons() & Qt::LeftButton)
    {
        float posValue = (orientation == Qt::Horizontal) ? aEvent->localPos().x() : aEvent->localPos().y();

        markSelected = NoMarkSelected;
        for (quint16 index = 0; index < handlesValue.size(); index++) {
            QRectF currentRect = handleRect(index);
            int handleRectPosValue = (orientation == Qt::Horizontal) ? currentRect.x() : currentRect.y();
            if (currentRect.contains(aEvent->localPos())) {
                markSelected = (SliderHandleSelected)index;
                mDelta = posValue - (handleRectPosValue + scHandleSideLength / 2.);
                break;
            }
        }
    }
}

void RangeSlider::mouseMoveEvent(QMouseEvent* aEvent)
{
    if(aEvent->buttons() & Qt::LeftButton)
    {
        int counter = 0;
        if (markSelected != NoMarkSelected) {
            float posValue = (orientation == Qt::Horizontal) ? aEvent->localPos().x() : aEvent->localPos().y();

            auto currentLimit = limits();
            auto &currentHandle = handlesValue[markSelected];

            if(posValue - mDelta + scHandleSideLength / 2. > currentLimit.second) {
                if (markSelected == SecondHandle && type == DoubleHandles ||
                    markSelected == ThirdHandle  && type == TripleHandles ||
                    markSelected == FourthHandle ) {
                    currentHandle = mMaximum;
                    //qDebug() << counter++ << currentHandle << markSelected;
                }
                else {
                    currentHandle = handlesValue.at(markSelected + 1) - 1;
                    //qDebug() << counter++ << currentHandle << markSelected;
                }
            }
            else if (posValue - mDelta - scHandleSideLength / 2. < currentLimit.first){
                if (markSelected == FirstHandle) {
                    currentHandle = mMinimum;
                    //qDebug() << counter++ << currentHandle << markSelected;
                }
                else {
                    currentHandle = handlesValue.at(markSelected - 1) + 1;
                    //qDebug() << counter++ << currentHandle << markSelected;
                }
            }
            else {
                currentHandle = (posValue - mDelta - scLeftRightMargin - scHandleSideLength / 2 - scHandleSideLength * markSelected) * 1.0 / validLength() * (mMaximum - mMinimum) + mMinimum;
                //qDebug() << counter++ << currentHandle << markSelected;
                if (markSelected != FirstHandle) {
                    if (currentHandle <= handlesValue.at(markSelected - 1)) {
                        currentHandle = handlesValue.at(markSelected - 1) + 1;
                        //qDebug() << counter++ << currentHandle << markSelected;
                    }
                }

                if ((markSelected < type) && currentHandle >= handlesValue.at(markSelected + 1)) {
                    currentHandle = handlesValue.at(markSelected + 1) - 1;
                    //qDebug() << counter++ << currentHandle << markSelected;
                }
            }
            //qDebug() << currentHandle << markSelected;
            update();
        }
    }
}

void RangeSlider::mouseReleaseEvent(QMouseEvent* aEvent)
{
    Q_UNUSED(aEvent);

    emit rangeChanged(QVector<int>(handlesValue));

    markSelected = NoMarkSelected;
}

QSize RangeSlider::minimumSizeHint() const
{
    return QSize(5 * scHandleSideLength * handlesValue.size() + scLeftRightMargin * 2, 10 * scHandleSideLength);
}


int RangeSlider::validLength() const
{
    int len = (orientation == Qt::Horizontal) ? width() : height();
    return len - scLeftRightMargin * 2 - scHandleSideLength * handlesValue.size();
}

QPair<int, int> RangeSlider::limits() const
{
    int max = 1.0 * validLength() + scLeftRightMargin;
    QPair <float, float> result;
    switch (markSelected) {
    case FirstHandle : {
        QRectF rightHandle = handleRect(markSelected + 1);
        result.first = scLeftRightMargin;
        result.second = (orientation == Qt::Horizontal) ? rightHandle.x() : rightHandle.y();
        break;
    }
    case SecondHandle : {
        QRectF leftHandle(handleRect(markSelected - 1)), rightHandle( (type > DoubleHandles) ? handleRect(markSelected + 1) : QRectF());
        result.first = (orientation == Qt::Horizontal) ? leftHandle.right() : leftHandle.bottom();
        result.second = ( type > DoubleHandles ?
                         (orientation == Qt::Horizontal) ? rightHandle.x() : rightHandle.y():
                          max + scHandleSideLength * handlesValue.size());
        break;
    }
    case ThirdHandle : {
        QRectF leftHandle(handleRect(markSelected - 1)), rightHandle( (type > TripleHandles) ? handleRect(markSelected + 1) : QRectF());
        result.first = (orientation == Qt::Horizontal) ? leftHandle.right() : leftHandle.bottom();
        result.second = ( type > TripleHandles ?
                         (orientation == Qt::Horizontal) ? rightHandle.x() : rightHandle.y():
                          max + scHandleSideLength * handlesValue.size());
        break;
    }
    case FourthHandle : {
        QRectF leftHandle(handleRect(markSelected - 1));
        result.first = (orientation == Qt::Horizontal) ? leftHandle.right() : leftHandle.bottom();
        result.second = max + scHandleSideLength * handlesValue.size();
        break;
    }
    case NoMarkSelected :
        break;
    }

    return result;
}



QPoint operator*(const QPoint &x, const QPointF &y)
{
    return QPoint((qreal)x.x() * y.x(), (qreal)x.y() * y.y());
}

CirclePalette::CirclePalette(QWidget *parent)
    : QWidget(parent),
      defaultPoints({QPoint(80,160), QPoint(250,40), QPoint(420,140), QPoint(410, 260)})
{
    isDragging = false;

    circlePic = QPixmap(QString::fromUtf8(":/Resources/Icon/PrimaryHandle.png"));
    primaryRadius = (double)circlePic.width() / 2.0;

    gamutShape = PrestoPalette::GamutShapeTriangle;
    wheelShape = PrestoPalette::WheelShapeFine;

    /*backgroundWheel = new QLabel(this);
    backgroundWheel->setGeometry(QRect(44, 37, 549, 549));
    backgroundWheel->setPixmap(QPixmap(QString::fromUtf8(":/Resources/Icon/Wheel_BG.png")).scaled(backgroundWheel->size(), Qt::KeepAspectRatio));*/

    colorWheel = new QLabel();
    //colorWheel->setGeometry(QRect(70, 62, 499, 499));
    colorWheel->setPixmap(QPixmap(QString::fromUtf8(":/Resources/Icon/YWheel_Course.png")).scaled(colorWheel->size(), Qt::KeepAspectRatio));
    //colorWheel->raise();

    drawnElements = new QWidget(colorWheel);
    drawnElements->setGeometry(colorWheel->geometry());
    //drawnElements->raise();
    drawnElements->installEventFilter(this);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(colorWheel);
    setLayout(mainLayout);

    setMouseTracking(true);

    installEventFilter(this);

    create_gamut_line();
    create_gamut_triangle();
    create_gamut_square();

    centroidTriangleOn = false; // centroids are off by default
    centroidQuadOn = false; // centroids are off by default
}

CirclePalette::~CirclePalette(void)
{
    for (auto p : pointsLine)
    {
        delete p;
    }
    pointsLine.clear();

    for (auto p : pointsTriangle)
    {
        delete p;
    }
    pointsTriangle.clear();

    for (auto p : pointsQuad)
    {
        delete p;
    }
    pointsQuad.clear();

}

QPointF CirclePalette::FindIntersectionWithCircle(const QPoint &p1, const QPoint &p2, const qreal radius)
{
    qreal theta = atan2(p2.y() - p1.y(), p2.x() - p1.x());
    return QPointF(p1.x() + radius * cos(theta), p1.y() + radius * sin(theta));
}

void CirclePalette::_draw_primary_imp(QPainter &painter, const QPoint &p, int circleRadius, bool isCentroid)
{
    QPoint p_center(p.x() - circleRadius, p.y() - circleRadius);

    painter.drawPixmap(p_center, circlePic);

    if (this->isDragging && this->dragPoint != nullptr) {
        if (p == *this->dragPoint) {
            QFont font;
            font.setFamily("Calibri");
            font.setPixelSize(20);
            painter.setPen(QPen(QColor(0, 0, 0, 200), 1, Qt::SolidLine, Qt::FlatCap));
            painter.setFont(font);

            int currentIndex = 0;
            for (auto &item : *this->points) {
                if (item->point == p) {
                    break;
                }
                currentIndex++;
            }

            QPoint textPoint(p_center);
            p_center.setY(p_center.y() - circleRadius + 10);
            p_center.setX(p_center.x() + circleRadius + 10);
            QRectF currentRect(textPoint, QSize(20,20));
            QRectF boundingRect;
            painter.drawText(currentRect, Qt::AlignCenter, QString::number(currentIndex + 1), &boundingRect);
            painter.drawText(boundingRect, Qt::AlignCenter, QString::number(currentIndex + 1));
        }
    }
}

void CirclePalette::_draw_line_imp(QPainter &painter, const QPoint &p1, const QPoint &p2, int circleRadius, bool secondaryOn)
{
    // TODO Trim the lines, so they don't go into the circle

    QPen linePen(Qt::white);
    linePen.setWidth(3);
    painter.setPen(linePen);

    //trim the lines by adjusting the points
    painter.drawLine(FindIntersectionWithCircle(p1, p2, circleRadius * 2), FindIntersectionWithCircle(p2, p1, circleRadius * 2));
    //painter.drawLine(p1, p2);

    /*QPoint midpoint((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);

    painter.setPen(QPen(Qt::transparent, 1));
    if (secondaryOn == true)
    {
        painter.setBrush(QBrush(Qt::white)); //on color
    }
    else
    {
        painter.setBrush(QBrush(QColor(64, 64, 57))); //off color
    }
    painter.drawEllipse(midpoint, circleRadius, circleRadius);*/
}

void CirclePalette::CalculateCentroid(int circleRadius)
{
    QPoint c;

    for (ColorPoint *cp : *this->points)
    {
        c += cp->point;
    }
    c /= this->points->size();

    // shift over the centroid (because the above is using top-left)
    centroid->setX(c.x() - circleRadius);
    centroid->setY(c.y() - circleRadius);
}

void CirclePalette::_draw_centroid(QPainter &painter, int circleRadius, bool secondaryOn)
{
    CalculateCentroid(circleRadius);

    painter.setPen(QPen(Qt::transparent, 1));
    if (secondaryOn == true)
    {
        painter.setBrush(QBrush(Qt::white)); //on color
    }
    else
    {
        painter.setBrush(QBrush(QColor(64, 64, 57))); //off color
    }
    painter.drawEllipse(*centroid, secondaryRadius, secondaryRadius);
}

bool CirclePalette::sort_angles(const ColorPoint i, const ColorPoint j)
{
    /*if (j.is_centroid)
    {
        return -1;
    }*/

    QPoint a, b;
    a = i.point;
    b = j.point;

    QPoint center = *centroid;

    if (a.x() - center.x() >= 0 && b.x() - center.x() < 0)
        return true;
    if (a.x() - center.x() < 0 && b.x() - center.x() >= 0)
        return false;
    if (a.x() - center.x() == 0 && b.x() - center.x() == 0)
    {
        if (a.y() - center.y() >= 0 || b.y() - center.y() >= 0)
            return a.y() > b.y();
        return b.y() > a.y();
    }

    // compute the cross product of vectors (center -> a) x (center -> b)
    int det = (a.x() - center.x()) * (b.y() - center.y()) - (b.x() - center.x()) * (a.y() - center.y());
    if (det < 0)
        return true;
    if (det > 0)
        return false;

    // points a and b are on the same line from the center
    // check which point is closer to the center
    int d1 = (a.x() - center.x()) * (a.x() - center.x()) + (a.y() - center.y()) * (a.y() - center.y());
    int d2 = (b.x() - center.x()) * (b.x() - center.x()) + (b.y() - center.y()) * (b.y() - center.y());
    return d1 > d2;
}

bool sort_angles1(ColorPoint *i, ColorPoint *j)
{
    return (i->angle <= j->angle);
}

QColor CirclePalette::getColorAt(const QPoint &p)
{
    QColor color = colorWheel->pixmap()->toImage().pixelColor(p.x(), p.y());
    /*ColorPoint cp;
    cp.color = color;
    cp.point = p;

    return cp;*/
    return color;
}

QColor CirclePalette::getMidPointColor(QPoint p1, QPoint p2)
{
    return getColorAt(QPoint((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2));
}

QPoint CirclePalette::getMidPoint(QPoint &p1, QPoint &p2)
{
    return QPoint((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);
}

// https://stackoverflow.com/questions/1311049/how-to-map-atan2-to-degrees-0-360
float PointPairToBearingDegrees(QPoint startingPoint, QPoint point)
{
    QPoint endingPoint;
    endingPoint = point;
    QPoint originPoint = QPoint(endingPoint.x() - startingPoint.x(), endingPoint.y() - startingPoint.y()); // get origin point to origin by subtracting end from start

    float bearingRadians = atan2f(originPoint.y(), originPoint.x()); // get bearing in radians
    float bearingDegrees = bearingRadians * (180.0 / M_PI); // convert to degrees

    // this is for PrestoPalette
    bearingDegrees += 105.0; // to shift over to the yellow (90 degrees plus 360/24)

    bearingDegrees = (bearingDegrees > 0.0 ? bearingDegrees : (360.0 + bearingDegrees)); // correct discontinuity
    return qDegreesToRadians(bearingDegrees);
}

bool CirclePalette::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseMove && (watched == this))
    {
        const QMouseEvent *me = static_cast<const QMouseEvent *>(event);

        QPoint p = colorWheel->mapFromGlobal(QCursor::pos());
        QPoint wheelCenter = QPoint(colorWheel->width() / 2, colorWheel->height() / 2);
        int radius = colorWheel->width() / 2;

        // prevent it from going off the edge of the whole circle palette
        if (isDragging)
        {
            if (_is_collision(wheelCenter, radius - primaryRadius, p) == false)
            {
                // end it
                return true;
            }
        }

        if (_is_collision(wheelCenter, radius, p))
        {
            QColor color = colorWheel->pixmap()->toImage().pixelColor(p.x(), p.y());

            emit hoverColor(color);
        }
    }

    if (watched == drawnElements && event->type() == QEvent::Paint)
    {
        QPainter painter(drawnElements);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

        //QVector<ColorPoint> colors;

        QPoint center = QPoint(drawnElements->width() / 2.0, drawnElements->height() / 2.0);

        /* draw the lighting icon */


        if (gamutShape == PrestoPalette::GamutShapeLine)
        {
            _draw_primary_imp(painter, (*(*this->points)[0]).point, primaryRadius, false);
            _draw_primary_imp(painter, (*(*this->points)[1]).point, primaryRadius, false);
            _draw_line_imp(painter, (*(*this->points)[0]).point, (*(*this->points)[1]).point, secondaryRadius, (*(*this->points)[0]).secondaryOn);
        }

        if (gamutShape == PrestoPalette::GamutShapeTriangle)
        {
            _draw_primary_imp(painter, (*(*this->points)[0]).point, primaryRadius, false);
            _draw_line_imp(painter, (*(*this->points)[0]).point, (*(*this->points)[1]).point, secondaryRadius, (*(*this->points)[0]).secondaryOn);
            _draw_primary_imp(painter, (*(*this->points)[1]).point, primaryRadius, false);
            _draw_line_imp(painter, (*(*this->points)[1]).point, (*(*this->points)[2]).point, secondaryRadius, (*(*this->points)[1]).secondaryOn);
            _draw_primary_imp(painter, (*(*this->points)[2]).point, primaryRadius, false);
            //_draw_line_imp(painter, (*(*this->points)[2]).point, (*(*this->points)[0]).point, secondaryRadius, (*(*this->points)[2]).secondaryOn);

            //_draw_centroid(painter, centroidRadius, centroidTriangleOn);
        }

        if (gamutShape == PrestoPalette::GamutShapeSquare)
        {
            _draw_primary_imp(painter, (*(*this->points)[0]).point, primaryRadius, false);
            _draw_line_imp(painter, (*(*this->points)[0]).point, (*(*this->points)[1]).point, secondaryRadius, (*(*this->points)[0]).secondaryOn);
            _draw_primary_imp(painter, (*(*this->points)[1]).point, primaryRadius, false);
            _draw_line_imp(painter, (*(*this->points)[1]).point, (*(*this->points)[2]).point, secondaryRadius, (*(*this->points)[1]).secondaryOn);
            _draw_primary_imp(painter, (*(*this->points)[2]).point, primaryRadius, false);
            _draw_line_imp(painter, (*(*this->points)[2]).point, (*(*this->points)[3]).point, secondaryRadius, (*(*this->points)[2]).secondaryOn);
            _draw_primary_imp(painter, (*(*this->points)[3]).point, primaryRadius, false);
            //_draw_line_imp(painter, (*(*this->points)[3]).point, (*(*this->points)[0]).point, secondaryRadius, (*(*this->points)[3]).secondaryOn);

            //_draw_centroid(painter, centroidRadius, centroidQuadOn);
        }

        // make sure that this is empty
        //colors.clear();

        // populate the angles for each point
        for (auto n : *points)
        {
            n->angle = PointPairToBearingDegrees(center, n->point);
        };

        //std::sort(points->begin(), points->end(), sort_angles1);

        QVector<QColor> sortedColors;
        if (gamutShape == PrestoPalette::GamutShapeLine)
        {
            sortedColors.append(getColorAt((*points)[0]->point));
            /*if ((*(*this->points)[0]).secondaryOn)
            {
                sortedColors.append(getMidPointColor((*points)[0]->point, (*points)[1]->point));
            }*/
            sortedColors.append(getColorAt((*points)[1]->point));
        }

        if (gamutShape == PrestoPalette::GamutShapeTriangle)
        {
            sortedColors.append(getColorAt((*points)[0]->point));
            /*if ((*points)[0]->secondaryOn)
            {
                sortedColors.append(getMidPointColor((*points)[0]->point, (*points)[1]->point));
            }*/
            sortedColors.append(getColorAt((*points)[1]->point));
            /*if ((*points)[1]->secondaryOn)
            {
                sortedColors.append(getMidPointColor((*points)[1]->point, (*points)[2]->point));
            }*/
            sortedColors.append(getColorAt((*points)[2]->point));
            /*if ((*points)[2]->secondaryOn)
            {
                sortedColors.append(getMidPointColor((*points)[2]->point, (*points)[0]->point));
            }

            if (centroidTriangleOn)
            {
                sortedColors.append(getColorAt(*this->centroid));
            }*/
        }

        if (gamutShape == PrestoPalette::GamutShapeSquare)
        {
            sortedColors.append(getColorAt((*points)[0]->point));
            /*if ((*points)[0]->secondaryOn)
            {
                sortedColors.append(getMidPointColor((*points)[0]->point, (*points)[1]->point));
            }*/
            sortedColors.append(getColorAt((*points)[1]->point));
            /*if ((*points)[1]->secondaryOn)
            {
                sortedColors.append(getMidPointColor((*points)[1]->point, (*points)[2]->point));
            }*/
            sortedColors.append(getColorAt((*points)[2]->point));
            /*if ((*points)[2]->secondaryOn)
            {
                sortedColors.append(getMidPointColor((*points)[2]->point, (*points)[3]->point));
            }*/
            sortedColors.append(getColorAt((*points)[3]->point));
            /*if ((*points)[3]->secondaryOn)
            {
                sortedColors.append(getMidPointColor((*points)[3]->point, (*points)[0]->point));
            }

            if (centroidQuadOn)
            {
                sortedColors.append(getColorAt(*this->centroid));
            }*/
        }

        QVector<QColor> newColorsVec;
        for (auto cc : sortedColors)
        {
            newColorsVec.append(cc);
        }

        if (this->selectedColors != newColorsVec)
        {
            this->selectedColors = newColorsVec;

            // notify that colors changed
            emit selectedColorsChanged();

            emit newColors(newColorsVec);
        }
    }

    return false;
}

bool CirclePalette::_is_collision(const QPoint &circleCenter, int circleRadius, const QPoint &hitTest)
{
    return (qPow(circleCenter.x() - hitTest.x(), 2.0) + qPow(circleCenter.y() - hitTest.y(), 2.0)) <= qPow(circleRadius, 2.0);
}

void CirclePalette::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (this->isDragging == false)
        {
            QPoint mousePos = colorWheel->mapFromGlobal(QCursor::pos());
            //QPoint mousePos = event->pos();
            //qDebug() << mousePos;

            // check for collisions with the points
            // http://math.stackexchange.com/questions/198764/how-to-know-if-a-point-is-inside-a-circle            
            for (auto cp : *this->points)
            {                
                QPoint *p = &cp->point;
                if (_is_collision(*p, primaryRadius, mousePos))
                {
                    this->dragStartPosition = event->pos();
                    this->isDragging = true;
                    this->dragPoint = p;
                    this->relativeDistance = dragStartPosition - *p;
                    //qInfo() << "CLICK: " << event->pos() << " CIRCLE: " << *p;
                    //qDebug() << "Draging enable";
                    return;
                }
            }            
        }

        this->drawnElements->repaint();
    }

    //BaseWidget::mousePressEvent(event);

    /* important, so the event filter catches it */
    //event->ignore();
}

void CirclePalette::mouseReleaseEvent(QMouseEvent *event)
{
    if (isDragging)
    {
        this->isDragging = false;
        this->dragStartPosition = QPoint();
        this->dragPoint = nullptr;
        //qDebug() << "Draging disable";
        this->drawnElements->repaint();
    }

    //BaseWidget::mouseReleaseEvent(event);

    /* important, so the event filter catches it */
    //event->ignore();
}

void CirclePalette::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging)
    {
        *this->dragPoint = (event->pos() - this->relativeDistance);
        this->drawnElements->repaint();
    }

    //BaseWidget::mouseMoveEvent(event);

    /* important, so the event filter catches it */
    //event->ignore();
}

void CirclePalette::create_gamut_line()
{
    /*ColorPoint(QPoint(411 - this->geometry().left() - LEFT_SHIFT, 172 - this->geometry().top() - TOP_SHIFT));*/
    /*ColorPoint(QPoint(120 - this->geometry().left() - LEFT_SHIFT, 407 - this->geometry().top() - TOP_SHIFT));*/
    /*ColorPoint *pFirst = new ColorPoint(QPoint(80,160));
    ColorPoint *pSecond = new ColorPoint(QPoint(250,40));

    pointsLine.push_back(pFirst);
    pointsLine.push_back(pSecond);*/

    for (quint8 index = 0; index < 2; index++) {
        ColorPoint *ptr = new ColorPoint(defaultPoints.at(index));
        pointsLine.push_back(ptr);
    }
}

void CirclePalette::create_gamut_triangle()
{
//    ColorPoint *pBottom = new ColorPoint(QPoint(269 - this->geometry().left() - LEFT_SHIFT, 519 - this->geometry().top() - TOP_SHIFT));
//    ColorPoint *pTopLeft = new ColorPoint(QPoint(423 - this->geometry().left() - LEFT_SHIFT, 253 - this->geometry().top() - TOP_SHIFT));
//    ColorPoint *pTopRight = new ColorPoint(QPoint(295 - this->geometry().left() - LEFT_SHIFT, 184 - this->geometry().top() - TOP_SHIFT));

    /*ColorPoint *pBottom = new ColorPoint(QPoint(80,160));
    ColorPoint *pTopLeft = new ColorPoint(QPoint(250,40));
    ColorPoint *pTopRight = new ColorPoint(QPoint(420,140));

    pointsTriangle.push_back(pBottom);
    pointsTriangle.push_back(pTopLeft);
    pointsTriangle.push_back(pTopRight);*/

    for (quint8 index = 0; index < 3; index++) {
        ColorPoint *ptr = new ColorPoint(defaultPoints.at(index));
        pointsTriangle.push_back(ptr);
    }
}

void CirclePalette::create_gamut_square()
{
//    ColorPoint *pFirst = new ColorPoint(QPoint(204 - this->geometry().left(), 40 - this->geometry().top()));
//    ColorPoint *pSecond = new ColorPoint(QPoint(410 - this->geometry().left(), 194 - this->geometry().top()));
//    ColorPoint *pThird = new ColorPoint(QPoint(380 - this->geometry().left(), 318 - this->geometry().top()));
//    ColorPoint *pFourth = new ColorPoint(QPoint(116 - this->geometry().left(), 355 - this->geometry().top()));
    /*ColorPoint *pFirst = new ColorPoint(QPoint(80,160));
    ColorPoint *pSecond = new ColorPoint(QPoint(250,40));
    ColorPoint *pThird = new ColorPoint(QPoint(420,140));
    ColorPoint *pFourth = new ColorPoint(QPoint(410, 260));

    pointsQuad.push_back(pFirst);
    pointsQuad.push_back(pSecond);
    pointsQuad.push_back(pThird);
    pointsQuad.push_back(pFourth);*/

    for (quint8 index = 0; index < 4; index++) {
        ColorPoint *ptr = new ColorPoint(defaultPoints.at(index));
        pointsQuad.push_back(ptr);
    }
}

void CirclePalette::RecreateGamut(QVector<QPoint> vPoints)
{
    PrestoPalette::GlobalGamutShape shape;
    QVector<ColorPoint *> *editableVector;
    if (vPoints.size() == 2) {
        editableVector = &pointsLine;
        shape = PrestoPalette::GamutShapeLine;
    }
    else if (vPoints.size() == 3) {
        editableVector = &pointsTriangle;
        shape = PrestoPalette::GamutShapeTriangle;
    }
    else if (vPoints.size() == 4) {
        editableVector = &pointsQuad;
        shape = PrestoPalette::GamutShapeSquare;
    }

    for (quint16 indexOfVector = 0; indexOfVector != editableVector->size(); indexOfVector++) {
        delete editableVector->operator [](indexOfVector);
        editableVector->operator [](indexOfVector) = new ColorPoint(QPoint(vPoints.at(indexOfVector)));
    }

    ChangeGamutShape(shape);
}

void CirclePalette::destroy_gamut()
{
    mouseReleaseEvent(NULL);

    gamutShape = PrestoPalette::GamutShapeNone;
    this->points = NULL;
    //this->secondaries = NULL;
    this->centroid = NULL;
}

void CirclePalette::ChangeGamutShape(PrestoPalette::GlobalGamutShape shape)
{
    if (shape != gamutShape)
    {
        destroy_gamut();
    }

    // boundary check
    if (shape >= PrestoPalette::GamutShapeLast)
    {
        gamutShape = PrestoPalette::GamutShapeTriangle;
    }
    else
    {
        gamutShape = shape;
    }

    if (gamutShape == PrestoPalette::GamutShapeLine)
    {
        this->points = &this->pointsLine;
        //this->secondaries = &this->seconardariesLine;
        this->centroid = NULL;
    }

    if (gamutShape == PrestoPalette::GamutShapeTriangle)
    {
        this->points = &this->pointsTriangle;
        //this->secondaries = &this->seconardariesTriangle;
        this->centroid = &this->centroidTriangle;
    }

    if (gamutShape == PrestoPalette::GamutShapeSquare)
    {
        this->points = &this->pointsQuad;
        //this->secondaries = &this->seconardariesQuad;
        this->centroid = &this->centroidQuad;
    }

    this->drawnElements->repaint();
}

void CirclePalette::SaveState(QJsonObject &saveState)
{
    saveState["centroidTriangleOn"] = this->centroidTriangleOn;
    saveState["centroidQuadOn"] = this->centroidQuadOn;

    {
        QJsonArray pointsLineArray;
        foreach (ColorPoint *point, pointsLine)
        {
            QJsonObject s;
            point->SaveState(s);
            pointsLineArray.append(s);
        }
        saveState["pointsLine"] = pointsLineArray;
    }

    {
        QJsonArray pointsTriangleArray;
        foreach (ColorPoint *point, pointsTriangle)
        {
            QJsonObject s;
            point->SaveState(s);
            pointsTriangleArray.append(s);
        }
        saveState["pointsTriangle"] = pointsTriangleArray;
    }

    {
        QJsonArray pointsQuadArray;
        foreach (ColorPoint *point, pointsQuad)
        {
            QJsonObject s;
            point->SaveState(s);
            pointsQuadArray.append(s);
        }
        saveState["pointsQuad"] = pointsQuadArray;
    }
}

void CirclePalette::LoadState(QJsonObject &loadState)
{
    this->centroidTriangleOn = loadState["centroidTriangleOn"].toBool();
    this->centroidQuadOn = loadState["centroidQuadOn"].toBool();

    {
        QJsonArray array = loadState["pointsLine"].toArray();
        for (int i = 0; i < array.size(); i++)
        {
            QJsonObject s = array[i].toObject();
            if (pointsLine.size() >= i)
            {
                pointsLine[i]->LoadState(s);
            }
        }
    }

    {
        QJsonArray array = loadState["pointsTriangle"].toArray();
        for (int i = 0; i < array.size(); i++)
        {
            QJsonObject s = array[i].toObject();
            if (pointsTriangle.size() >= i)
            {
                pointsTriangle[i]->LoadState(s);
            }
        }
    }

    {
        QJsonArray array = loadState["pointsQuad"].toArray();
        for (int i = 0; i < array.size(); i++)
        {
            QJsonObject s = array[i].toObject();
            if (pointsQuad.size() >= i)
            {
                pointsQuad[i]->LoadState(s);
            }
        }
    }
}

QVector<QPoint> CirclePalette::getVectorOfPoint()
{
    QVector<QPoint> vecOfPoint;
    for (quint16 index = 0; index != points->size(); index++) {
        vecOfPoint.push_back(points->at(index)->point);
    }
    return vecOfPoint;
}

void CirclePalette::ChangeWheelShape(PrestoPalette::GlobalWheelShape shape)
{
    // boundary check
    if (shape >= PrestoPalette::WheelShapeLast)
    {
        this->wheelShape = PrestoPalette::WheelShapeCoarse;
    }
    else
    {
        this->wheelShape = shape;
    }

    switch (this->wheelShape)
    {
    case PrestoPalette::GlobalWheelShape::WheelShapeCoarse:
        colorWheel->setPixmap(QPixmap(QString::fromUtf8(":/Resources/Icon/YWheel_Course.png")).scaled(colorWheel->size(), Qt::KeepAspectRatio));
        break;
    case PrestoPalette::GlobalWheelShape::WheelShapeFine:
        colorWheel->setPixmap(QPixmap(QString::fromUtf8(":/Resources/Icon/YWheel_Fine.png")).scaled(colorWheel->size(), Qt::KeepAspectRatio));
        break;
    default:
        // not sure what to do here
        break;
    }
}

ColorSchemeWidget::ColorSchemeWidget(QWidget *parent) :
    QWidget(parent),
    dspSettingFilePath(QApplication::applicationDirPath() +  "/setting.json"), editableScheme(-1)
{    
    //setTitle("Настройка цветовой схемы");
    //setBlockResizing(true);
    QMargins margin(15,5,15,5);
    setGeometry(80,0,300,300);

    circlePalette = new CirclePalette();
    //circlePalette->setGeometry(0,0,300,300);
    circlePalette->ChangeWheelShape(PrestoPalette::WheelShapeFine);
    circlePalette->ChangeGamutShape(PrestoPalette::GamutShapeTriangle);

    QVBoxLayout *circleLayout = new QVBoxLayout();
    circleLayout->addWidget(circlePalette);
    circleLayout->addStretch(1);

    QGroupBox *circlePaletteGroupBox = new QGroupBox("Цветовая палитра");
    circlePaletteGroupBox->setLayout(circleLayout);

    QLabel *colorNumberLabel = new QLabel("Количество точек");
    QLabel *limitsLabel = new QLabel("Предельные значения");
    QLabel *resultGradientLabel = new QLabel("Итоговый градиент");
    QLabel *colorSchemeLabel = new QLabel("Выбор сохраненной схемы");
    QLabel *colorSchemeNameLabel = new QLabel("Название схемы");

    schemeNameLineEdit = new QLineEdit(this);

    progressBar = new QProgressBar();
    progressBar->hide();

    numberOfPointsComboBox = new QComboBox();
    numberOfPointsComboBox->setMaxCount(3);
    numberOfPointsComboBox->addItems(QStringList({"2",
                                    "3",
                                    "4"}));
    numberOfPointsComboBox->setCurrentIndex(1);

    QPushButton *saveButton = new QPushButton("Сохранить");
    rangeSlider = new RangeSlider(Qt::Horizontal, Option::TripleHandles, nullptr);
    rangeSlider->setGeometry(0,0,100,30);
    gradientLabel = new GradientLabel();

    QHBoxLayout *nameSchemeBodyLayout = new QHBoxLayout();
    nameSchemeBodyLayout->setContentsMargins(margin);
    nameSchemeBodyLayout->addWidget(schemeNameLineEdit);

    QVBoxLayout *nameSchemeMainLayout = new QVBoxLayout();
    nameSchemeMainLayout->addWidget(colorSchemeNameLabel);
    nameSchemeMainLayout->addLayout(nameSchemeBodyLayout);

    QHBoxLayout *colorNumberBodyLayout = new QHBoxLayout();
    colorNumberBodyLayout->setContentsMargins(margin);
    colorNumberBodyLayout->addWidget(numberOfPointsComboBox);

    QVBoxLayout *colorNumberMainLayout = new QVBoxLayout();
    colorNumberMainLayout->addWidget(colorNumberLabel);
    colorNumberMainLayout->addLayout(colorNumberBodyLayout);

    QHBoxLayout *limitsBodyLayout = new QHBoxLayout();
    limitsBodyLayout->setContentsMargins(margin);
    limitsBodyLayout->addWidget(rangeSlider);

    QVBoxLayout *limitsMainLayout = new QVBoxLayout();
    limitsMainLayout->addWidget(limitsLabel);
    limitsMainLayout->addLayout(limitsBodyLayout);

    QHBoxLayout *resultGradientBodyLayout = new QHBoxLayout();
    resultGradientBodyLayout->setContentsMargins(margin);
    resultGradientBodyLayout->addWidget(gradientLabel);

    QVBoxLayout *resultGradientMainLayout = new QVBoxLayout();
    resultGradientMainLayout->addWidget(resultGradientLabel);
    resultGradientMainLayout->addLayout(resultGradientBodyLayout);

    QHBoxLayout *saveControlLayout = new QHBoxLayout();
    saveControlLayout->addWidget(progressBar);
    saveControlLayout->addStretch(1);
    saveControlLayout->addWidget(saveButton);

    QVBoxLayout *mainOptionLayout = new QVBoxLayout();
    mainOptionLayout->addLayout(nameSchemeMainLayout);
    mainOptionLayout->addLayout(colorNumberMainLayout);
    mainOptionLayout->addLayout(limitsMainLayout);
    mainOptionLayout->addLayout(resultGradientMainLayout);
    mainOptionLayout->addLayout(saveControlLayout);
    //mainOptionLayout->addWidget(saveButton, 0, Qt::AlignRight);
    mainOptionLayout->addStretch(1);

    mainColorsOptionsGroupBox = new QGroupBox("Параметры");
    mainColorsOptionsGroupBox->setLayout(mainOptionLayout);
    mainColorsOptionsGroupBox->setEnabled(false);

    QGroupBox *presetColorSchemeGroupBox = new QGroupBox("Сохраненные схемы");
    presetComboBox = new QComboBox();/*
    presetComboBox->setMaxCount(3);
    presetComboBox->addItems(QStringList({"RGB 1",
                                          "RGB 2",
                                          "RGB 3"}));*/


    QSize pixmapSize(100,14);
    QPixmap *pixmap = new QPixmap(pixmapSize);
    pixmap->fill(Qt::transparent);

    QPainter painter(pixmap);
    QRect r = pixmap->rect();

    QLinearGradient gradient(r.topLeft(), r.topRight());
    gradient.setColorAt(0, QColor(Qt::green));
    gradient.setColorAt(0.5, QColor(Qt::yellow));
    gradient.setColorAt(1, QColor(Qt::red));

    painter.setBrush(QBrush(gradient));
    painter.fillRect(r, painter.brush());
    painter.end();

    /*QStandardItemModel *model = new QStandardItemModel(3,1);
    for (int i = 0; i != 4; i++) {
        QStandardItem* sItem2 = new QStandardItem;
        //sItem2->setData(QVariant(QColor( Qt::transparent )), Qt::DecorationRole);
        sItem2->setText(QString("RGB %1").arg(i));
        sItem2->setIcon(QIcon(*pixmap));
        model->setItem(i, 0, sItem2);
    }
    presetComboBox->setModel(model);
    presetComboBox->setIconSize(pixmapSize);*/

    QPushButton *addButton = new QPushButton("Добавить");
    QPushButton *delButton = new QPushButton("Удалить");

    QHBoxLayout *controlButtonLayout = new QHBoxLayout();
    controlButtonLayout->addWidget(addButton);
    controlButtonLayout->addWidget(delButton);

    QHBoxLayout *presetBodyLayout = new QHBoxLayout();
    presetBodyLayout->setContentsMargins(margin);
    presetBodyLayout->addWidget(presetComboBox);

    QVBoxLayout *presetMainLayout = new QVBoxLayout();
    //presetMainLayout->addWidget(colorSchemeLabel);
    presetMainLayout->addLayout(presetBodyLayout);
    presetMainLayout->addLayout(controlButtonLayout);

    presetColorSchemeGroupBox->setLayout(presetMainLayout);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(presetColorSchemeGroupBox);
    leftLayout->addWidget(mainColorsOptionsGroupBox);
    leftLayout->addStretch(1);

    QHBoxLayout *bodyLayout = new QHBoxLayout();
    bodyLayout->addLayout(leftLayout);
    bodyLayout->addWidget(circlePaletteGroupBox);
    bodyLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addSpacerItem(new QSpacerItem(1, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
    mainLayout->addLayout(bodyLayout);
    //mainLayout->addWidget(circlePalette);    

    setLayout(mainLayout);

    connect(numberOfPointsComboBox, QOverload<int>::of(&QComboBox::activated), this, &ColorSchemeWidget::setNumberOfPoints);
    connect(circlePalette, &CirclePalette::newColors, gradientLabel, &GradientLabel::setColors);
    connect(rangeSlider, &RangeSlider::rangeChanged, gradientLabel, &GradientLabel::setPersents);
    connect(saveButton, &QPushButton::clicked, this, &ColorSchemeWidget::acceptColors);
    connect(presetComboBox, QOverload<int>::of(&QComboBox::activated), this, &ColorSchemeWidget::currentSchemeChanged);

    connect(addButton, &QPushButton::released, this, &ColorSchemeWidget::insertScheme);
    connect(delButton, &QPushButton::released, this, &ColorSchemeWidget::deleteScheme);
}

ColorSchemeWidget::~ColorSchemeWidget()
{

}

void ColorSchemeWidget::initColorScheme()
{
    initPresetSettings();
}

void ColorSchemeWidget::setNumberOfPoints()
{
    QComboBox *cb = qobject_cast<QComboBox*>(sender());
    quint16 currentValue = cb->currentText().toInt();

    QString currentScheme = presetComboBox->currentText();

    auto itemOfMap = mapOfColorRanges.find(currentScheme);
    if (itemOfMap != mapOfColorRanges.end()) {
        quint32 vecSize = itemOfMap.value().colorsForGradientLabel.size();
        if (currentValue > vecSize) {
            if ((currentValue - vecSize) > 1)
                itemOfMap.value().push_back();
            itemOfMap.value().push_back();
        }
        else if (currentValue < vecSize) {
            if ((vecSize - currentValue) > 1)
                itemOfMap.value().pop_back();
            itemOfMap.value().pop_back();
        }
        else if (currentValue == vecSize) {
            return;
        }

        fillMainColorsOptionsGroupBox(itemOfMap);
    }


    /*if (currentValue == 2) {
        circlePalette->ChangeGamutShape(PrestoPalette::GamutShapeLine);
        rangeSlider->setRangeType(Option::DoubleHandles);
    }
    else if (currentValue == 3) {
        circlePalette->ChangeGamutShape(PrestoPalette::GamutShapeTriangle);
        rangeSlider->setRangeType(Option::TripleHandles);
    }
    else if (currentValue == 4) {
        circlePalette->ChangeGamutShape(PrestoPalette::GamutShapeSquare);
        rangeSlider->setRangeType(Option::QuadrupleHandles);
    }*/
}

void ColorSchemeWidget::acceptColors()
{
    if (presetComboBox->currentIndex() == editableScheme) {
        editableScheme = -1;
    }
    progressBar->setValue(0);
    progressBar->show();
//    if (gradientLabel->vectorOfColors.size() != gradientLabel->vectorOfPersent.size())
//        return;

//    ColorsMap colorMap;
//    for (quint16 index = 0; index != gradientLabel->vectorOfColors.size(); index++) {
//        colorMap << qMakePair(gradientLabel->vectorOfPersent.at(index) / 100., gradientLabel->vectorOfColors.at(index));
//    }

//    emit sendColorMap(colorMap);

    ColorRanges colorRangesValue(circlePalette->getVectorOfPoint(),
                                 gradientLabel->getColors(),
                                 gradientLabel->getPersents());

    QString lastName = presetComboBox->currentText(),
            currentName = schemeNameLineEdit->text();

    QFile settingFile(dspSettingFilePath);

    if (!settingFile.open(QIODevice::ReadWrite)) {
        qDebug() << "ColorSchemeWidget::acceptColors - Can't open dsp setting file";
        return;
    }

    progressBar->setValue(25);

    QByteArray saveData = settingFile.readAll();
    settingFile.close();
    QJsonDocument settingDoc(QJsonDocument::fromJson(saveData));

    QJsonObject json = settingDoc.object();

    QJsonObject saveState;

    saveState["name"] = currentName;
    QJsonArray ranges;
    quint16 vecSize = colorRangesValue.colorsForGradientLabel.size();
    for (quint16 index = 0; index != vecSize; index++) {
        QJsonObject rangeObject;
        rangeObject["range"] = colorRangesValue.rangesForRangeSlider.at(index);
        rangeObject["rgb"] = colorRangesValue.colorsForGradientLabel.at(index).name();
        rangeObject["x"] = colorRangesValue.pointsForCirclePalette.at(index).x();
        rangeObject["y"] = colorRangesValue.pointsForCirclePalette.at(index).y();

        ranges.insert(index, rangeObject);
    }
    saveState["ranges"] = ranges;

    progressBar->setValue(50);

    if (json.contains("colors") && json["colors"].isArray()) {        
        QJsonArray colorArray = json["colors"].toArray();
        auto colorIter = colorArray.begin(), currentIter = colorArray.end(), repeatIter = colorArray.end();
        QSet<QString> setOfName;
        while (colorIter != colorArray.end()) {
            const QJsonObject &rangeObject = (*colorIter).toObject();

            if (rangeObject.contains("name") && rangeObject["name"].isString()) {
                const QString str = rangeObject["name"].toString();
                setOfName.insert(str);
                if (str == lastName) {
                    currentIter = colorIter;
                }
                if (str == currentName) {
                    repeatIter = colorIter;
                }
            }
            colorIter++;
        }
        if (repeatIter == currentIter) {
            repeatIter = colorArray.end();
        }
        if (repeatIter != colorArray.end()) {
            quint16 counter = 0;
            while (setOfName.find(currentName + QString("_%1").arg(counter)) != setOfName.end()) {
                counter++;
            }
            currentName = currentName + QString("_%1").arg(counter);
            saveState["name"] = currentName;
        }

        if (currentIter != colorArray.end()) {
            *currentIter = saveState;
        }
        else {
            colorArray.push_back(saveState);
        }

        QJsonValueRef ref = json.find("colors").value();
        ref = colorArray;
    }
    else {
        QJsonArray colorArray;
        colorArray.push_back(saveState);
        json.insert("colors", colorArray);
    }
    settingDoc.setObject(json);    

    if (currentName != lastName) {
        auto erasingItem = mapOfColorRanges.find(lastName);
        if (erasingItem != mapOfColorRanges.end()) {
            mapOfColorRanges.erase(erasingItem);
        }
        else {
            qDebug() << "ColorSchemeWidget::acceptColors - Can't find old item in map";
        }
    }
    auto insertItem = mapOfColorRanges.insert(currentName, colorRangesValue);
    presetComboBox->setItemText(presetComboBox->currentIndex(), currentName);
    setItemToPresetComboBox(presetComboBox->currentIndex(), insertItem);

    emit schemeChanged(lastName, currentName, insertItem.value().colorsForGradientLabel, insertItem.value().rangesForRangeSlider);

    progressBar->setValue(75);

    if (!settingFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "ColorSchemeWidget::acceptColors - Can't open dsp setting file";
        return;
    }
    settingFile.write(settingDoc.toJson());
    settingFile.flush();
    settingFile.close();

    progressBar->setValue(100);

    QTimer::singleShot(1000, progressBar, &QProgressBar::hide);
}

void ColorSchemeWidget::initPresetSettings()
{
    QFile settingFile(dspSettingFilePath);
    if (settingFile.exists()) {
        if (!settingFile.open(QIODevice::ReadOnly)) {
            qDebug() << "ColorSchemeWidget::initPresetSettings - Can't open dsp setting file";
            return;
        }

        QByteArray saveData = settingFile.readAll();
        settingFile.close();
        QJsonDocument settingDoc(QJsonDocument::fromJson(saveData));

        const QJsonObject &json = settingDoc.object();

        QStringList namesList;
        if (json.contains("colors") && json["colors"].isArray()) {
            const QJsonArray &colorArray = json["colors"].toArray();

            for (quint16 colorIndex = 0; colorIndex != colorArray.size(); colorIndex++) {
                const QJsonObject &rangesObject = colorArray[colorIndex].toObject();

                ColorRanges colorRanges;
                if (rangesObject.contains("name") && rangesObject["name"].isString()) {
                    //colorRanges.nameForComboBox = rangesObject["name"].toString();
                    namesList.push_back(rangesObject["name"].toString());
                }

                if (rangesObject.contains("ranges") && rangesObject["ranges"].isArray()) {
                    const QJsonArray &rangesArray = rangesObject["ranges"].toArray();

                    for (quint16 rangeIndex = 0; rangeIndex != rangesArray.size(); rangeIndex++) {
                        const QJsonObject &dataObject = rangesArray[rangeIndex].toObject();

                        if (dataObject.contains("range") && dataObject["range"].isDouble()) {
                            colorRanges.rangesForRangeSlider.push_back(dataObject["range"].toInt());
                        }

                        if (dataObject.contains("rgb") && dataObject["rgb"].isString()) {
                            colorRanges.colorsForGradientLabel.push_back(QColor(dataObject["rgb"].toString()));
                        }

                        QPoint xy;
                        if (dataObject.contains("x") && dataObject["x"].isDouble()) {
                            xy.setX(dataObject["x"].toInt());
                        }
                        if (dataObject.contains("y") && dataObject["y"].isDouble()) {
                            xy.setY(dataObject["y"].toInt());
                        }
                        colorRanges.pointsForCirclePalette.push_back(xy);
                    }
                }

                mapOfColorRanges.insert(namesList.last(), colorRanges);
                emit schemeChanged(namesList.last(), namesList.last(), colorRanges.colorsForGradientLabel, colorRanges.rangesForRangeSlider);
            }
        }

        presetComboBox->clear();
        if (!namesList.isEmpty()) {
            QString currentStrOfComboBox;
            if (json.contains("currentScheme") && json["currentScheme"].isString()) {
                currentStrOfComboBox = json["currentScheme"].toString();
            }

            auto currentItem = mapOfColorRanges.find(currentStrOfComboBox);
            if (currentItem == mapOfColorRanges.end())
                currentItem = mapOfColorRanges.begin();

            for (auto itemOfMap = mapOfColorRanges.begin(); itemOfMap != mapOfColorRanges.end(); itemOfMap++) {
                setItemToPresetComboBox(presetComboBox->count(), itemOfMap);
            }


//            for (quint16 indexNameList = 0; indexNameList != namesList.size(); indexNameList++) {
//                //const QString &currentName = namesList.at(indexNameList);
//                setItemToPresetComboBox(presetComboBox->count(), currentItem);
//            }

            //mainColorsOptionsGroupBox->setEnabled(true);


            //auto *currentColorRanges = &(mapOfColorRanges.begin().value());
            /*if (currentItem != mapOfColorRanges.end()) {
                 currentColorRanges = &(currentItem.value());
            }
            else {
                currentItem = mapOfColorRanges.begin();
                currentStrOfComboBox = mapOfColorRanges.begin().key();
            }*/

            fillMainColorsOptionsGroupBox(currentItem);
            presetComboBox->setCurrentIndex(presetComboBox->findText(currentItem.key()));

            /*presetComboBox->setCurrentIndex(presetComboBox->findText(currentStrOfComboBox));
            schemeNameLineEdit->setText(currentStrOfComboBox);

            numberOfPointsComboBox->setCurrentIndex(
                        numberOfPointsComboBox->findText(
                            QString::number(currentColorRanges->pointsForCirclePalette.size())));

            gradientLabel->setColors(currentColorRanges->colorsForGradientLabel);
            gradientLabel->setPersents(currentColorRanges->rangesForRangeSlider);

            rangeSlider->setRangeType((Option)(currentColorRanges->rangesForRangeSlider.size() - 1),
                                      currentColorRanges->rangesForRangeSlider);

            circlePalette->RecreateGamut(currentColorRanges->pointsForCirclePalette);*/
        }
    }
    else {
        insertScheme();

        acceptColors();
    }


}

void ColorSchemeWidget::setItemToPresetComboBox(const quint32 &index, const MapOfColorRanges::iterator &itemOfColorMap)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(presetComboBox->model());

    QSize pixmapSize(100,14);
    QPixmap *pixmap = new QPixmap(pixmapSize);
    pixmap->fill(Qt::transparent);

    QPainter painter(pixmap);
    QRect r = pixmap->rect();

    QLinearGradient gradient(r.topLeft(), r.topRight());
    for (quint16 indexOfArray = 0; indexOfArray != itemOfColorMap.value().colorsForGradientLabel.size(); indexOfArray++){
        gradient.setColorAt(itemOfColorMap.value().rangesForRangeSlider.at(indexOfArray) / 100.,
                            itemOfColorMap.value().colorsForGradientLabel.at(indexOfArray));
    }

    painter.setBrush(QBrush(gradient));
    painter.fillRect(r, painter.brush());
    painter.end();

    QStandardItem* sItem;
    if (index >= presetComboBox->count()) {
        sItem = new QStandardItem;
        model->setItem(model->rowCount(), 0, sItem);
        sItem = model->item(model->rowCount() - 1);
    }
    else {
        sItem = model->item(index);
    }
    sItem->setText(itemOfColorMap.key());
    sItem->setIcon(QIcon(*pixmap));

    //presetComboBox->setEditable(true);
    presetComboBox->setIconSize(pixmapSize);
}

void ColorSchemeWidget::fillMainColorsOptionsGroupBox(const MapOfColorRanges::iterator &itemOfColorMap)
{
    if (!mainColorsOptionsGroupBox->isEnabled()) {
        mainColorsOptionsGroupBox->setEnabled(true);
    }

    schemeNameLineEdit->setText(itemOfColorMap.key());

    numberOfPointsComboBox->setCurrentIndex(
                numberOfPointsComboBox->findText(
                    QString::number(itemOfColorMap.value().colorsForGradientLabel.size())));

    gradientLabel->setColors(itemOfColorMap.value().colorsForGradientLabel);
    gradientLabel->setPersents(itemOfColorMap.value().rangesForRangeSlider);

    rangeSlider->setRangeType((Option)(itemOfColorMap.value().rangesForRangeSlider.size() - 1),
                              itemOfColorMap.value().rangesForRangeSlider);

    circlePalette->RecreateGamut(itemOfColorMap.value().pointsForCirclePalette);
}

void ColorSchemeWidget::insertScheme()
{
    if (editableScheme != -1) {
        auto deletedItem = mapOfColorRanges.find(presetComboBox->itemText(editableScheme));
        if (deletedItem != mapOfColorRanges.end()) {
            mapOfColorRanges.erase(deletedItem);
        }
        else {
            qDebug() << "ColorSchemeWidget::insertScheme - Cant find editable item in map";
        }
        presetComboBox->removeItem(editableScheme);

    }

    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(presetComboBox->model());
    quint16 freeNumber = model->rowCount();
    QString defaultName = QString("Схема %1").arg(freeNumber);
    while (presetComboBox->findText(defaultName, Qt::MatchExactly) != -1) {
        defaultName = QString("Схема %1").arg(++freeNumber);
    }

    ColorRanges colorRanges;
    colorRanges.defaultInit(3);
    auto itemOfMap = mapOfColorRanges.insert(defaultName, colorRanges);
    setItemToPresetComboBox(model->rowCount(), itemOfMap);
    fillMainColorsOptionsGroupBox(itemOfMap);

    editableScheme = model->rowCount();

    presetComboBox->setCurrentIndex(presetComboBox->findText(itemOfMap.key()));
}

void ColorSchemeWidget::deleteScheme()
{
    if (presetComboBox->currentIndex() != -1) {
        const QString &currentName = presetComboBox->currentText();
        if (presetComboBox->currentIndex() == editableScheme) {
            presetComboBox->removeItem(presetComboBox->currentIndex());
            schemeNameLineEdit->clear();
            mainColorsOptionsGroupBox->setEnabled(false);
            editableScheme = -1;
            return;
        }

        emit schemeDeleted(currentName);

        QFile settingFile(dspSettingFilePath);

        if (!settingFile.open(QIODevice::ReadWrite)) {
            qDebug() << "ColorSchemeWidget::acceptColors - Can't open dsp setting file";
            return;
        }

        QByteArray saveData = settingFile.readAll();
        settingFile.close();
        QJsonDocument settingDoc(QJsonDocument::fromJson(saveData));

        QJsonObject json = settingDoc.object();

        bool findItem = false;
        if (json.contains("colors") && json["colors"].isArray()) {

            QJsonArray colorArray = json["colors"].toArray();
            auto colorIter = colorArray.begin();

            while (colorIter != colorArray.end()) {
                 const QJsonObject &rangeObject = (*colorIter).toObject();
                if (rangeObject.contains("name") && rangeObject["name"].isString()) {
                    const QString str = rangeObject["name"].toString();

                    if (str == currentName) {
                        findItem = true;
                        colorArray.erase(colorIter);
                        break;
                    }
                }
            }
            if (findItem) {
                QJsonValueRef ref = json.find("colors").value();
                ref = colorArray;
            }
        }
        if (findItem) {
            settingDoc.setObject(json);

            if (!settingFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                qDebug() << "ColorSchemeWidget::acceptColors - Can't open dsp setting file";
                return;
            }
            settingFile.write(settingDoc.toJson());
            settingFile.flush();
            settingFile.close();
        }        
    }
}

void ColorSchemeWidget::currentSchemeChanged()
{
    QComboBox *cb = qobject_cast<QComboBox*>(sender());
    QString str = cb->currentText();

    auto itemOfMap = mapOfColorRanges.find(str);
    if (itemOfMap != mapOfColorRanges.end()) {
        fillMainColorsOptionsGroupBox(itemOfMap);
    }
    else {
        qDebug() << "ColorSchemeWidget::currentSchemeChanged - Can't find name of scheme";
    }
}

GradientLabel::GradientLabel(QWidget *widget) : QLabel(widget)
{
    setGeometry(0,0,100,40);
    vectorOfColors << QColor(Qt::black) << QColor(Qt::black) << QColor(Qt::black);
    vectorOfPersent << 0 << 50 << 100;
}

const QVector<QColor> &GradientLabel::getColors() const
{
    return vectorOfColors;
}

const QVector<int> &GradientLabel::getPersents() const
{
    return vectorOfPersent;
}

void GradientLabel::setColors(QVector<QColor> colors)
{
    vectorOfColors.swap(colors);
    update();
}

void GradientLabel::setPersents(QVector<int> persents)
{
    vectorOfPersent.swap(persents);
    update();
}

void GradientLabel::paintEvent(QPaintEvent *event)
{
    if (vectorOfColors.size() != vectorOfPersent.size())
        return;

    QPainter painter(this);
    QRect widgetRect(0, 0, size().width(), size().height());

    QLinearGradient bgGradient(widgetRect.topLeft(), widgetRect.topRight());
    for (quint16 index = 0; index != vectorOfColors.size(); index++) {
        bgGradient.setColorAt(vectorOfPersent.at(index) / 100., vectorOfColors.at(index));
    }
    painter.fillRect(widgetRect, bgGradient);
}
