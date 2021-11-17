#include <qnumeric.h>
#include <qwt_color_map.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_magnifier.h>
#include "customplotter.h"
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

quint16 xMaxInterval = 32;
quint16 yMaxInterval = 2512;
const quint16 zMaxInterval = 100;

class MyZoomer: public QwtPlotZoomer
{
public:
    MyZoomer( QWidget *canvas ):
        QwtPlotZoomer( canvas )
    {
        setTrackerMode( AlwaysOn );
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QColor bg( Qt::white );
        bg.setAlpha( 200 );

        //QwtText text = QwtPlotZoomer::trackerTextF( pos );
        //text.setBackgroundBrush( QBrush( bg ) );

        QwtPlotSpectrogram *item = reinterpret_cast<QwtPlotSpectrogram *>(plot()->itemList( QwtPlotItem::Rtti_PlotSpectrogram ).at(0));
        qreal value = item->data()->value(pos.x(), pos.y());

        QString strCoords = QString("%1, %2, %3").arg(qFloor(pos.x())).arg(qFloor(pos.y())).arg(qFloor(value));
        QwtText text(strCoords);
        text.setBackgroundBrush( QBrush( bg ) );

        return text;
    }
};



SpectrogramData::SpectrogramData()
{
    setInterval( Qt::XAxis, QwtInterval( 0, xMaxInterval ) );
    setInterval( Qt::YAxis, QwtInterval( 0, yMaxInterval ) );
    setInterval( Qt::ZAxis, QwtInterval( 0.0, zMaxInterval ) );

    setResampleMode(QwtMatrixRasterData::BilinearInterpolation);

    refreshMatrix();
}

SpectrogramData::SpectrogramData(quint16 xMax, quint16 yMax) {
    setInterval( Qt::XAxis, QwtInterval( -10.0, xMax ) );
    setInterval( Qt::YAxis, QwtInterval( -10.0, yMax ) );
    setInterval( Qt::ZAxis, QwtInterval( 0.0, zMaxInterval ) );

    setResampleMode(QwtMatrixRasterData::BilinearInterpolation);

    setBackground();
}

void SpectrogramData::refreshMatrix()
{
    static int x = 10;
    int y = x / 2;
    QVector<double> zVector;
    const quint32 col = xMaxInterval, row = yMaxInterval;
    const quint32 colAdd = 0, rowAdd = 0;
    const quint32 totalCol = col + colAdd,
            totalRow = row + rowAdd;

    quint32 step = 3;
    quint32 xLeftLimit = x - step, xRightLimit = x + step,
            yLeftLimit = y - step, yRightLimit = y + step;
    if (yRightLimit > totalRow) {
        x = 10;
        yRightLimit = row;
    }
    const quint32 sizeOfVector = totalCol * totalRow;
    for (quint32 currentRow = 0; currentRow < totalRow; currentRow++) {
        for (quint32 currentCol = 0; currentCol < totalCol; currentCol++) {
            if (xLeftLimit <= currentCol && currentCol <= xRightLimit) {
                if (yLeftLimit <= currentRow && currentRow <= yRightLimit) {
                    zVector.push_back(100 - (qrand() % 4) * 10);
                }
                else
                    zVector.push_back(10);
            }
            else
                zVector.push_back(10);
        }
    }
    x++;
//    for (quint32 index = 0; index < sizeOfVector; index++ ) {
//        zVector.push_back(qrand() % 100);
//    }

    setValueMatrix(zVector, totalCol);
}

void SpectrogramData::updateMatrix(const quint32 &n, const quint32 &m, const QVector<qreal> &vector)
{
    setInterval( Qt::XAxis, QwtInterval( -10.0, xMaxInterval ) );
    setInterval( Qt::YAxis, QwtInterval( -10.0, yMaxInterval ) );
    setValueMatrix(vector, n);
}

void SpectrogramData::setBackground()
{
    const quint32 vecSize = 110;
    QVector<double> zVector(vecSize * vecSize, 0);

    setValueMatrix(zVector, vecSize);
}


class LinearColorMapRGB: public QwtLinearColorMap
{
public:
    LinearColorMapRGB():
        QwtLinearColorMap( Qt::black, Qt::red, QwtColorMap::RGB )

