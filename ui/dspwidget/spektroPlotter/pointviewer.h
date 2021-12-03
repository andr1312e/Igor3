#ifndef POINTVIEWVER_H
#define POINTVIEWVER_H
#include <QtMath>
#include <QWidget>

#include <qwt_plot_zoomer.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot.h>


class PointViewer: public QwtPlotZoomer
{
public:
    explicit PointViewer( QWidget *canvas );
    ~PointViewer();

    virtual QwtText trackerTextF( const QPointF &pos ) const override;
};
#endif // POINTVIEWVER_H
