#ifndef CUSTOMPLOTTER_H
#define CUSTOMPLOTTER_H

#include <QComboBox>
#include <QWidget>

#include <QStyledItemDelegate>
#include <QListView>
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_scale_widget.h>
#include <QSet>
#include <fftw3.h>
#include "ui/dspwidget/spektroPlotter/linearcolormaprgb.h"
#include "structs/colorscheme/colorranges.h"
#include "presenter/colorscheme/colorschemepresenter.h"
#include "ui/dspwidget/spektroPlotter/pointviewer.h"
#include "model/spectrogramdata.h"
#include "ui/dspwidget/spektroPlotter/huecolormap.h"
#include "ui/dspwidget/spektroPlotter/alphacolormap.h"
#include "ui/dspwidget/spektroPlotter/linearcolormapindexed.h"
#include <qnumeric.h>
#include <qwt_color_map.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_magnifier.h>
#include "enums/colormap.h"
#include <QTimer>
#include <QTableView>
#include <QLayout>
#include <QTimer>
#include "math.h"
#include <QComboBox>
#include <QGroupBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QSpinBox>
#include <QSlider>
#include <QMouseEvent>
#include <QMenu>
#include <QCheckBox>
#include <QToolTip>
#include <QSlider>
#include <QtMath>
#include <QPushButton>
#include <QLabel>
#include <QPushButton>
#include "ui/dspwidget/bottomdspcontrolpanel.h"

class CustomPlotterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomPlotterWidget(QWidget *parent);
    ~CustomPlotterWidget();
private:
    void CreateObjects();
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void CreateBehavoirObjects();
    void InitBehavoirObjects();
    void ConnectObjects();
Q_SIGNALS:
    void ToStartTimer();
    void ToRequestDSPData(int frame);
    void ToSetSliderLimit(int limit);

public Q_SLOTS:
    void OnChangeGradient(const ColorRanges &range);

private Q_SLOTS:
    void OnPannerHandler(int dx,int dy);
    void OnZoomHandler(const QRectF & rect);
public:
    void UpdateData(const quint32 &distSamplesNum, const quint32 &TimeSamplesNum, const QVector<qreal> &data);
    void SetPageNumForGif(int value);
private:
    const QColor GetMaxContrastColor(const QVector<QColor> &currentSpectorColors);
    int CalculateColorDistance(const QColor &color, const QVector<QColor> &spectorColors);
    bool CompareRects(const QRectF &firstRect, const QRectF &secondRect);
private:
    QwtPlot *m_qwtPlot;
    QwtPlotSpectrogram *d_spectrogram;
    SpectrogramData *m_spectogramData;
    QwtPlotPanner *m_chartFixer;
    QwtPlotZoomer *m_pointViewer;


    BottomDspControlPanel *m_bottomDspControlPanel;
    const QVarLengthArray<QColor, 9> m_contrastColors;
};

#endif // CUSTOMPLOTTER_H