    {
        //addColorStop( 0.1, Qt::cyan );
        //addColorStop( 0.6, Qt::green );
        //addColorStop( 0.95, Qt::yellow );

        //addColorStop( 0.1, Qt::red );
        //addColorStop( 0.6, Qt::yellow );
        //addColorStop( 0.0, Qt::blue );
    }

    LinearColorMapRGB(ColorsMap colorMap) :
    QwtLinearColorMap( colorMap.first().second, colorMap.last().second, QwtColorMap::RGB )
    {
        for (auto &item : colorMap) {
            addColorStop(item.first, item.second);
        }
        setMode(Mode::ScaledColors);
    }
};

class LinearColorMapIndexed: public QwtLinearColorMap
{
public:
    LinearColorMapIndexed():
        QwtLinearColorMap( Qt::darkCyan, Qt::red, QwtColorMap::Indexed )
    {
        //addColorStop( 0.1, Qt::cyan );
        //addColorStop( 0.6, Qt::green );
        //addColorStop( 0.95, Qt::yellow );

        addColorStop( 0.1, Qt::red );
        addColorStop( 0.5, Qt::yellow );
        addColorStop( 0.0, Qt::darkBlue );
    }
};



class HueColorMap: public QwtColorMap
{
public:
    // class backported from Qwt 6.2

    HueColorMap():
        d_hue1(0),
        d_hue2(359),
        d_saturation(150),
        d_value(200)
    {
        updateTable();

    }

    virtual QRgb rgb( const QwtInterval &interval, double value ) const
    {
        if ( qIsNaN(value) )
            return 0u;

        const double width = interval.width();
        if ( width <= 0 )
            return 0u;

        if ( value <= interval.minValue() )
            return d_rgbMin;

        if ( value >= interval.maxValue() )
            return d_rgbMax;

        const double ratio = ( value - interval.minValue() ) / width;
        int hue = d_hue1 + qRound( ratio * ( d_hue2 - d_hue1 ) );

        if ( hue >= 360 )
        {
            hue -= 360;

            if ( hue >= 360 )
                hue = hue % 360;
        }

        return d_rgbTable[hue];
    }

    virtual unsigned char colorIndex( const QwtInterval &, double ) const
    {
        // we don't support indexed colors
        return 0;
    }


private:
    void updateTable()
    {
        for ( int i = 0; i < 360; i++ )
            d_rgbTable[i] = QColor::fromHsv( i, d_saturation, d_value ).rgb();

        d_rgbMin = d_rgbTable[ d_hue1 % 360 ];
        d_rgbMax = d_rgbTable[ d_hue2 % 360 ];
    }

    int d_hue1, d_hue2, d_saturation, d_value;
    QRgb d_rgbMin, d_rgbMax, d_rgbTable[360];
};

class AlphaColorMap: public QwtAlphaColorMap
{
public:
    AlphaColorMap()
    {
        //setColor( QColor("DarkSalmon") );
        setColor( QColor("SteelBlue") );
    }
};

