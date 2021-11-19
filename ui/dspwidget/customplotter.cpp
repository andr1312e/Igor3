#include "customplotter.h"
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

quint16 xMaxInterval = 32;
quint16 yMaxInterval = 2512;
const quint16 zMaxInterval = 100;


CustomPlotterWidget::CustomPlotterWidget(qint32 _targetID, bool _displayPlot,  QWidget *parent) : QWidget(parent),
    d_alpha(255), dspRefreshRate(200), targetID(_targetID), displayPlot(_displayPlot)
{
    QString title;
    if (targetID < 0) {
        title = QString("ДСП по ЦУ №%1 ").arg(abs(targetID));
    }
    else {
        title = QString("ДСП по трассе №%1 ").arg(targetID);
    }
    setWindowTitle(title);

    m_qwtPlot = new QwtPlot();

    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount( 1 ); // use system specific thread count
    d_spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );

    m_spectogramData = new SpectrogramData();
    d_spectrogram->setData(m_spectogramData);
    d_spectrogram->attach( m_qwtPlot );

    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );

    // A color bar on the right axis

    QwtScaleWidget *rightAxis = m_qwtPlot->axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( "Амплитуда" );
    rightAxis->setColorBarEnabled( true );

    m_qwtPlot->setAxisScale(QwtPlot::yLeft, 0, yMaxInterval);
    m_qwtPlot->setAxisScale(QwtPlot::xBottom, 0, xMaxInterval);

    m_qwtPlot->setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    m_qwtPlot->enableAxis( QwtPlot::yRight );

    m_qwtPlot->plotLayout()->setAlignCanvasToScales( true );

    OnSetColorMap( ColorMap::RGBMap );
    QwtPlotZoomer* zoomer = new MyZoomer( m_qwtPlot->canvas() );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
                             Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
                             Qt::RightButton );

    connect(zoomer, &QwtPlotZoomer::zoomed, this, &CustomPlotterWidget::OnZoomHandler);

    QwtPlotPanner *panner = new QwtPlotPanner( m_qwtPlot->canvas() );
    panner->setAxisEnabled( QwtPlot::yRight, false );
    panner->setMouseButton( Qt::MiddleButton );

    connect(panner, &QwtPlotPanner::panned, this, &CustomPlotterWidget::OnPannerHandler);

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm( m_qwtPlot->axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = m_qwtPlot->axisScaleDraw( QwtPlot::yLeft );
    sd->setMinimumExtent( fm.width( "5.00" ) );

    const QColor c( Qt::darkBlue );
    zoomer->setRubberBandPen( c );
    zoomer->setTrackerPen( c );

    //    QLabel *colorLabel = new QLabel("Цветовая схема");

    //НЕ ИСПОЛЬЗУТЕСЯ????????
    QComboBox *mapBox = new QComboBox( );
    mapBox->addItem( "RGB" );
    mapBox->addItem( "Hue" );
    mapBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    connect( mapBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CustomPlotterWidget::OnSetColorMap);

    //НЕ ИСПОЛЬЗУТЕСЯ????????
    QLabel *antiAlasingLabel = new QLabel("Сглаживание");
    QComboBox *setAntiAlasingComboBox = new QComboBox( );
    setAntiAlasingComboBox->addItem( "Выкл" );
    setAntiAlasingComboBox->addItem( "Вкл" );
    setAntiAlasingComboBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    connect( setAntiAlasingComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CustomPlotterWidget::OnSetAntiAlasing);

//    m_colorSchemeWidget = new GradientColorChangerWidget(Q_NULLPTR);
    //    connect(m_colorSchemeWidget, &MainColorSchemeWidget::ToChangeScheme, this, &CustomPlotterWidget::OnSchemeChange);
    //    connect(m_colorSchemeWidget, &MainColorSchemeWidget::ToDeleteScheme, this, &CustomPlotterWidget::OnSchemeDelete);

    presetComboBox = new QComboBox();
    //    m_colorSchemeWidget->initColorScheme();
    QPushButton *colorSchemeSettingButton = new QPushButton("Настроить");
//    connect(colorSchemeSettingButton, &QPushButton::clicked, [&](){
//        if (m_colorSchemeWidget->isHidden())
//            m_colorSchemeWidget->show();
//        else
//            m_colorSchemeWidget->hide();
//    });
    QHBoxLayout *settingButtonLayout = new QHBoxLayout();
    settingButtonLayout->addWidget(presetComboBox);
    settingButtonLayout->addWidget(antiAlasingLabel);
    settingButtonLayout->addWidget(setAntiAlasingComboBox);
    settingButtonLayout->addStretch(1);
    settingButtonLayout->addWidget(colorSchemeSettingButton);

    QVBoxLayout *preSetLayout = new QVBoxLayout();
    //preSetLayout->addWidget(presetComboBox);
    preSetLayout->addLayout(settingButtonLayout);
    preSetLayout->addStretch(1);

    QGroupBox *presetColorSchemeGroupBox = new QGroupBox("Цветовая схема");
    //presetColorSchemeGroupBox->setStyleSheet(groupBoxCSS);
    presetColorSchemeGroupBox->setLayout(preSetLayout);

    sliderControl = new QSlider();
    sliderControl->setOrientation(Qt::Horizontal);
    sliderControl->setSingleStep(1);
    sliderControl->setPageStep(1);
    sliderControl->setTickPosition(QSlider::TicksBelow);
    sliderControl->setMaximum(100);

    //startStopButton = new BasePushButton(":/Resources/Icon/llfr_ic_seek_start_on.png", ":/Resources/Icon/llfr_ic_play_on.png");
    //startStopButton->setSelected();
    startStopButton = new QPushButton("Старт");

    QHBoxLayout *plotControlsLayout = new QHBoxLayout();
    plotControlsLayout->addWidget(startStopButton);
    plotControlsLayout->addWidget(sliderControl);

    //QHBoxLayout *circlePaletteLayout = new QHBoxLayout();
    //circlePaletteLayout->addWidget(paletteListViewer);
    //circlePaletteLayout->addWidget(circlePalette);

    /*QVBoxLayout *colorSchemeLayout = new QVBoxLayout();
    colorSchemeLayout->addWidget(presetColorSchemeGroupBox);
    colorSchemeLayout->addLayout(circlePaletteLayout);

    colorSchemeBox->setLayout(colorSchemeLayout);*/

    QVBoxLayout *mainLayout = new  QVBoxLayout();
    mainLayout->addSpacerItem(new QSpacerItem(1, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
    //mainLayout->addLayout(comboBoxLayout);
    mainLayout->addWidget(presetColorSchemeGroupBox);
    //mainLayout->addStretch(1);
    //mainLayout->addWidget(dspGroupBox);
    if (displayPlot) {
        mainLayout->addWidget(m_qwtPlot, 1);
        mainLayout->addLayout(plotControlsLayout);
    }
    //mainLayout->addWidget(table);

    setLayout(mainLayout);

    updatePlot = new QTimer(this);
    connect(updatePlot, &QTimer::timeout, this, &CustomPlotterWidget::timerTimeout);
    //updatePlot->start(dspRefreshRate);

    //connect(startStopButton, &BasePushButton::clicked, this, &CustomPlotterWidget::startStopClicked);
    connect(startStopButton, &QPushButton::clicked, this, &CustomPlotterWidget::startStopClicked);
    connect(sliderControl, &QSlider::sliderReleased, this, &CustomPlotterWidget::sliderOffset);
    connect(presetComboBox, QOverload<int>::of(&QComboBox::activated), this, &CustomPlotterWidget::currentSchemeChanged);
}

CustomPlotterWidget::~CustomPlotterWidget()
{
    delete m_colorSchemeWidget;

}

void CustomPlotterWidget::UpdateData(const quint32 &n,const quint32 &m,const quint32 &counter, const QVector<qreal> &vector)
{
    //if (sliderControl->value() == sliderControl->maximum() || (startStopButton->text() == QString("Стоп"))) {
    //sp_data->updateMatrix(n, m, vector);
    //}

    if (xMaxInterval != n || yMaxInterval != m) {
        xMaxInterval = n;
        yMaxInterval = m;
        m_qwtPlot->setAxisScale(QwtPlot::xBottom, -10.0, xMaxInterval);
        m_qwtPlot->setAxisScale(QwtPlot::yLeft, -10.0, yMaxInterval);
    }

    m_spectogramData->UpdateMatrix(n, m, vector);

    //    sliderControl->setMaximum(counter);
    //    if (sliderControl->value() == sliderControl->maximum())
    //        sliderControl->setValue(counter);

    d_spectrogram->invalidateCache();
    d_spectrogram->itemChanged();

    m_qwtPlot->replot();
}

void CustomPlotterWidget::ForceSendColorRangesToScene()
{
    currentSchemeChanged();
}

void CustomPlotterWidget::OnChangeGradient(const ColorRanges &range)
{

}

void CustomPlotterWidget::OnShowContour( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnShowSpectrogram( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    d_spectrogram->setDefaultContourPen(
                on ? QPen( Qt::black, 0 ) : QPen( Qt::NoPen ) );

    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnSetColorMap( int type )
{
    QwtScaleWidget *axis = m_qwtPlot->axisWidget( QwtPlot::yRight );
    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );

    d_mapType = type;

    int alpha = d_alpha;
    switch( type )
    {
    case ColorMap::HueMap:
    {
        d_spectrogram->setColorMap( new HueColorMap() );
        axis->setColorMap( zInterval, new HueColorMap() );
        break;
    }
    case ColorMap::AlphaMap:
    {
        alpha = 255;
        d_spectrogram->setColorMap( new AlphaColorMap() );
        axis->setColorMap( zInterval, new AlphaColorMap() );
        break;
    }
    case ColorMap::IndexMap:
    {
        d_spectrogram->setColorMap( new LinearColorMapIndexed() );
        axis->setColorMap( zInterval, new LinearColorMapIndexed() );
        break;
    }
    case ColorMap::RGBMap:
    default:
    {
        d_spectrogram->setColorMap( new LinearColorMapRGB() );
        axis->setColorMap( zInterval, new LinearColorMapRGB() );
    }
    }
    d_spectrogram->setAlpha( alpha );

    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnSetColorsMap(const ColorsMap &colors)
{
    QwtScaleWidget *axis = m_qwtPlot->axisWidget( QwtPlot::yRight );
    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );

    d_mapType = ColorMap::RGBMap;

    d_spectrogram->setColorMap( new LinearColorMapRGB(colors) );
    axis->setColorMap( zInterval, new LinearColorMapRGB(colors) );

    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnSetAlpha( int alpha )
{
    // setting an alpha value doesn't make sense in combination
    // with a color map interpolating the alpha value

    d_alpha = alpha;
    if ( d_mapType != ColorMap::AlphaMap )
    {
        d_spectrogram->setAlpha( alpha );
        m_qwtPlot->replot();
    }
}

void CustomPlotterWidget::OnSetAntiAlasing(int on)
{
    if (on) {
        m_spectogramData->setResampleMode(QwtMatrixRasterData::BilinearInterpolation);
    }
    else {
        m_spectogramData->setResampleMode(QwtMatrixRasterData::NearestNeighbour);
    }
}

void CustomPlotterWidget::OnUpdatePlotSlot()
{
    m_spectogramData->RefreshMatrix();
    d_spectrogram->invalidateCache();
    d_spectrogram->itemChanged();

    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnPannerHandler(int dx, int dy)
{
    QwtPlotPanner *panner = qobject_cast<QwtPlotPanner *>(sender());
    QwtPlot *plot_ptr = panner->plot();

    QwtInterval intervalY = plot_ptr->axisInterval(QwtPlot::yLeft);
    QwtInterval intervalX = plot_ptr->axisInterval(QwtPlot::xBottom);

    //qDebug() << intervalX << intervalY;

    QwtInterval correctIntervalY;
    if (intervalY.minValue() < 0) {
        correctIntervalY.setMinValue(0);
    }
    else {
        correctIntervalY.setMinValue(intervalY.minValue());
    }
    if (intervalY.maxValue() > yMaxInterval) {
        correctIntervalY.setMaxValue(yMaxInterval);
    }
    else {
        correctIntervalY.setMaxValue(intervalY.maxValue());
    }

    QwtInterval correctIntervalX;
    if (intervalX.minValue() < 0) {
        correctIntervalX.setMinValue(0);
    }
    else {
        correctIntervalX.setMinValue(intervalX.minValue());
    }
    if (intervalX.maxValue() > xMaxInterval) {
        correctIntervalX.setMaxValue(xMaxInterval);
    }
    else {
        correctIntervalX.setMaxValue(intervalX.maxValue());
    }


    plot_ptr->setAxisScale(QwtPlot::yLeft, correctIntervalY.minValue(), correctIntervalY.maxValue());
    plot_ptr->setAxisScale(QwtPlot::xBottom, correctIntervalX.minValue(), correctIntervalX.maxValue());

    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnZoomHandler(const QRectF &rect)
{
    QwtPlotZoomer *zoomer = qobject_cast<QwtPlotZoomer *>(sender());
    //QwtPlot *plot_ptr = panner->plot();

    QRectF correctRect(rect);
    if (rect.x() < 0) {
        correctRect.setX(0);
    }
    else if (rect.x() > (xMaxInterval - 10)) {
        correctRect.setX(xMaxInterval - 10);
        if (rect.width() > 10)
            correctRect.setWidth(10);
    }
    if (rect.y() < 0) {
        correctRect.setY(0);
    }
    else if (rect.y() > (yMaxInterval - 10)) {
        correctRect.setY(yMaxInterval - 10);
        if (rect.height() > 10)
            correctRect.setHeight(10);
    }

    if (rect.right() > xMaxInterval) {
        correctRect.setWidth(xMaxInterval - rect.left());
    }

    if (rect.bottom() > yMaxInterval) {
        correctRect.setHeight(yMaxInterval - rect.top());
    }

    if (correctRect != rect) {
        zoomer->zoom(-1);
        zoomer->zoom(correctRect);

        m_qwtPlot->replot();
    }

    //qDebug() << rect;
    //qDebug() << zoomer->zoomBase();
}

void CustomPlotterWidget::setItemToPresetComboBox(const quint32 &index, const MapOfColorRanges::iterator &itemOfColorMap)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(presetComboBox->model());

    QSize pixmapSize(100,14);
    QPixmap pixmap(pixmapSize);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QRect r = pixmap.rect();

    QLinearGradient gradient(r.topLeft(), r.topRight());
    for (quint16 indexOfArray = 0; indexOfArray != itemOfColorMap.value().colorsForGradientLabel.size(); indexOfArray++){
        gradient.setColorAt(itemOfColorMap.value().rangesForRangeSlider.at(indexOfArray) / 100.,
                            itemOfColorMap.value().colorsForGradientLabel.at(indexOfArray));
    }

    painter.setBrush(QBrush(gradient));
    painter.fillRect(r, painter.brush());
    painter.end();

    QStandardItem* sItem;
    if (index >= (quint32)presetComboBox->count()) {
        sItem = new QStandardItem;
        model->setItem(model->rowCount(), 0, sItem);
        sItem = model->item(model->rowCount() - 1);
    }
    else {
        sItem = model->item(index);
    }
    sItem->setText(itemOfColorMap.key());
    sItem->setIcon(QIcon(pixmap));

    presetComboBox->setIconSize(pixmapSize);
}

void CustomPlotterWidget::createColorArray(const ColorRanges &colorRanges)
{
    ColorsMap colorMap;
    for (quint8 index = 0; index !=  colorRanges.colorsForGradientLabel.size(); index++) {
        colorMap.push_back(qMakePair(colorRanges.rangesForRangeSlider.at(index),
                                     colorRanges.colorsForGradientLabel.at(index)));
    }
    OnSetColorsMap(colorMap);
    if (!displayPlot)
        emit ToSendColorRangesToScene(targetID, colorRanges);
}

void CustomPlotterWidget::startStopClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button->text() == QString("Старт")) {
        if (sliderControl->value() == sliderControl->maximum()) {
            updatePlot->stop();
        }
        else {
            updatePlot->start(dspRefreshRate);
        }
        button->setText(QString("Стоп"));
    }
    else {
        updatePlot->stop();
        button->setText(QString("Старт"));
    }
}

void CustomPlotterWidget::sliderOffset()
{
    QSlider *slider = qobject_cast<QSlider *>(sender());
    Q_EMIT ToSetDSPDataOnPlotter(targetID, slider->value());
}

void CustomPlotterWidget::timerTimeout()
{
    if (sliderControl->value() != sliderControl->maximum())
    {
        sliderControl->setValue(sliderControl->value() + 1);
    }
    else
    {
        sliderControl->setValue(0);
    }
}

void CustomPlotterWidget::OnSchemeChange(QString &nameScheme, QString &newNameScheme, QVector<QColor> &colors, QVector<int> &ranges)
{
    auto itemOfMap = mapOfColorRanges.find(nameScheme);
    if (itemOfMap != mapOfColorRanges.end()) {
        if (nameScheme != newNameScheme) {
            mapOfColorRanges.erase(itemOfMap);
            itemOfMap = mapOfColorRanges.insert(newNameScheme, ColorRanges());
        }
    }
    else {
        itemOfMap = mapOfColorRanges.insert(newNameScheme, ColorRanges());
    }
    itemOfMap.value().colorsForGradientLabel = colors;
    itemOfMap.value().rangesForRangeSlider = ranges;


    int searchingIndex = presetComboBox->findText(nameScheme),
            currentIndex = presetComboBox->currentIndex();
    if (searchingIndex == -1) {
        setItemToPresetComboBox(presetComboBox->count(), itemOfMap);
        if (currentIndex == -1) {
            presetComboBox->setCurrentIndex(0);
            currentSchemeChanged();
        }
    }
    else {
        setItemToPresetComboBox(searchingIndex, itemOfMap);
        if (searchingIndex == currentIndex) {
            createColorArray(itemOfMap.value());
        }
    }
}

void CustomPlotterWidget::OnSchemeDelete(QString nameScheme)
{
    auto itemOfMap = mapOfColorRanges.find(nameScheme);
    if (itemOfMap != mapOfColorRanges.end()) {
        mapOfColorRanges.erase(itemOfMap);
    }

    int searchingIndex = presetComboBox->findText(nameScheme);
    presetComboBox->removeItem(searchingIndex);

    QString currentStr = presetComboBox->currentText();
    itemOfMap = mapOfColorRanges.find(currentStr);
    if (itemOfMap != mapOfColorRanges.end()) {
        createColorArray(itemOfMap.value());
    }
}

void CustomPlotterWidget::OnSetSliderLimit(quint32 counter)
{
    sliderControl->setMaximum(counter);
}


void CustomPlotterWidget::currentSchemeChanged()
{
    //QComboBox *cb = qobject_cast<QComboBox*>(sender());
    QString str = presetComboBox->currentText();

    auto itemOfMap = mapOfColorRanges.find(str);
    if (itemOfMap != mapOfColorRanges.end()) {
        createColorArray(itemOfMap.value());
    }
    else {
        qDebug() << "CustomPlotterWidget::currentSchemeChanged - Can't find name of scheme";
    }
}
