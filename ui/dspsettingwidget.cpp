#include "dspsettingwidget.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QStandardItemModel>

DSPSettingWidget::DSPSettingWidget(QWidget *parent) : QWidget(parent)
{
    //setTitle("Трассы");
    QMargins margin(15,5,15,5);

    //plotterWidget = new CustomPlotterWidget(10);
    //plotterWidget->hide();

    tableView = new QTableView();
    QStringList hHeader;
    hHeader.append("Номер");
    hHeader.append("Приоритет");
    hHeader.append("ДСП");
    hHeader.append("СЦ");
    hHeader.append("Отрисовка");
    hHeader.append("Трек");

    QStandardItemModel *modelForTable = new QStandardItemModel();
    modelForTable->setHorizontalHeaderLabels(hHeader);

    tableView->setModel(modelForTable);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->verticalHeader()->setMinimumWidth(40);
    tableView->verticalHeader()->hide();
    //tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //tableView->horizontalHeader()->setSectionsMovable(true);

    /*ComboBoxItemDelegate* cbid = new ComboBoxItemDelegate(ComboBoxDelegateType::TypeDSP);
    tableView->setItemDelegateForColumn(2, cbid);
    ComboBoxItemDelegate* cbid2 = new ComboBoxItemDelegate(ComboBoxDelegateType::TargetDSP);
    tableView->setItemDelegateForColumn(1, cbid2);
    connect(this, &DSPSettingWidget::updateTraceInfoToDelegate, [this, cbid2] (qint32 trackId, bool isReset) {
        cbid2->setTraceInfo(trackId, isReset);
    });*/
    /*ComboBoxItemDelegate* cbid3 = new ComboBoxItemDelegate(ComboBoxDelegateType::PlottingDSP);
    tableView->setItemDelegateForColumn(2, cbid3);*/


    QPushButton *acceptDsp = new QPushButton("Применить");
    QPushButton *plottingDsp = new QPushButton("Отобразить");
    //connect(acceptDsp, &QPushButton::clicked, )

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(plottingDsp);
    buttonLayout->addWidget(acceptDsp);

    initMenu();
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableView, &QTableView::customContextMenuRequested, this, &DSPSettingWidget::customMenuForTable);

    QVBoxLayout *dspMainLayout = new QVBoxLayout();
    //dspMainLayout->addLayout(tableMainLayout);
    //dspMainLayout->addWidget(dspSettingGroupBox);
    dspMainLayout->addWidget(tableView);
    //dspMainLayout->addLayout(buttonLayout);

    QGroupBox *dspGroupBox = new QGroupBox("Установка и настройка ДСП");
    dspGroupBox->setLayout(dspMainLayout);

    QVBoxLayout *mainLayout = new  QVBoxLayout();
    mainLayout->addSpacerItem(new QSpacerItem(1, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
    //mainLayout->addLayout(comboBoxLayout);
    //mainLayout->addWidget(presetColorSchemeGroupBox);
    mainLayout->addWidget(dspGroupBox);
    //mainLayout->addWidget(plot);
    //mainLayout->addWidget(table);

    setLayout(mainLayout);
}

void DSPSettingWidget::customMenuForTable(QPoint pos)
{
    tableMenu->move(tableView->viewport()->mapToGlobal(pos));
    tableMenu->show();
}

void DSPSettingWidget::updateTraceInfo(qint32 trackId, bool isReset)
{
    emit updateTraceInfoToDelegate(trackId, isReset);
}

void DSPSettingWidget::initMenu()
{
    tableMenu = new QMenu();
    QAction *action = new QAction("Добавить ДСП");
    connect(action, &QAction::triggered,
            [this] () {
        QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tableView->model());
        int selectedRow = model->rowCount();

        QStandardItem *sItem = new QStandardItem();
        sItem->setText("###");
        model->setItem(selectedRow, 0, sItem);

        QComboBox *priorityComboBox = new QComboBox();
        priorityComboBox->addItem("Да");
        priorityComboBox->addItem("Нет");
        tableView->setIndexWidget(model->index(selectedRow, 1), priorityComboBox);

        QComboBox *dspComboBox = new QComboBox();
        dspComboBox->addItem("Откл");
        dspComboBox->addItem("2 мс");
        dspComboBox->addItem("20 мс");
        tableView->setIndexWidget(model->index(selectedRow, 2), dspComboBox);

        QCheckBox *check1 = new QCheckBox();
        tableView->setIndexWidget(model->index(selectedRow, 3), check1);

        QWidget *wgt = new QWidget();
        QPushButton *button1 = new QPushButton();
        button1->setIcon(QIcon(":/Resources/Icon/llfr_ic_play_on.png"));
        button1->setFixedSize(22,22);
        //button->setAttribute(Qt::WA_AlwaysShowToolTips, true);
        button1->setToolTip("Воспроизвести");
        //button1->setEnabled(false);
        //button1->hide();

        QPushButton *button2 = new QPushButton();
        button2->setIcon(QIcon(":/Resources/Icon/llfr_ic_seek_start_on.png"));
        button2->setFixedSize(22,22);
        //button->setAttribute(Qt::WA_AlwaysShowToolTips, true);
        button2->setToolTip("Остановить");

        QHBoxLayout *mainLayout = new QHBoxLayout();
        mainLayout->setMargin(0);
        mainLayout->setSpacing(0);
        mainLayout->addWidget(button1);
        mainLayout->addWidget(button2);

        wgt->setLayout(mainLayout);

        QWidget *wgt2 = new QWidget();
        QCheckBox *check = new QCheckBox();
        QHBoxLayout *mainLayout2 = new QHBoxLayout();
        mainLayout2->setMargin(0);
        mainLayout2->setSpacing(0);
        mainLayout2->addWidget(check,0, Qt::AlignCenter);
        wgt2->setLayout(mainLayout2);

        //tableView->setIndexWidget(model->index(selectedRow, 0), wgt2);
        tableView->setIndexWidget(model->index(selectedRow, 4), wgt);

        QCheckBox *check2 = new QCheckBox();
        tableView->setIndexWidget(model->index(selectedRow, 5), check2);

    });
    tableMenu->addAction(action);

    action = new QAction("Удалить ДСП");
    connect(action, &QAction::triggered,
            [this] () {
        auto selectedList = tableView->selectionModel()->selectedRows();
        if (!selectedList.isEmpty())
            tableView->model()->removeRow(selectedList.first().row());
    });
    tableMenu->addAction(action);
}

