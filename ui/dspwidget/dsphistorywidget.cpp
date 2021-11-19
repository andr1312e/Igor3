#include "dsphistorywidget.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QStandardItemModel>

DSPHistoryWidget::DSPHistoryWidget(QWidget *parent)
    : QWidget(parent)
    , m_tableHeader({"Номер", "Приоритет", "ДСП", "СЦ", "Отрисовка", "Трек"})
{
    CreateObjects();
    CreateUI();
    initMenu();
    InsertWidgetsIntoLayouts();
    FillUI();
    ConnectObjects();


}

DSPHistoryWidget::~DSPHistoryWidget()
{

}

void DSPHistoryWidget::CreateObjects()
{
    m_modelForTable= new QStandardItemModel();
    m_modelForTable->setHorizontalHeaderLabels(m_tableHeader);

}

void DSPHistoryWidget::CreateUI()
{
    m_mainLayout= new QVBoxLayout();

    m_buttonLayout = new QHBoxLayout();
    m_acceptDsp=new QPushButton();
    m_plotDsp=new QPushButton();

    m_dspGroupBox = new QGroupBox();
    m_groupBoxLayout=new QVBoxLayout();
    m_tableView = new QTableView();
}

void DSPHistoryWidget::InsertWidgetsIntoLayouts()
{
    m_buttonLayout->addWidget(m_plotDsp);
    m_buttonLayout->addWidget(m_acceptDsp);

    m_groupBoxLayout->addWidget(m_tableView);
    m_groupBoxLayout->addWidget(m_tableMenu);

    m_dspGroupBox->setLayout(m_groupBoxLayout);

    m_mainLayout->addLayout(m_buttonLayout);
    m_mainLayout->addSpacerItem(new QSpacerItem(1, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_mainLayout->addWidget(m_dspGroupBox);

    setLayout(m_mainLayout);
}

void DSPHistoryWidget::FillUI()
{
    m_dspGroupBox->setTitle("Установка и настройка ДСП");
    m_acceptDsp->setText("Сохранить???");
    m_plotDsp->setText("Отобразить ДСП");
    setWindowTitle("История РСП");
    m_tableView->setModel(m_modelForTable);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->verticalHeader()->setMinimumWidth(80);
    m_tableView->verticalHeader()->hide();
    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
}

void DSPHistoryWidget::ConnectObjects()
{
    connect(m_tableView, &QTableView::customContextMenuRequested, this, &DSPHistoryWidget::customMenuForTable);
}

void DSPHistoryWidget::customMenuForTable(QPoint pos)
{
    m_tableMenu->move(m_tableView->viewport()->mapToGlobal(pos));
    m_tableMenu->show();
}

void DSPHistoryWidget::updateTraceInfo(qint32 trackId, bool isReset)
{
    emit ToUpdateTraceInfoToDelegate(trackId, isReset);
}

void DSPHistoryWidget::initMenu()
{
    m_tableMenu = new QMenu();
    QAction *action = new QAction("Добавить ДСП");
    connect(action, &QAction::triggered,
            [this] () {
        QStandardItemModel *model = qobject_cast<QStandardItemModel *>(m_tableView->model());
        int selectedRow = model->rowCount();

        QStandardItem *sItem = new QStandardItem();
        sItem->setText("###");
        model->setItem(selectedRow, 0, sItem);

        QComboBox *priorityComboBox = new QComboBox();
        priorityComboBox->addItem("Да");
        priorityComboBox->addItem("Нет");
        m_tableView->setIndexWidget(model->index(selectedRow, 1), priorityComboBox);

        QComboBox *dspComboBox = new QComboBox();
        dspComboBox->addItem("Откл");
        dspComboBox->addItem("2 мс");
        dspComboBox->addItem("20 мс");
        m_tableView->setIndexWidget(model->index(selectedRow, 2), dspComboBox);

        QCheckBox *check1 = new QCheckBox();
        m_tableView->setIndexWidget(model->index(selectedRow, 3), check1);

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
        m_tableView->setIndexWidget(model->index(selectedRow, 4), wgt);

        QCheckBox *check2 = new QCheckBox();
        m_tableView->setIndexWidget(model->index(selectedRow, 5), check2);

    });
    m_tableMenu->addAction(action);

    action = new QAction("Удалить ДСП");
    connect(action, &QAction::triggered,
            [this] () {
        auto selectedList = m_tableView->selectionModel()->selectedRows();
        if (!selectedList.isEmpty())
            m_tableView->model()->removeRow(selectedList.first().row());
    });
    m_tableMenu->addAction(action);
}

