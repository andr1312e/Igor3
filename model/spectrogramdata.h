#ifndef SPECTROGRAMDATA_H
#define SPECTROGRAMDATA_H

#include <QtGlobal>
#include <QVector>
#include <QtGlobal>

#include <qwt_matrix_raster_data.h>

class SpectrogramData : public QwtMatrixRasterData {

public:
    SpectrogramData();
    SpectrogramData(quint16 xMax, quint16 yMax);
    ~SpectrogramData();
    void RefreshMatrix();
    void UpdateMatrix(const quint32 &n, const quint32 &TimeSamplesNum, const QVector<qreal> &vector);
    void SetBackground();

    quint16 GetXMaxAxisValue() const;
    void SetXMaxInterval(const quint16 &value);

    quint16 GetYMaxAxisValue() const;
    void SetYMaxInterval(const quint16 &value);

    const double xMinValueAxis = 0.0;
    const double yMinValueAxis =0.0;

private:
    quint16 xMaxInterval = 32;
    quint16 yMaxInterval = 2512;
    static const quint16 zMaxInterval = 100;


};

#endif // SPECTROGRAMDATA_H
