#include "spectrogramdata.h"

SpectrogramData::SpectrogramData()
{
    setInterval( Qt::XAxis, QwtInterval( 0, xMaxInterval ) );
    setInterval( Qt::YAxis, QwtInterval( 0, yMaxInterval ) );
    setInterval( Qt::ZAxis, QwtInterval( 0.0, zMaxInterval ) );

    setResampleMode(QwtMatrixRasterData::NearestNeighbour);

    RefreshMatrix();
}

SpectrogramData::SpectrogramData(quint16 xMax, quint16 yMax) {
    setInterval( Qt::XAxis, QwtInterval( 0, xMax ) );
    setInterval( Qt::YAxis, QwtInterval( 0, yMax ) );
    setInterval( Qt::ZAxis, QwtInterval( 0.0, zMaxInterval ) );

    setResampleMode(QwtMatrixRasterData::NearestNeighbour);

    SetBackground();
}

SpectrogramData::~SpectrogramData()
{

}

void SpectrogramData::RefreshMatrix()
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

void SpectrogramData::UpdateMatrix(const quint32 &DistSamplesNum, const quint32 &TimeSamplesNum, const QVector<qreal> &data)
{
    setInterval( Qt::XAxis, QwtInterval( xMinValueAxis, xMaxInterval ) );
    setInterval( Qt::YAxis, QwtInterval( yMinValueAxis, yMaxInterval ) );
    setValueMatrix(data, DistSamplesNum);
}

void SpectrogramData::SetBackground()
{
    const quint32 vecSize = 110;
    QVector<double> zVector(vecSize * vecSize, 0);

    setValueMatrix(zVector, vecSize);
}

quint16 SpectrogramData::GetXMaxAxisValue() const
{
    return xMaxInterval;
}

void SpectrogramData::SetXMaxInterval(const quint16 &value)
{
    xMaxInterval = value;
}

quint16 SpectrogramData::GetYMaxAxisValue() const
{
    return yMaxInterval;
}

void SpectrogramData::SetYMaxInterval(const quint16 &value)
{
    yMaxInterval = value;
}
