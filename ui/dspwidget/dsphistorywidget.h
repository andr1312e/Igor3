#ifndef DSPSETTINGWIDGET_H
#define DSPSETTINGWIDGET_H

#include <QTableView>
#include <QWidget>
#include <QMenu>
#include <QCheckBox>
#include <QHeaderView>
#include <QVBoxLayout>

#include "customplotter.h"

class DSPHistoryWidget : public QWidget {
    Q_OBJECT
public:
    explicit DSPHistoryWidget(QWidget *parent);
    ~DSPHistoryWidget();
private:
    void CreateObjects();
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void ConnectObjects();
Q_SIGNALS:
    void ToUpdateTraceInfoToDelegate(qint32 trackId, bool isReset);

public Q_SLOTS:
    void customMenuForTable(QPoint pos);
    void updateTraceInfo(qint32 trackId, bool isReset);

private:
    void initMenu();

private:
    QVBoxLayout *m_mainLayout;

    QHBoxLayout *m_buttonLayout;
    QPushButton *m_acceptDsp;
    QPushButton *m_plotDsp;

    QGroupBox *m_dspGroupBox;
    QVBoxLayout *m_groupBoxLayout;
    QMenu *m_tableMenu;
    QTableView *m_tableView;



    CustomPlotterWidget *plotterWidget;

private:
    QStandardItemModel *m_modelForTable;
    const QStringList m_tableHeader;
};
#endif // DSPSETTINGWIDGET_H
