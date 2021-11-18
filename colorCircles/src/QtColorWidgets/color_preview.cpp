/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2013-2020 Mattia Basaglia
 * \copyright Copyright (C) 2014 Calle Laakkonen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "QtColorWidgets/color_preview.hpp"

#include <QStylePainter>
#include <QStyleOptionFrame>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QDebug>

namespace color_widgets {

class ColorPreview::Private
{
public:
    QColor col; ///< color to be viewed
    QColor comparison; ///< comparison color
    QBrush back;///< Background brush, visible on a transparent color
    DisplayMode display_mode; ///< How the color(s) are to be shown
    bool draw_frame = true; ///< Whether to draw a frame around the color

    Private() : col(Qt::red), back(Qt::darkGray, Qt::DiagCrossPattern), display_mode(NoAlpha)
    {}
};

ColorPreview::ColorPreview( QWidget *parent, int id) :
    QWidget(parent)
  , p(new Private)
  , m_checked(false)
  , m_id(id)
{
    p->back.setTexture(QPixmap(QStringLiteral(":/color_widgets/alphaback.png")));
}

ColorPreview::~ColorPreview()
{
    delete p;
}

void ColorPreview::setBackground(const QBrush &bk)
{
    p->back = bk;
    update();
    Q_EMIT backgroundChanged(bk);
}

QBrush ColorPreview::background() const
{
    return p->back;
}

ColorPreview::DisplayMode ColorPreview::displayMode() const
{
    return p->display_mode;
}

void ColorPreview::setDisplayMode(DisplayMode m)
{
    p->display_mode = m;
    update();
    Q_EMIT displayModeChanged(m);
}

QColor ColorPreview::color() const
{
    return p->col;
}

QColor ColorPreview::comparisonColor() const
{
    return p->comparison;
}

QSize ColorPreview::sizeHint() const
{
    int width = style()->pixelMetric(QStyle::PM_IndicatorWidth, nullptr, nullptr);
    int height = style()->pixelMetric(QStyle::PM_IndicatorHeight, nullptr, nullptr);
    return QSize(qMax(24, width), qMax(24, height));
}

void ColorPreview::paint(QPainter &painter, QRect rect) const
{
    QColor c1(p->col.rgb());



    //    if ( c1.alpha() < 255 )
    //        painter.fillRect(0, 0, rect.width(), rect.height(), p->back);

    painter.fillRect(0, 0, rect.width(), rect.height(), c1);
    if (m_checked)
    {
        QPen pen(Qt::blue, 3);
        painter.setPen(pen);
        painter.drawRect(0, 0, rect.width(), rect.height());
    }
}

void ColorPreview::setColor(const QColor &c)
{
    p->col = c;
    update();
    Q_EMIT colorChanged(c);
}

void ColorPreview::setComparisonColor(const QColor &c)
{
    p->comparison = c;
    update();
    Q_EMIT comparisonColorChanged(c);
}

void ColorPreview::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);

    paint(painter, geometry());
}

void ColorPreview::resizeEvent(QResizeEvent *)
{
    update();
}

void ColorPreview::mouseReleaseEvent(QMouseEvent * ev)
{
    Q_EMIT ToColorButtonClicked(m_id);
    update();
}

void ColorPreview::mouseMoveEvent(QMouseEvent *ev)
{

    if ( ev->buttons() &Qt::LeftButton && !QRect(QPoint(0,0),size()).contains(ev->pos()) )
    {
        QMimeData *data = new QMimeData;

        data->setColorData(p->col);

        QDrag* drag = new QDrag(this);
        drag->setMimeData(data);

        QPixmap preview(24,24);
        preview.fill(p->col);
        drag->setPixmap(preview);

        drag->exec();
    }
}
void ColorPreview::SetChecked(bool checked)
{
    m_checked=checked;
    update();
}
} // namespace color_widgets
