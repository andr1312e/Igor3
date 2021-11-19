#ifndef CIRCLEPALETTE_H
#define CIRCLEPALETTE_H

#include <QLabel>
#include <QPoint>
#include <QJsonObject>
#include <QSettings>
#include <QSet>
#include <QLineEdit>

#include "enums/prestoPalette.h"

#include "structs/colorscheme/colorranges.h"

#include "model/colorpoint.h"

#include "ui/colorscheme/gradientInfo/gradientlabel.h"





class CirclePalette : public QWidget
{
    Q_OBJECT
public:
    explicit CirclePalette(QWidget *parent);
    ~CirclePalette();

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
    QVector<QPoint> GetVectorOfPoint();


Q_SIGNALS:
    void selectedColorsChanged();
    void hoverColor(const QColor &color);
    void lightingColorChanged(const QColor &color);
    void ToSetColors(QVector<QColor> &colors);

private Q_SLOTS:

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

    bool sort_angles(const ColorPoint &i, const ColorPoint &j);
    QColor getColorAt(const QPoint &p);
    QColor getMidPointColor(QPoint p1, QPoint p2);
    QPoint getMidPoint(QPoint &p1, QPoint &p2);
};



#endif // CIRCLEPALETTE_H
