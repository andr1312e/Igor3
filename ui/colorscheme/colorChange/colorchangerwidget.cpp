#include "colorchangerwidget.h"

#include <QPushButton>

ColorChangerWidget::ColorChangerWidget(QWidget *parent)
    : QWidget(parent)
{
    CreateObjects();
    CreateUI();
    InsertWidgetsIntoLayouts();
    FillUI();
    ConnectObjects();
}

ColorChangerWidget::~ColorChangerWidget()
{

}

void ColorChangerWidget::CreateObjects()
{

}

void ColorChangerWidget::CreateUI()
{
    m_mainLayout=new QVBoxLayout(this);

    m_groupBox=new QGroupBox(this);

    m_groupBoxLayout=new QVBoxLayout(this);
    m_allColorListWidget=new AllColorsWidget(this);

    m_colorDialog=new ColorDialog(this, Qt::Widget | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

}

void ColorChangerWidget::InsertWidgetsIntoLayouts()
{
    m_groupBoxLayout->addWidget(m_allColorListWidget);

    m_groupBoxLayout->addWidget(m_colorDialog);

    m_groupBox->setLayout(m_groupBoxLayout);
    m_mainLayout->addWidget(m_groupBox);
    setLayout(m_mainLayout);

}

void ColorChangerWidget::FillUI()
{
    m_groupBox->setTitle("Выбор цвета");

}

void ColorChangerWidget::ConnectObjects()
{
    connect(m_colorDialog, &ColorDialog::colorChanged, m_allColorListWidget, &AllColorsWidget::OnSetColorToButton);
    connect(m_colorDialog, &ColorDialog::colorChanged, this, &ColorChangerWidget::OnColorChange);
}

void ColorChangerWidget::OnColorChange(const QColor &color)
{
    int index=m_allColorListWidget->GetCurrentIndex();
    if(index == -1)
    {

    }
    else
    {
        const QVector<QColor> colors=m_allColorListWidget->GetColors();
        Q_EMIT ToUpdateColors(colors);
    }
}

void ColorChangerWidget::OnChangeGradient(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges)
{
    Q_UNUSED(gradientName)
    Q_UNUSED(ranges)
    m_allColorListWidget->SetColors(colors);
}

void ColorChangerWidget::OnColorsCountChanged(const int colorCounts)
{
    switch (colorCounts) {
    case 2:
        m_allColorListWidget->SetColors(doubleHandlesColorVector);
        break;
    case 3:
        m_allColorListWidget->SetColors(tripleHandlesColorVector);
        break;
    case 4:
        m_allColorListWidget->SetColors(quadrupleHandlesColorVector);
        break;
    default:
        Q_UNREACHABLE();
    }
}

void ColorChangerWidget::OnAppendGradient()
{
    m_allColorListWidget->SetColors(tripleHandlesColorVector);
}
