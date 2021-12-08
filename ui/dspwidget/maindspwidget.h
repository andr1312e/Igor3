#ifndef DSPWIDGET_H
#define DSPWIDGET_H

#include <QWidget>
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>
#include <QtGlobal>
#include <QSharedPointer>
#include <QApplication>
#include <QCursor>
#include <QtConcurrent/QtConcurrent>
#include "ui/dspwidget/customplotter.h"

#include "presenter/dsppresenter.h"

#include "ui/dspwidget/topuserpanel.h"

#include "ui/colorscheme/maincolorschemewidget.h"

class MainDspWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainDspWidget(QWidget *parent);
    ~MainDspWidget();
private:
    void CreateObjects();
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void ConnectObjects();
private Q_SLOTS:
    void OnSetTitileInfo(int &dspType, float &sensorAzm, float &sensorUgm);
    void OnRequestDSPData(int frame);
    void OnSetImagesToGif();
    void OnConverted();
protected:
    void timerEvent(QTimerEvent *event) override;
private:
    void InitWindowTitle(int dspType);
private:
    QVBoxLayout *m_mainLayout;

    TopUserPanel *m_topUserPanel;

    CustomPlotterWidget *m_customPlotterWidget;

    QProgressBar *m_convertingProgressBar;
private:
    DspPresenter *m_presenter;

};

#endif // DSPWIDGET_H
