#ifndef CIRCLEPALETTE_H
#define CIRCLEPALETTE_H

#include <QLabel>
#include <QPoint>
#include <QJsonObject>
#include <QSettings>
#include <QSet>
#include <QLineEdit>

class QComboBox;
class QGroupBox;
class QProgressBar;

struct ColorRanges {
    ColorRanges() :
        pointsForCirclePalette(),
        colorsForGradientLabel(),
        rangesForRangeSlider(),
        defaultPoints({QPoint(80,160), QPoint(250,40), QPoint(420,140), QPoint(410, 260)}),
        defaultColors({QColor("#13ed1b"), QColor("#f8f008"), QColor("#fb0505"), QColor("#e6199b")})
    {}
    ColorRanges(QVector<QPoint> points, QVector<QColor> colors, QVector<int> ranges) :
        pointsForCirclePalette(points),
        colorsForGradientLabel(colors),
        rangesForRangeSlider(ranges),
        defaultPoints({QPoint(80,160), QPoint(250,40), QPoint(420,140), QPoint(410, 260)}),
        defaultColors({QColor("#13ed1b"), QColor("#f8f008"), QColor("#fb0505"), QColor("#e6199b")})
    {}
    void defaultInit(const quint8 &pointsNumber) {
        pointsForCirclePalette.clear();
        colorsForGradientLabel.clear();
        rangesForRangeSlider.clear();
        for (quint8 index = 0; index != pointsNumber; index++ ) {
            pointsForCirclePalette.push_back(defaultPoints.at(index));
            colorsForGradientLabel.push_back(defaultColors.at(index));
        }
        if (pointsNumber == 2) {
            rangesForRangeSlider << 0 << 100;
        }
        else if (pointsNumber == 3) {
            rangesForRangeSlider << 0 << 50 << 100;
        }
        else if (pointsNumber == 4) {
            rangesForRangeSlider << 0 << 33 << 66 << 100;
        }
    }
    void pop_back() {
        if (pointsForCirclePalette.size() == 2)
            return;
        pointsForCirclePalette.pop_back();
        colorsForGradientLabel.pop_back();
        rangesForRangeSlider.pop_back();
    }

    void push_back() {//240 - 240
        const qint16 vectorSize = colorsForGradientLabel.size();
        if (vectorSize == 4)
            return;
        QSet<QString> currentColorsSet;
        for (quint8 index = 0; index != colorsForGradientLabel.size(); index++) {
            currentColorsSet.insert(colorsForGradientLabel.at(index).name());
        }

        quint8 indexForPushing = 0;
        while (indexForPushing != defaultColors.size()) {
            auto itemColorSet = currentColorsSet.find(defaultColors.at(indexForPushing).name());
            if (itemColorSet != currentColorsSet.end())
                indexForPushing++;
            else
                break;
        }
        if (indexForPushing != defaultColors.size()) {
            colorsForGradientLabel.push_back(defaultColors.at(indexForPushing));
            pointsForCirclePalette.push_back(defaultPoints.at(indexForPushing));
        }
        else {
            colorsForGradientLabel.push_back(QColor("#808080"));
            pointsForCirclePalette.push_back(QPoint(239,235));
        }

        int lastElement = *(rangesForRangeSlider.end() - 1),
                preLastElement = *(rangesForRangeSlider.end() - 2),
                newEndElement = 100;
        if (lastElement > 90) {
            if ((lastElement - preLastElement) < 10) {
                lastElement -= 10;
                preLastElement -= 10;
            }
            else {
                lastElement -= 10;
            }
        }
        rangesForRangeSlider[rangesForRangeSlider.size() - 1] = lastElement;
        rangesForRangeSlider[rangesForRangeSlider.size() - 2] = preLastElement;
        rangesForRangeSlider.push_back(newEndElement);
    }

