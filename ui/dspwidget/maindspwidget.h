#ifndef DSPWIDGET_H
#define DSPWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>

#include "ui/dspwidget/customplotter.h"

#include "presenter/dsppresenter.h"

#include "ui/dspwidget/topuserpanel.h"

#include "ui/dspwidget/dsphistorywidget.h"
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
    void OnSetDSPDataOnPlotter(quint32 targetId, quint32 counter);

private:
    QVBoxLayout *m_mainLayout;

    TopUserPanel *m_topUserPanel;

    CustomPlotterWidget *m_customPlotterWidget;

    DSPHistoryWidget *m_dspHistoryWidget;
private:
    DspPresenter *m_presenter;

};

#endif // DSPWIDGET_H
