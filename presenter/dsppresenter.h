#ifndef PRESENTER_DSPPRESENTER_H
#define PRESENTER_DSPPRESENTER_H

#include <list>
#include <forward_list>

#include <QObject>
#include <QHash>
#include <QDataStream>
#include <QFile>
#include <QDateTime>
#include <QtMath>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QDesktopServices>

#include "ImageMagick-6/Magick++.h"
#include "ImageMagick-6/Magick++/STL.h"
#include "ImageMagick-6/Magick++/Geometry.h"
#include "ImageMagick-6/Magick++/Image.h"
#include "ImageMagick-6/Magick++/Pixels.h"
#include "ImageMagick-6/Magick++/Color.h"

#include "structs/RegFileHeader.h"
#include "structs/RawBlockDSP.h"
#include "structs/POIDistSpeedPortrait.h"
#include "structs/AmpBlockDSP.h"
#include "structs/fftwinfo.h"


using MapOfRawDSPData = QHash<quint32, RawBlockDSP>;
using VectorOfDSPData = QVector<AmpBlockDSP>;
using ListOfImages = std::forward_list<QImage>;

class DspPresenter: public QObject
{
    Q_OBJECT
public:
    explicit DspPresenter(QObject *parent);
    ~DspPresenter();
Q_SIGNALS:
    void ToSetTitileInfo(int &dspType, float &sensorAzm, float &sensorUgm);
    void ToSetSliderLimit(int limit);
    void ToSetDSPDataOnPlotter(int frame);
    void ToConverted(bool state);
public Q_SLOTS:
    void OnTheadConvertToGif();
public:

    quint64 GetElapsedTimeOfTwoImages();

    void ReadDspFromFile(const QString &fileName);
    int GetVectorOfDataElementsCount();
    const AmpBlockDSP GetElementOfData(int elementIndex);
    void ClearImagesList();
    void OnAppendImageIntoList(const QImage &image);
private:
    void ConvertQPixmapListToImageMaigckList(std::forward_list<Magick::Image> &listOfMagickImages);
    void SaveImageFromImageMagickListImages(std::forward_list<Magick::Image> &listOfMagickImages);
    void CalculateAmplitude();
    void ClearRepository();
    static qreal AbsoluteOfComplexValue(qreal &real, qreal &imaginary) ;
private:
    Magick::Image *QImageToImage(const QImage &pixmap);
private:
    MapOfRawDSPData m_mapOfRawData;
    VectorOfDSPData m_vectorOfData;
    ListOfImages m_listOfImages;

};

#endif // PRESENTER_DSPPRESENTER_H
