#include "plotterzoomer.h"

MyZoomer::MyZoomer(QWidget *canvas)
    :QwtPlotZoomer( canvas )

{
    setTrackerMode( AlwaysOn );
}

MyZoomer::~MyZoomer()
{

}

QwtText MyZoomer::trackerTextF(const QPointF &pos) const
{
    QColor bg( Qt::white );
    bg.setAlpha( 200 );

    QwtPlotItem *item =plot()->itemList( QwtPlotItem::Rtti_PlotSpectrogram ).at(0);
    QwtPlotSpectrogram *plotSpectogram = reinterpret_cast<QwtPlotSpectrogram *>(item);
    qreal value = plotSpectogram->data()->value(pos.x(), pos.y());

    QString strCoords = QString("%1, %2, %3").arg(qFloor(pos.x())).arg(qFloor(pos.y())).arg(qFloor(value));
    QwtText text(strCoords);
    text.setBackgroundBrush( QBrush( bg ) );

    return text;
}

