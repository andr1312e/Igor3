#ifndef DSPSETTINGWIDGET_H
#define DSPSETTINGWIDGET_H

#include <QTableView>
#include <QWidget>
#include <QMenu>
#include <QCheckBox>
#include <QHeaderView>

#include "customplotter.h"

class DSPSettingWidget : public QWidget {
    Q_OBJECT
public:
    explicit DSPSettingWidget(QWidget *parent = nullptr);

Q_SIGNALS:
    void updateTraceInfoToDelegate(qint32 trackId, bool isReset);

public Q_SLOTS:
    void customMenuForTable(QPoint pos);
    void updateTraceInfo(qint32 trackId, bool isReset);

private:
    void initMenu();

private:
    QTableView *tableView;
    QMenu *tableMenu;
    CustomPlotterWidget *plotterWidget;
};
#endif // DSPSETTINGWIDGET_H