    QVector<QPoint> pointsForCirclePalette;
    QVector<QColor> colorsForGradientLabel;
    QVector<int> rangesForRangeSlider;

private:
    QVector<QPoint> defaultPoints;
    QVector<QColor> defaultColors;

};

using MapOfColorRanges = QMap<QString, ColorRanges>;
using ColorsMap = QVector<QPair<float, QColor>>;

class GradientLabel : public QLabel {
    Q_OBJECT

public:
    GradientLabel(QWidget *widget = nullptr);
    const QVector<QColor> &getColors() const;
    const QVector<int>& getPersents() const;

public slots:
    void setColors(QVector<QColor> colors);
    void setPersents(QVector<int> persents);


protected:
    void paintEvent(QPaintEvent *event);

public:
    QVector<QColor> vectorOfColors;
    QVector<int> vectorOfPersent;
};

enum SliderHandleSelected {
    FirstHandle = 0,
    SecondHandle = 1,
    ThirdHandle = 2,
    FourthHandle = 3,

    NoMarkSelected = 10
};

enum Option {
    //NoHandle = 0x0,
    DoubleHandles = 0x1,
    TripleHandles = 0x2,
    QuadrupleHandles = 0x3
};

class RangeSlider : public QWidget
{
    Q_OBJECT
public:
    RangeSlider( QWidget* aParent = nullptr);
    RangeSlider( Qt::Orientation ori, Option t = TripleHandles, QWidget* aParent = nullptr);

    QSize minimumSizeHint() const override;


protected:
    void paintEvent(QPaintEvent* aEvent) override;
    void mousePressEvent(QMouseEvent* aEvent) override;
    void mouseMoveEvent(QMouseEvent* aEvent) override;
    void mouseReleaseEvent(QMouseEvent* aEvent) override;

    QRectF handleRect(int index) const;

signals:
    void lowerValueChanged(int aLowerValue);
    void upperValueChanged(int aUpperValue);
    void rangeChanged(QVector<int> range);


public slots:
    //void setMinimum(int aMinimum);
    //void setMaximum(int aMaximum);
    void setRangeType(Option t);
    void setRangeType(Option t, QVector<int> vector);

private:
    float currentPercentage();
    int validLength() const;
    QPair<int, int> limits() const;

    SliderHandleSelected markSelected;

    int mMinimum;
    int mMaximum;
    float mDelta;

    QVector<int> handlesValue;

    Qt::Orientation orientation;
    Option type;
};

namespace PrestoPalette
{
enum GlobalGamutShape
{
    GamutShapeNone,
    GamutShapeLine,
    GamutShapeTriangle,
    GamutShapeSquare,
    GamutShapeLast
};

enum GlobalWheelShape
{
    WheelShapeCoarse,
    WheelShapeFine,
    WheelShapeLast
};
}

class ColorPoint
{
      public:
    QColor color;
    QPoint point;
    qreal angle; // in radians
    bool secondaryOn;

    ColorPoint(QPoint p)
    {
        this->point = p;
        this->angle = 0;
        this->secondaryOn = false; // secondaries are off by default
        this->color = QColor();
    }

    void SaveState(QJsonObject &saveState)
    {
        saveState["secondary"] = this->secondaryOn;
        saveState["x"] = this->point.x();
        saveState["y"] = this->point.y();
    }

    void LoadState(QJsonObject &loadState)
    {
        this->secondaryOn = loadState["secondary"].toBool();
        this->point.setX(loadState["x"].toInt());
        this->point.setY(loadState["y"].toInt());
    }
};

class CirclePalette : public QWidget
{
    Q_OBJECT
      public:
    explicit CirclePalette(QWidget *parent = 0);
    ~CirclePalette(void);

    QWidget *drawnElements;
    QLabel *colorWheel;
    QLabel *backgroundWheel;
    QVector<QColor> selectedColors;
    QColor lightingColor;
    PrestoPalette::GlobalGamutShape gamutShape;
    PrestoPalette::GlobalWheelShape wheelShape;

    bool enableLighting;

