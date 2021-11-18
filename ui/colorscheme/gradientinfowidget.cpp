#include "gradientinfowidget.h"

GradientInfoWidget::GradientInfoWidget(ColorSchemePresenter *presenter, QWidget *parent)
    : QWidget(parent)
    , m_presenter(presenter)
{
    CreateUI();
    InsertWidgetsIntoLayouts();
    FillUI();
    ConnectObjects();
}

GradientInfoWidget::~GradientInfoWidget()
{
    delete m_widgetsLayout;
    delete m_mainLayout;

    delete m_mainGroupBox;
}

void GradientInfoWidget::CreateUI()
{
    m_mainLayout=new QVBoxLayout();
    m_widgetsLayout = new QVBoxLayout();

    m_mainGroupBox= new QGroupBox();

    m_colorSchemeNameLabel=new QLabel();
    m_schemeNameLineEdit=new QLineEdit();


    m_colorNumberLabel=new QLabel();
    m_numberOfPointsComboBox=new QComboBox();

    m_rangesLabel=new QLabel();
    m_rangesSlider=new RangeSlider(m_doubleHandlesVector, m_tripleHandlesVector, m_quadrupleHandlesVector, this);

    m_resultGradientLabel=new QLabel();
    m_gradientLabel=new GradientLabel(m_doubleHandlesVector, m_tripleHandlesVector, m_quadrupleHandlesVector, m_doubleHandlesColorVector, m_tripleHandlesColorVector, m_quadrupleHandlesColorVector, this);

    m_actionLabel=new QLabel();
    m_saveButton=new QPushButton();

}

void GradientInfoWidget::InsertWidgetsIntoLayouts()
{
    m_widgetsLayout->addWidget(m_colorSchemeNameLabel);
    m_widgetsLayout->addWidget(m_schemeNameLineEdit);

    m_widgetsLayout->addWidget(m_colorNumberLabel);
    m_widgetsLayout->addWidget(m_numberOfPointsComboBox);

    m_widgetsLayout->addWidget(m_rangesLabel);
    m_widgetsLayout->addWidget(m_rangesSlider);

    m_widgetsLayout->addWidget(m_resultGradientLabel);
    m_widgetsLayout->addWidget(m_gradientLabel);

    m_widgetsLayout->addWidget(m_actionLabel);
    m_widgetsLayout->addWidget(m_saveButton);

    m_mainGroupBox->setLayout(m_widgetsLayout);
    m_mainLayout->addWidget(m_mainGroupBox);
    setLayout(m_mainLayout);
}

void GradientInfoWidget::FillUI()
{
    m_mainGroupBox->setTitle("Подробная настройка выбранного спектра:");

    m_colorSchemeNameLabel->setText("Название схемы:");

    m_colorNumberLabel->setText("Количество точек:");
    m_numberOfPointsComboBox->setMaxCount(3);
    m_numberOfPointsComboBox->addItems(QStringList({"2",
                                                    "3",
                                                    "4"}));
    m_numberOfPointsComboBox->setCurrentIndex(1);
    m_numberOfPointsComboBox->setEditable(false);

    m_rangesLabel->setText("Значения точек:");

    m_resultGradientLabel->setText("Итоговый градиент:");

    m_actionLabel->setText("Нажмите на кнопку снизу, для сохранения...");
    m_saveButton->setText("Сохранить");

}

void GradientInfoWidget::ConnectObjects()
{
    connect(m_saveButton, &QPushButton::clicked, this, &GradientInfoWidget::OnSaveButtonClick);
    connect(m_numberOfPointsComboBox, QOverload<int>::of(&QComboBox::activated), this, &GradientInfoWidget::OnComboBoxPointsCountChange);
    connect(m_rangesSlider, &RangeSlider::ToRangesChange, m_gradientLabel, &GradientLabel::OnRangesChange);
}

void GradientInfoWidget::OnSaveButtonClick()
{
    QString currentName=m_schemeNameLineEdit->text();
    auto colors=m_gradientLabel->GetColors();
    auto ranges=m_gradientLabel->GetRanges();
    m_presenter-> SaveGradient(currentName, ranges, colors);
    Q_EMIT ToRepaintComboBox();
}

void GradientInfoWidget::OnComboBoxPointsCountChange(int indexComboBoxValue)
{
    switch (indexComboBoxValue) {
    case 0:
    case 1:
    case 2:
    {
        RangeSliderPointsCount pointCount=static_cast<RangeSliderPointsCount>(indexComboBoxValue+2);
        m_gradientLabel->SetDefaultRanges(pointCount);
        m_rangesSlider->SetDefaultRanges(pointCount);
        break;
    }
    default:
    {
        qFatal("Поведение не реализовано");
        break;
    }
    }
}

void GradientInfoWidget::ChangeGradient(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges)
{
    m_schemeNameLineEdit->setText(gradientName);
    m_rangesSlider->SetRangesVector(ranges);
    m_numberOfPointsComboBox->setCurrentIndex(ranges.count()-2);
    m_gradientLabel->SetRangesAndColors(colors, ranges);
}

void GradientInfoWidget::OnAppendGradient()
{
    m_schemeNameLineEdit->clear();
    m_rangesSlider->SetDefaultRanges(RangeSliderPointsCount::TripleHandles);
    m_gradientLabel->SetDefaultRanges(RangeSliderPointsCount::TripleHandles);
}
