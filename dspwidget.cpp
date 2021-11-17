#include "dspwidget.h"
#include <QLabel>
#include <QVector>
#include <QBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QDataStream>
#include <QDateTime>
#include <QtMath>


DSPWidget::DSPWidget(QWidget *parent)
    : QWidget(parent)
{
    QPushButton *selectFile = new QPushButton("Выбор файла");

    customWidget = new CustomPlotterWidget(10, true);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(selectFile);
    mainLayout->addWidget(customWidget);

    setLayout(mainLayout);

    connect(selectFile, &QPushButton::clicked, this, &DSPWidget::selectFileSlot);

    connect(this, &DSPWidget::setMaxCounter, customWidget, &CustomPlotterWidget::setSliderLimit);
    connect(customWidget, &CustomPlotterWidget::setPositionOnVectorOfDSP, this, &DSPWidget::setDSPDataOnPlotter);
}

DSPWidget::~DSPWidget()
{
    for (auto iter = mapOfRawData.begin(); iter != mapOfRawData.end(); iter++) {
        delete[] iter.value().data;
    }
    mapOfRawData.clear();
}

void DSPWidget::selectFileSlot()
{
    QString str = QFileDialog::getOpenFileName(0, "Open DSP", "~/Project", "*.dsp");
    if (str.isEmpty())
        return;

    QFile dspFile(str);
    if (!dspFile.open(QIODevice::ReadOnly)) {
        qDebug() << "DSPWidget::selectFileSlot - Can't open dsp data file";
        return;
    }

    QByteArray data(dspFile.readAll());

    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_3);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    qint64 counter = 0;
    RawBlockDSP rawBlock;
    quint32 sizeDSPBlock = 0, currentPositionInBlock = 0;

    while (!in.atEnd()) {
        POIDistSpeedPortrait msg;
        in.readRawData(reinterpret_cast<char *>(&msg), sizeof(POIDistSpeedPortrait));

        if (msg.POI_DSP_Header.CurrentPartNum == 0) {
            if (sizeDSPBlock == currentPositionInBlock) {
                if (counter != 0) {
                    mapOfRawData.insert(counter, rawBlock);
                    currentPositionInBlock = 0;
                }
            }
            else {
                delete[] rawBlock.data;
                currentPositionInBlock = 0;
            }

            rawBlock.header = msg.POI_DSP_Header;
            counter = QDateTime::currentSecsSinceEpoch();

            sizeDSPBlock = 2 * msg.POI_DSP_Header.DistSamplesNum * msg.POI_DSP_Header.TimeSamplesNum;
            rawBlock.data = new short[sizeDSPBlock];
        }

        quint32 currentSizeData = 2 * msg.POI_DSP_Header.TimeSamplesNumInPack * msg.POI_DSP_Header.DistSamplesNum;
        for (quint32 index = 0; index != currentSizeData; index++, currentPositionInBlock++) {
            rawBlock.data[currentPositionInBlock] = msg.data[index];
        }

    };

    if (sizeDSPBlock == currentPositionInBlock) {
        if (counter != 0)
            mapOfRawData.insert(counter, rawBlock);
    }
    else {
        delete[] rawBlock.data;
    }

    dspFile.close();

    emit setMaxCounter(mapOfRawData.size());

    calcAmp();
}

void DSPWidget::setDSPDataOnPlotter(int targetID, quint32 counter)
{
    if (counter >= vectorOfData.size())
        return;
    auto &firstFrame = vectorOfData.at(counter);
    customWidget->updateData(firstFrame.header.DistSamplesNum, firstFrame.header.TimeSamplesNum, counter, firstFrame.data);
}

void DSPWidget::calcAmp()
{
    quint16 lastTimeSamples = 0;
    vectorOfData.clear();
    FFTWInfo fftwInfo;
    for (auto &item : mapOfRawData) {
        auto &dSamples = item.header.DistSamplesNum, &tSamples = item.header.TimeSamplesNum;

        //quint32 size = 2 * dSamples * tSamples;
        //short *tdata = new short[size];

        //transposeMatrix(tSamples, dSamples,
        //                reinterpret_cast<int *>(item.data), reinterpret_cast<int *>(tdata));

        AmpBlockDSP resultBlock;
        resultBlock.header = item.header;

        if (lastTimeSamples == 0 || lastTimeSamples != tSamples) {
            fftwInfo.complexMatrix = fftw_alloc_complex(tSamples);
            fftwInfo.fftwPlan = fftw_plan_dft_1d(tSamples,
                                                 fftwInfo.complexMatrix, fftwInfo.complexMatrix,
                                                 FFTW_FORWARD, FFTW_ESTIMATE);
            lastTimeSamples = tSamples;
        }
        for (quint16 indexOfDist = 0; indexOfDist != dSamples; indexOfDist++) {

            for (int index = 0, indexFFT = 0; index != tSamples; index++, indexFFT++) {
//                fftwInfo.complexMatrix[indexFFT][0] = tdata[indexOfDist * tSamples + 2 * index];
//                fftwInfo.complexMatrix[indexFFT][1] = tdata[indexOfDist * tSamples + 2 * index + 1];
                fftwInfo.complexMatrix[indexFFT][0] = item.data[2 * (dSamples * index + indexOfDist)];
                fftwInfo.complexMatrix[indexFFT][1] = item.data[2 * (dSamples * index + indexOfDist) + 1];
            }

            fftw_execute(fftwInfo.fftwPlan);

            for (int index = 0; index != tSamples; index++) {
                qreal result = moduleOfComplexValue(fftwInfo.complexMatrix[index][0],
                                                    fftwInfo.complexMatrix[index][1]);
                result = 20. * log10(result);
                resultBlock.data.append(result);
            }
        }

        vectorOfData.append(resultBlock);

        //delete[] tdata;
    }

    fftw_destroy_plan(fftwInfo.fftwPlan);
    fftw_free(fftwInfo.complexMatrix);

    setDSPDataOnPlotter(0, 0);
}

void DSPWidget::transposeMatrix(qint32 timeSamples, qint32 distSamples, int *in, int *out)
{
    #pragma omp parallel for
    for(int n = 0; n < distSamples * timeSamples; n++) {
        int i = n / timeSamples;
        int j = n % timeSamples;
        out[n] = in[distSamples * j + i];
    }
}

qreal DSPWidget::moduleOfComplexValue(qreal &real, qreal &imag)
{
    return qSqrt(real * real + imag * imag);
}
