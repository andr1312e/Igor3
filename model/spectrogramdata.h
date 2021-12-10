#ifndef SPECTROGRAMDATA_H
#define SPECTROGRAMDATA_H

#include <QtGlobal>
#include <QVector>
#include <QtGlobal>

#include <qwt_matrix_raster_data.h>

class SpectrogramData : public QwtMatrixRasterData {

public:
    SpectrogramData();

    ~SpectrogramData();

    void RefreshMatrix();
    void UpdateMatrix(const QVector<qreal> &vector);
    void SetBackground();

    qint32 GetXMinAxisValue() const;
    void SetXMinAxisValue(const qint32 &value);

    qint32 GetYMinAxisValue() const;
    void SetYMinAxisValue(const qint32 &value);

    qint32 GetXMaxAxisValue() const;
    void SetXMaxInterval(const qint32 &value);

    qint32 GetYMaxAxisValue() const;
    void SetYMaxInterval(const qint32 &value);

private:

    qint32 xMinValueAxis = 0;
    qint32 yMinValueAxis =0;
    qint32 xMaxInterval = 32;
    qint32 yMaxInterval = 2512;
    static const quint16 zMaxInterval = 100;


};

#endif // SPECTROGRAMDATA_H