/*class MyPlotMagnifier : public QwtPlotMagnifier {
    Q_OBJECT
public:
    MyPlotMagnifier(QWidget *wgt) : QwtPlotMagnifier(wgt) {
        setAxisEnabled(Qt::XAxis, true);
        setAxisEnabled(Qt::YAxis,true);
        setAxisEnabled(Qt::ZAxis,false);
    }

signals:
    void zoomed();

protected:
    virtual void rescale(double factor) {
        QwtPlotMagnifier::rescale(factor);

        emit zoomed();
    }
};*/

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
    //setTitle(title);

    plot = new QwtPlot();
    //plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //plot->setStyleSheet("QWidget {background-color: black;}");

    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount( 1 ); // use system specific thread count
    d_spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );

    /*QList<double> contourLevels;
    for ( double level = 20; level < 100.0; level += 20.0 )
        contourLevels += level;
    d_spectrogram->setContourLevels( contourLevels );
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, true );*/

    sp_data = new SpectrogramData();
    d_spectrogram->setData(sp_data);
    d_spectrogram->attach( plot );

    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );

    // A color bar on the right axis

    QwtScaleWidget *rightAxis = plot->axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( "Амплитуда" );
    rightAxis->setColorBarEnabled( true );

    plot->setAxisScale(QwtPlot::yLeft, 0, yMaxInterval);
    plot->setAxisScale(QwtPlot::xBottom, 0, xMaxInterval);

    plot->setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    plot->enableAxis( QwtPlot::yRight );

    plot->plotLayout()->setAlignCanvasToScales( true );

    setColorMap( ColorMap::RGBMap );

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size

    QwtPlotZoomer* zoomer = new MyZoomer( plot->canvas() );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    connect(zoomer, SIGNAL(zoomed(QRectF)), this, SLOT(zoomHandler(QRectF)));

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setAxisEnabled( QwtPlot::yRight, false );
    panner->setMouseButton( Qt::MiddleButton );

    connect(panner, SIGNAL(panned(int,int)), this, SLOT(pannerHandler(int,int)));    

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm( plot->axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = plot->axisScaleDraw( QwtPlot::yLeft );
    sd->setMinimumExtent( fm.width( "5.00" ) );

    const QColor c( Qt::darkBlue );
    zoomer->setRubberBandPen( c );
    zoomer->setTrackerPen( c );

    QLabel *colorLabel = new QLabel("Цветовая схема");
    QLabel *aaLabel = new QLabel("Anti-aliasing");
    QComboBox *mapBox = new QComboBox( );
    mapBox->addItem( "RGB" );
    mapBox->addItem( "Hue" );
    mapBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    connect( mapBox, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( setColorMap( int ) ) );

    QComboBox *aaBox = new QComboBox( );
    aaBox->addItem( "Off" );
    aaBox->addItem( "On" );
    aaBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    connect( aaBox, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( setAntiAlasing( int ) ) );

    //QGroupBox *colorSchemeBox = new QGroupBox("Цветовая схема");
    //QTabWidget *colorSchemeTabWidget = new QTabWidget();

    //circlePalette = new CirclePalette();
    //circlePalette->setGeometry(QRect(44, 37, 549, 549));
    //circlePalette->ChangeWheelShape(PrestoPalette::WheelShapeFine);
    //circlePalette->ChangeGamutShape(PrestoPalette::GamutShapeTriangle);
    //circlePalette->show();

    colorSchemeWidget = new ColorSchemeWidget();    
    //colorSchemeWidget->show();

    //connect(colorSchemeWidget, &ColorSchemeWidget::sendColorMap, this, &CustomPlotterWidget::setColorsMap);
    connect(colorSchemeWidget, &ColorSchemeWidget::schemeChanged, this, &CustomPlotterWidget::schemeChangedSlot);
    connect(colorSchemeWidget, &ColorSchemeWidget::schemeDeleted, this, &CustomPlotterWidget::schemeDeletedSlot);

    presetComboBox = new QComboBox();
    colorSchemeWidget->initColorScheme();


    /*QSize pixmapSize(100,14);
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

    QStandardItemModel *model = new QStandardItemModel(3,1);
    for (int i = 0; i != 4; i++) {
        QStandardItem* sItem2 = new QStandardItem;
        //sItem2->setData(QVariant(QColor( Qt::transparent )), Qt::DecorationRole);
        sItem2->setText(QString("RGB %1").arg(i));
        sItem2->setIcon(QIcon(*pixmap));
        model->setItem(i, 0, sItem2);
    }
    presetComboBox->setModel(model);
    presetComboBox->setIconSize(pixmapSize);*/
    QPushButton *colorSchemeSettingButton = new QPushButton("Настроить");
    connect(colorSchemeSettingButton, &QPushButton::clicked, [this] () {
        if (colorSchemeWidget->isHidden())
            colorSchemeWidget->show();
        else
            colorSchemeWidget->hide();
    } );

    QHBoxLayout *settingButtonLayout = new QHBoxLayout();
    settingButtonLayout->addWidget(presetComboBox);
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
        mainLayout->addWidget(plot, 1);
        mainLayout->addLayout(plotControlsLayout);
    }
    //mainLayout->addWidget(table);

    setLayout(mainLayout);

    updatePlot = new QTimer();
    connect(updatePlot, &QTimer::timeout, this, &CustomPlotterWidget::timerTimeout);
    //updatePlot->start(dspRefreshRate);

    //connect(startStopButton, &BasePushButton::clicked, this, &CustomPlotterWidget::startStopClicked);
    connect(startStopButton, &QPushButton::clicked, this, &CustomPlotterWidget::startStopClicked);
    connect(sliderControl, &QSlider::sliderReleased, this, &CustomPlotterWidget::sliderOffset);
    connect(presetComboBox, QOverload<int>::of(&QComboBox::activated), this, &CustomPlotterWidget::currentSchemeChanged);
}

