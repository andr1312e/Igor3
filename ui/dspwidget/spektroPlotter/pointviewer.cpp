#include "pointviewer.h"

PointViewer::PointViewer(QWidget *canvas)
    :QwtPlotZoomer( canvas )

{
    setTrackerMode( AlwaysOn );
}

PointViewer::~PointViewer()
{

}

QwtText PointViewer::trackerTextF(const QPointF &pos) const// текст рядом с точкой выделеной
{
    QColor backgroundColor( Qt::white );
    backgroundColor.setAlpha( 200 );

    QwtPlotItem *item =plot()->itemList( QwtPlotItem::Rtti_PlotSpectrogram ).at(0);
    QwtPlotSpectrogram *plotSpectogram = reinterpret_cast<QwtPlotSpectrogram *>(item);
    qreal value = plotSpectogram->data()->value(pos.x(), pos.y());

    QString strCoords = QString(" Дальность: %1\nВремя: %2,Амплитуда: %3   ").arg(qFloor(pos.x())).arg(qFloor(pos.y())).arg(qFloor(value));
    QwtText text(strCoords);
    text.setBackgroundBrush( QBrush(backgroundColor ) );

    return text;
}

