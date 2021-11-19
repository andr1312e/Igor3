#ifndef PLOTTERZOOMER_H
#define PLOTTERZOOMER_H
#include <QtMath>
#include <QWidget>

#include <qwt_plot_zoomer.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot.h>


class MyZoomer: public QwtPlotZoomer
{
public:
    explicit MyZoomer( QWidget *canvas );
    ~MyZoomer();

    virtual QwtText trackerTextF( const QPointF &pos ) const;
};
#endif // PLOTTERZOOMER_H
