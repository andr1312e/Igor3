#ifndef CUSTOMPLOTTER_H
#define CUSTOMPLOTTER_H

#include <QWidget>

#include <QStyledItemDelegate>
#include <QListView>
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_scale_widget.h>
#include <QSet>
#include <fftw3.h>
#include <QComboBox>
#include <QPushButton>
#include <QTableView>


#include "enums/colormap.h"

#include "model/spectrogramdata.h"

#include "ui/colorscheme/maincolorschemewidget.h"
#include "circlepalette.h"


class CustomPlotterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomPlotterWidget(qint32 _targetID, bool _displayPlot, QWidget *parent);
    ~CustomPlotterWidget();

    void UpdateData(const quint32 &n, const quint32 &m, const quint32 &counter, const QVector<qreal> &vector);
    void ForceSendColorRangesToScene();

Q_SIGNALS:
    void ToSetDSPDataOnPlotter(const qint32 target, quint32 pos);
    void ToSendColorRangesToScene(const qint32 target, ColorRanges colorRanges);

public Q_SLOTS:
    void OnShowContour( bool on );
    void OnShowSpectrogram( bool on );
    void OnSetColorMap( int );
    void OnSetColorsMap(const ColorsMap &colors );
    void OnSetAlpha( int );
    void OnSetAntiAlasing(int on);

    void OnUpdatePlotSlot();
    void OnPannerHandler(int dx,int dy);
    void OnZoomHandler(const QRectF & rect);

    void OnSchemeChange(QString &nameScheme, QString &newNameScheme, QVector<QColor> &colors, QVector<int> &ranges);
    void OnSchemeDelete(QString nameScheme);

private Q_SLOTS:
    void startStopClicked();
    void sliderOffset();
    void timerTimeout();

public:
    void SetSliderLimit(quint32 counter);

private:
    void setItemToPresetComboBox(const quint32 &index, const MapOfColorRanges::iterator &itemOfColorMap);
    void createColorArray(const ColorRanges &colorRanges);



    void currentSchemeChanged();

private:
    QwtPlot *m_qwtPlot;
    QwtPlotSpectrogram *d_spectrogram;
    SpectrogramData *m_spectogramData;

    MainColorSchemeWidget *m_colorSchemeWidget;

    QSlider *sliderControl;
    QPushButton *startStopButton;
    QComboBox *presetComboBox;

    QTimer *updatePlot;

    MapOfColorRanges mapOfColorRanges;

    int d_mapType;
    int d_alpha;    
    const quint16 dspRefreshRate;
    const qint32 targetID;
    const bool displayPlot;
};



#endif // CUSTOMPLOTTER_H
