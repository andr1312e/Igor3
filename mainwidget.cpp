#include "mainwidget.h"

#include <QMessageBox>
#include <QMessageBox>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    QPushButton *selectFileButton = new QPushButton("Выбор файла");

    m_customPlotterWidget = new CustomPlotterWidget(10, true, this);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(selectFileButton);
    mainLayout->addWidget(m_customPlotterWidget);

    setLayout(mainLayout);

    connect(selectFileButton, &QPushButton::clicked, this, &MainWidget::OnFileSelect);
    connect(m_customPlotterWidget, &CustomPlotterWidget::ToSetDSPDataOnPlotter, this, &MainWidget::OnSetDSPDataOnPlotter);
}

MainWidget::~MainWidget()
{
    delete m_customPlotterWidget;
    //qDeleteAll???
    for (QMap<quint32, RawBlockDSP>::iterator iter = m_mapOfRawData.begin(); iter != m_mapOfRawData.end(); iter++) {
        delete[] iter.value().data;
    }
    //++iter ? iter++ clear??
    m_mapOfRawData.clear();

}

void MainWidget::OnFileSelect()
{
    QString dspFilePath = QFileDialog::getOpenFileName(nullptr, "Open DSP", "~/Project", "*.dsp");
    if (dspFilePath.isEmpty())
        return;

    QFile dspFile(dspFilePath);
    if (!dspFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "MainWidget::OnFileSelect() - Не можем открыть файл", dspFile.errorString());
        return;
    }

    QByteArray data(dspFile.readAll());
    dspFile.close();

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
                    m_mapOfRawData.insert(counter, rawBlock);
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
            m_mapOfRawData.insert(counter, rawBlock);
    }
    else {
        delete[] rawBlock.data;
    }
    m_customPlotterWidget->SetSliderLimit(m_mapOfRawData.size());

    CalculateAmplitude();
}

void MainWidget::OnSetDSPDataOnPlotter(int targetID, quint32 counter)
{
    Q_UNUSED(targetID)//int targetID что за параметр
    if (counter >= (quint32)m_vectorOfData.size())
        return;
    const AmpBlockDSP &firstFrame = m_vectorOfData.at(counter);
    m_customPlotterWidget->UpdateData(firstFrame.header.DistSamplesNum, firstFrame.header.TimeSamplesNum, counter, firstFrame.data);
}

void MainWidget::CalculateAmplitude()
{
    quint16 lastTimeSamples = 0;
    m_vectorOfData.clear();
    FFTWInfo fftwInfo;
    for (RawBlockDSP &item : m_mapOfRawData) {
        auto &dSamples = item.header.DistSamplesNum, &tSamples = item.header.TimeSamplesNum;
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
                fftwInfo.complexMatrix[indexFFT][0] = item.data[2 * (dSamples * index + indexOfDist)];
                fftwInfo.complexMatrix[indexFFT][1] = item.data[2 * (dSamples * index + indexOfDist) + 1];
            }

            fftw_execute(fftwInfo.fftwPlan);

            for (int index = 0; index != tSamples; index++) {
                qreal result = MainWidget::AbsoluteOfComplexValue(fftwInfo.complexMatrix[index][0],
                        fftwInfo.complexMatrix[index][1]);
                result = 20. * log10(result);
                resultBlock.data.append(result);
            }
        }

        m_vectorOfData.push_back(resultBlock);
    }

    fftw_destroy_plan(fftwInfo.fftwPlan);
    fftw_free(fftwInfo.complexMatrix);

    OnSetDSPDataOnPlotter(0, 0);
}

void MainWidget::TransposeMatrix(qint32 timeSamples, qint32 distSamples, int *in, int *out)
{
    //МЕТОД НЕ ИСПОЛЬЗУЕТСЯ
#pragma omp parallel for//зачем  код не на базе технологии OpenMP???
    for(int n = 0; n < distSamples * timeSamples; n++) {
        int i = n / timeSamples;
        int j = n % timeSamples;
        out[n] = in[distSamples * j + i];
    }
}

qreal MainWidget::AbsoluteOfComplexValue(qreal &real, qreal &imaginary)
{
    return qSqrt(real * real + imaginary * imaginary);
}
