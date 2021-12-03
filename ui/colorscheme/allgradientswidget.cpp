#include "ui/colorscheme/allgradientswidget.h"
#include <QDebug>
AllGradientsWidget::AllGradientsWidget(QSharedPointer<ColorSchemePresenter> presenter, QWidget *parent)
    : QWidget(parent)
    , m_margin(QMargins(10, 0, 15, 0))
    , m_presenter(presenter)
{
    CreateUI();
    InsertWidgetsIntoLayouts();
    FillUI();
    ConnectObjects();
}

AllGradientsWidget::~AllGradientsWidget()
{
    delete m_buttonsLayout;
    delete m_savedPreSetLayout;
    delete m_mainLayout;

    delete m_savedPreSetLabel;
    delete m_preSetComboBox;
    delete m_addButton;
    delete m_delButton;
}

void AllGradientsWidget::CreateUI()
{
    m_mainLayout=new QVBoxLayout(this);

    m_savedPreSetLayout = new QVBoxLayout(this);
    m_savedPreSetLayout->setContentsMargins(m_margin);
    m_savedPreSetLabel=new QLabel(this);
    m_preSetComboBox=new QComboBox(this);

    m_buttonsLayout= new QHBoxLayout(this);
    m_addButton = new QPushButton(this);
    m_delButton = new QPushButton(this);

}

void AllGradientsWidget::InsertWidgetsIntoLayouts()
{
    m_savedPreSetLayout->addWidget(m_savedPreSetLabel);
    m_savedPreSetLayout->addWidget(m_preSetComboBox);

    m_buttonsLayout->addWidget(m_addButton);
    m_buttonsLayout->addWidget(m_delButton);

    m_mainLayout->addLayout(m_savedPreSetLayout);
    m_mainLayout->addLayout(m_buttonsLayout);

    setLayout(m_mainLayout);
}

void AllGradientsWidget::FillUI()
{
    m_savedPreSetLabel->setText("Сохраненные схемы");

    OnUpdateGradient();
    m_addButton->setText("Добавить схему");
    m_delButton->setText("Удалить схему");
}

void AllGradientsWidget::InitComboBoxValues()
{
    m_preSetComboBox->clear();
    const QList<QString> gradients=m_presenter->GetAllGradientsNames();
    for (const QString &gradient: gradients)
    {
        const QVector<QColor> colors=m_presenter->GetColorVector(gradient);
        const QVector<int> ranges=m_presenter->GetRanges(gradient);
        AddGradientToComboBox(gradient, colors, ranges);
    }
    m_preSetComboBox->setIconSize(m_pixmapSize);
}

void AllGradientsWidget::AddGradientToComboBox(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges)
{
    QPixmap pixmap(m_pixmapSize);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QRect rect(0, 0, m_pixmapSize.width(), m_pixmapSize.height());
    QLinearGradient gradient(0, 0, m_pixmapSize.width(), m_pixmapSize.height());
    for (int i=0; i<ranges.count(); ++i)
    {
        gradient.setColorAt(ranges.at(i)/100.0, colors.at(i));
    }
    painter.setBrush(QBrush(gradient));
    painter.fillRect(rect, painter.brush());
    painter.end();

    m_preSetComboBox->addItem(pixmap, gradientName);
}

void AllGradientsWidget::ConnectObjects()
{
    connect(m_addButton, &QPushButton::clicked, this, &AllGradientsWidget::ToAppendGradient);
    connect(m_delButton, &QPushButton::clicked, this, &AllGradientsWidget::OnDeleteScheme);
    connect(m_preSetComboBox, QOverload<int>::of(&QComboBox::activated), this, &AllGradientsWidget::OnComboBoxItemChangedByUser);
}

void AllGradientsWidget::OnDeleteScheme()
{
    int currentIndex=m_preSetComboBox->currentIndex();
    QString currentGradientName=m_preSetComboBox->currentText();
    if (currentIndex!=-1)
    {
        m_preSetComboBox->removeItem(currentIndex);
        m_presenter->DeleteGradientByName(currentGradientName);
        if(m_preSetComboBox->count()>0)
        {
            QString gradientName=m_preSetComboBox->currentText();
            auto colors=m_presenter->GetColorVector(gradientName);
            auto ranges=m_presenter->GetRanges(gradientName);
            Q_EMIT ToChangeGradient(gradientName, colors, ranges);
        }
        else
        {
            QMessageBox::critical(nullptr, "AllGradientsWidget::OnDeleteScheme - Поле пусто", "Значений больше нет...");
        }
    }
    else
    {
        QMessageBox::critical(nullptr, "AllGradientsWidget::OnDeleteScheme - Поле пусто", "Удалять нечего...");
    }
}

void AllGradientsWidget::OnComboBoxItemChangedByUser(int index)
{
    if (index<m_preSetComboBox->count())
    {
        QString gradientName=m_preSetComboBox->itemText(index);
        auto colors=m_presenter->GetColorVector(gradientName);
        auto ranges=m_presenter->GetRanges(gradientName);
        Q_EMIT ToChangeGradient(gradientName, colors, ranges);
    }
    else
    {
        Q_UNREACHABLE();//"недопустимый индекс"
    }
}

void AllGradientsWidget::OnUpdateGradient()
{
    InitComboBoxValues();
}