CustomPlotterWidget::~CustomPlotterWidget()
{


}

void CustomPlotterWidget::updateData(const quint32 &n,const quint32 &m,const quint32 &counter, const QVector<qreal> &vector)
{
    //if (sliderControl->value() == sliderControl->maximum() || (startStopButton->text() == QString("Стоп"))) {
    //sp_data->updateMatrix(n, m, vector);
    //}

    if (xMaxInterval != n || yMaxInterval != m) {
        xMaxInterval = n;
        yMaxInterval = m;
        plot->setAxisScale(QwtPlot::xBottom, -10.0, xMaxInterval);
        plot->setAxisScale(QwtPlot::yLeft, -10.0, yMaxInterval);
    }

    sp_data->updateMatrix(n, m, vector);

//    sliderControl->setMaximum(counter);
//    if (sliderControl->value() == sliderControl->maximum())
//        sliderControl->setValue(counter);

    d_spectrogram->invalidateCache();
    d_spectrogram->itemChanged();

    plot->replot();
}

void CustomPlotterWidget::forceSendColorRangesToScene()
{
    currentSchemeChanged();
}

void CustomPlotterWidget::showContour( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
    plot->replot();
}

void CustomPlotterWidget::showSpectrogram( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    d_spectrogram->setDefaultContourPen(
        on ? QPen( Qt::black, 0 ) : QPen( Qt::NoPen ) );

    plot->replot();
}

void CustomPlotterWidget::setColorMap( int type )
{
    QwtScaleWidget *axis = plot->axisWidget( QwtPlot::yRight );
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

    plot->replot();
}

void CustomPlotterWidget::setColorsMap(const ColorsMap &colors)
{
    QwtScaleWidget *axis = plot->axisWidget( QwtPlot::yRight );
    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );

    d_mapType = ColorMap::RGBMap;

    d_spectrogram->setColorMap( new LinearColorMapRGB(colors) );
    axis->setColorMap( zInterval, new LinearColorMapRGB(colors) );

    plot->replot();
}

void CustomPlotterWidget::setAlpha( int alpha )
{
    // setting an alpha value doesn't make sense in combination
    // with a color map interpolating the alpha value

    d_alpha = alpha;
    if ( d_mapType != ColorMap::AlphaMap )
    {
        d_spectrogram->setAlpha( alpha );
        plot->replot();
    }
}

void CustomPlotterWidget::setAntiAlasing(int on)
{
    if (on) {
        sp_data->setResampleMode(QwtMatrixRasterData::BilinearInterpolation);
    }
    else {
        sp_data->setResampleMode(QwtMatrixRasterData::NearestNeighbour);
    }
}

void CustomPlotterWidget::updatePlotSlot()
{
    sp_data->refreshMatrix();
    d_spectrogram->invalidateCache();
    d_spectrogram->itemChanged();

    plot->replot();
}

void CustomPlotterWidget::pannerHandler(int dx, int dy)
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

    plot->replot();
}

void CustomPlotterWidget::zoomHandler(const QRectF &rect)
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

        plot->replot();
    }

    //qDebug() << rect;
    //qDebug() << zoomer->zoomBase();
}

void CustomPlotterWidget::setItemToPresetComboBox(const quint32 &index, const MapOfColorRanges::iterator &itemOfColorMap)
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

    presetComboBox->setIconSize(pixmapSize);
}

void CustomPlotterWidget::createColorArray(const ColorRanges &colorRanges)
{
    ColorsMap colorMap;
    for (quint8 index = 0; index !=  colorRanges.colorsForGradientLabel.size(); index++) {
        colorMap.push_back(qMakePair(colorRanges.rangesForRangeSlider.at(index),
                                     colorRanges.colorsForGradientLabel.at(index)));
    }
    setColorsMap(colorMap);
    if (!displayPlot)
        emit sendColorRangesToScene(targetID, colorRanges);
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
    emit setPositionOnVectorOfDSP(targetID, slider->value());
}

void CustomPlotterWidget::timerTimeout()
{
    if (sliderControl->value() != sliderControl->maximum())
        sliderControl->setValue(sliderControl->value() + 1);
    else if (sliderControl->value() == sliderControl->maximum()) {
        sliderControl->setValue(0);
    }
}

