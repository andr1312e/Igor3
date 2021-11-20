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
#include "math.h"
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <QSlider>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTimer>
#include <QToolTip>
#include <QtMath>
#include <qnumeric.h>
#include <qwt_color_map.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_draw.h>

#include "enums/colormap.h"

#include "model/spectrogramdata.h"

#include "ui/colorscheme/maincolorschemewidget.h"

#include "ui/dspwidget/spektroPlotter/alphacolormap.h"
#include "ui/dspwidget/spektroPlotter/huecolormap.h"
#include "ui/dspwidget/spektroPlotter/linearcolormapindexed.h"
#include "ui/dspwidget/spektroPlotter/linearcolormaprgb.h"
#include "ui/dspwidget/spektroPlotter/plotterzoomer.h"

class CustomPlotterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomPlotterWidget(qint32 _targetID, bool _displayPlot, QWidget *parent);
    ~CustomPlotterWidget();
private:
    void CreateObjects();
    void CreateUI();
    void InitWindowsTitle();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void CreateBehavoirObjects();
    void InitBehavoirObjects();
    void ConnectObjects();


Q_SIGNALS:
    void ToSetDSPDataOnPlotter(quint32 targetId, quint32 pos);
    void ToSendColorRangesToScene(const qint32 target, const ColorRanges &colorRanges);

public Q_SLOTS:
    void OnStartMovie();
    void OnChangeGradient(const ColorRanges &range);
    void OnShowContour( bool on );
    void OnShowSpectrogram( bool on );
    void OnSetColorsMap(const ColorsMap &colors );
    void OnSetAlpha( int );

    void OnUpdatePlotSlot();
    void OnPannerHandler(int dx,int dy);
    void OnZoomHandler(const QRectF & rect);

    void OnSchemeChange(QString &nameScheme, QString &newNameScheme, QVector<QColor> &colors, QVector<int> &ranges);
    void OnSchemeDelete(QString nameScheme);

private Q_SLOTS:
    void startStopClicked();
    void sliderOffset();
    void timerTimeout();

public Q_SLOTS:
    void OnSetSliderLimit(quint32 counter);

public:
    void UpdateData(const quint32 &n, const quint32 &m, const quint32 &counter, const QVector<qreal> &vector);
    void ForceSendColorRangesToScene();
private:
    void setItemToPresetComboBox(const quint32 &index, const MapOfColorRanges::iterator &itemOfColorMap);
    void createColorArray(const ColorRanges &colorRanges);
    void currentSchemeChanged();

private:
    QwtPlot *m_qwtPlot;
    QwtPlotSpectrogram *d_spectrogram;
    SpectrogramData *m_spectogramData;
    QwtPlotZoomer *m_zoomer;
    QwtPlotPanner *m_panner;

    GradientColorChangerWidget *m_colorSchemeWidget;

    QSlider *sliderControl;
    QPushButton *startStopButton;

    QTimer *updatePlot;

    MapOfColorRanges mapOfColorRanges;

    int d_mapType;
    const quint16 dspRefreshRate;
    const qint32 targetID;
    const bool displayPlot;
};



#endif // CUSTOMPLOTTER_H