    void ChangeGamutShape(PrestoPalette::GlobalGamutShape shape);
    void ChangeWheelShape(PrestoPalette::GlobalWheelShape shape);
    void RecreateGamut(QVector<QPoint> vPoints);

    void CalculateCentroid(int circleRadius);
    void SaveState(QJsonObject &saveState);
    void LoadState(QJsonObject &loadState);
    QVector<QPoint> getVectorOfPoint();


      signals:
    void selectedColorsChanged();
    void hoverColor(const QColor &color);
    void lightingColorChanged(const QColor &color);
    void newColors(QVector<QColor> colors);

      private slots:

      private:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

      private:
    int primaryRadius = 15;
    int secondaryRadius = 6;
    int centroidRadius = 5;

    QVector<ColorPoint *> pointsLine;
    QVector<ColorPoint *> pointsTriangle;
    QVector<ColorPoint *> pointsQuad;

    QPoint centroidTriangle;
    QPoint centroidQuad;

    bool centroidTriangleOn;
    bool centroidQuadOn;

    QVector<ColorPoint *> *points;

    QPoint *lighting;
    QPoint *centroid;

    QPoint dragStartPosition;
    bool isDragging;
    QPoint *dragPoint;
    QPoint relativeDistance;
    QPixmap circlePic;

    const QVector<QPoint> defaultPoints;

    void create_gamut_line();
    void create_gamut_triangle();
    void create_gamut_square();    
    void destroy_gamut();

    QPointF FindIntersectionWithCircle(const QPoint &p1, const QPoint &p2, const qreal radius);
    void _draw_primary_imp(QPainter &painter, const QPoint &p, int circleRadius, bool isCentroid);
    void _draw_line_imp(QPainter &painter, const QPoint &p1, const QPoint &p2, int circleRadius, bool secondaryOn);
    void _draw_centroid(QPainter &painter, int circleRadius, bool secondaryOn);
    bool _is_collision(const QPoint &circle, int circleRadius, const QPoint &hitTest);

      public:
    struct Less
    {
        Less(CirclePalette &c)
            : circlePalette(c)
        {
        }
        bool operator()(const ColorPoint &i1, const ColorPoint &i2)
        {
            return circlePalette.sort_angles(i1, i2);
        }
        CirclePalette &circlePalette;
    };

    bool sort_angles(const ColorPoint i, const ColorPoint j);
    QColor getColorAt(const QPoint &p);
    QColor getMidPointColor(QPoint p1, QPoint p2);
    QPoint getMidPoint(QPoint &p1, QPoint &p2);
};

class ColorSchemeWidget : public QWidget {
    Q_OBJECT

public:
    explicit ColorSchemeWidget(QWidget *parent = 0);
    ~ColorSchemeWidget();
    void initColorScheme();

signals:
    void sendColorMap(ColorsMap colorMap);

    void schemeChanged(QString nameScheme, QString newNameScheme, QVector<QColor> colors, QVector<int> ranges);
    void schemeDeleted(QString nameScheme);

public slots:
    void setNumberOfPoints();
    void acceptColors();

private:
    void initPresetSettings();
    void setItemToPresetComboBox(const quint32 &index, const MapOfColorRanges::iterator &itemOfColorMap);
    void fillMainColorsOptionsGroupBox(const MapOfColorRanges::iterator &itemOfColorMap);

private slots:
    void insertScheme();
    void deleteScheme();
    void currentSchemeChanged();

private:
    CirclePalette *circlePalette;
    RangeSlider *rangeSlider;
    GradientLabel *gradientLabel;
    QComboBox *presetComboBox, *numberOfPointsComboBox;
    const QString dspSettingFilePath;
    MapOfColorRanges mapOfColorRanges;
    QLineEdit *schemeNameLineEdit;
    QGroupBox *mainColorsOptionsGroupBox;
    QProgressBar *progressBar;

    qint16 editableScheme;
};

#endif // CIRCLEPALETTE_H