void CustomPlotterWidget::schemeChangedSlot(QString nameScheme, QString newNameScheme, QVector<QColor> colors, QVector<int> ranges)
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

void CustomPlotterWidget::schemeDeletedSlot(QString nameScheme)
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

void CustomPlotterWidget::setSliderLimit(quint32 counter)
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


DSPSettingWidget::DSPSettingWidget(QWidget *parent) : QWidget(parent)
{
    //setTitle("Трассы");
    QMargins margin(15,5,15,5);

    //plotterWidget = new CustomPlotterWidget(10);
    //plotterWidget->hide();

    tableView = new QTableView();
    QStringList hHeader;
    hHeader.append("Номер");
    hHeader.append("Приоритет");
    hHeader.append("ДСП");
    hHeader.append("СЦ");
    hHeader.append("Отрисовка");
    hHeader.append("Трек");

    QStandardItemModel *modelForTable = new QStandardItemModel();
    modelForTable->setHorizontalHeaderLabels(hHeader);

    tableView->setModel(modelForTable);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->verticalHeader()->setMinimumWidth(40);
    tableView->verticalHeader()->hide();
    //tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //tableView->horizontalHeader()->setSectionsMovable(true);

    /*ComboBoxItemDelegate* cbid = new ComboBoxItemDelegate(ComboBoxDelegateType::TypeDSP);
    tableView->setItemDelegateForColumn(2, cbid);
    ComboBoxItemDelegate* cbid2 = new ComboBoxItemDelegate(ComboBoxDelegateType::TargetDSP);
    tableView->setItemDelegateForColumn(1, cbid2);
    connect(this, &DSPSettingWidget::updateTraceInfoToDelegate, [this, cbid2] (qint32 trackId, bool isReset) {
        cbid2->setTraceInfo(trackId, isReset);
    });*/
    /*ComboBoxItemDelegate* cbid3 = new ComboBoxItemDelegate(ComboBoxDelegateType::PlottingDSP);
    tableView->setItemDelegateForColumn(2, cbid3);*/


    QPushButton *acceptDsp = new QPushButton("Применить");
    QPushButton *plottingDsp = new QPushButton("Отобразить");
    //connect(acceptDsp, &QPushButton::clicked, )

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(plottingDsp);
    buttonLayout->addWidget(acceptDsp);

    initMenu();
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableView, &QTableView::customContextMenuRequested, this, &DSPSettingWidget::customMenuForTable);

    QVBoxLayout *dspMainLayout = new QVBoxLayout();
    //dspMainLayout->addLayout(tableMainLayout);
    //dspMainLayout->addWidget(dspSettingGroupBox);
    dspMainLayout->addWidget(tableView);
    //dspMainLayout->addLayout(buttonLayout);

    QGroupBox *dspGroupBox = new QGroupBox("Установка и настройка ДСП");
    dspGroupBox->setLayout(dspMainLayout);

    QVBoxLayout *mainLayout = new  QVBoxLayout();
    mainLayout->addSpacerItem(new QSpacerItem(1, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
    //mainLayout->addLayout(comboBoxLayout);
    //mainLayout->addWidget(presetColorSchemeGroupBox);
    mainLayout->addWidget(dspGroupBox);
    //mainLayout->addWidget(plot);
    //mainLayout->addWidget(table);

    setLayout(mainLayout);
}

void DSPSettingWidget::customMenuForTable(QPoint pos)
{
    tableMenu->move(tableView->viewport()->mapToGlobal(pos));
    tableMenu->show();
}

void DSPSettingWidget::updateTraceInfo(qint32 trackId, bool isReset)
{
    emit updateTraceInfoToDelegate(trackId, isReset);
}

void DSPSettingWidget::initMenu()
{
    tableMenu = new QMenu();
    QAction *action = new QAction("Добавить ДСП");
    connect(action, &QAction::triggered,
            [this] () {
        QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tableView->model());
        int selectedRow = model->rowCount();

        QStandardItem *sItem = new QStandardItem();
        sItem->setText("###");
        model->setItem(selectedRow, 0, sItem);

        QComboBox *priorityComboBox = new QComboBox();
        priorityComboBox->addItem("Да");
        priorityComboBox->addItem("Нет");
        tableView->setIndexWidget(model->index(selectedRow, 1), priorityComboBox);

        QComboBox *dspComboBox = new QComboBox();
        dspComboBox->addItem("Откл");
        dspComboBox->addItem("2 мс");
        dspComboBox->addItem("20 мс");
        tableView->setIndexWidget(model->index(selectedRow, 2), dspComboBox);

        QCheckBox *check1 = new QCheckBox();
        tableView->setIndexWidget(model->index(selectedRow, 3), check1);

        QWidget *wgt = new QWidget();
        QPushButton *button1 = new QPushButton();
        button1->setIcon(QIcon(":/Resources/Icon/llfr_ic_play_on.png"));
        button1->setFixedSize(22,22);
        //button->setAttribute(Qt::WA_AlwaysShowToolTips, true);
        button1->setToolTip("Воспроизвести");
        //button1->setEnabled(false);
        //button1->hide();

        QPushButton *button2 = new QPushButton();
        button2->setIcon(QIcon(":/Resources/Icon/llfr_ic_seek_start_on.png"));
        button2->setFixedSize(22,22);
        //button->setAttribute(Qt::WA_AlwaysShowToolTips, true);
        button2->setToolTip("Остановить");

        QHBoxLayout *mainLayout = new QHBoxLayout();
        mainLayout->setMargin(0);
        mainLayout->setSpacing(0);
        mainLayout->addWidget(button1);
        mainLayout->addWidget(button2);

        wgt->setLayout(mainLayout);

        QWidget *wgt2 = new QWidget();
        QCheckBox *check = new QCheckBox();
        QHBoxLayout *mainLayout2 = new QHBoxLayout();
        mainLayout2->setMargin(0);
        mainLayout2->setSpacing(0);
        mainLayout2->addWidget(check,0, Qt::AlignCenter);
        wgt2->setLayout(mainLayout2);

        //tableView->setIndexWidget(model->index(selectedRow, 0), wgt2);
        tableView->setIndexWidget(model->index(selectedRow, 4), wgt);

        QCheckBox *check2 = new QCheckBox();
        tableView->setIndexWidget(model->index(selectedRow, 5), check2);

    });
    tableMenu->addAction(action);

    action = new QAction("Удалить ДСП");
    connect(action, &QAction::triggered,
            [this] () {
        auto selectedList = tableView->selectionModel()->selectedRows();
        if (!selectedList.isEmpty())
            tableView->model()->removeRow(selectedList.first().row());
    });
    tableMenu->addAction(action);
}

/*DSPDataHandler::DSPDataHandler(QObject *parent) : isFFTInit(false)
{

}

DSPDataHandler::~DSPDataHandler()
{
    fftw_destroy_plan(fftwPlan);
    fftw_free(complexMatrix);
    isFFTInit = false;
}

void DSPDataHandler::newData(quint32 N, quint32 M, QVector<qint16> matrix)
{
    quint32 matrixSize = N * M;

    if (!isFFTInit)  {
        complexMatrix = fftw_alloc_complex(matrixSize);
        if( complexMatrix == NULL ){
            return;
        }
        fftwPlan = fftw_plan_dft_2d(N, M,
                                    complexMatrix, complexMatrix,
                                    FFTW_FORWARD, FFTW_ESTIMATE);
        isFFTInit = true;
    }

    for (quint32 index = 0; index != matrix.size(); index++) {
        quint32 fftMatrixIndex = index / 2;
        complexMatrix[fftMatrixIndex][0] = matrix.at(index++);
        complexMatrix[fftMatrixIndex][1] = matrix.at(index);
    }

    fftw_execute(fftwPlan);

    resultInfo.data.clear();
    resultInfo.N = N;
    resultInfo.M = M;
    for (quint32 index = 0; index != matrixSize; index++) {
        float result = moduleOfComplexValue(complexMatrix[index][0], complexMatrix[index][1]);
        result = 20 * log10(result);
        resultInfo.data.push_back(result);
    }

}

void DSPDataHandler::deletePlan()
{
    fftw_destroy_plan(fftwPlan);
    fftw_free(complexMatrix);
    isFFTInit = false;
}

float DSPDataHandler::moduleOfComplexValue(qreal &real, qreal &imag)
{
    return sqrt(pow(real, 2) + pow(imag, 2));
}*/
