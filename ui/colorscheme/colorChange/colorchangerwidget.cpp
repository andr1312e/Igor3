#include "colorchangerwidget.h"

#include <QPushButton>

ColorChangerWidget::ColorChangerWidget(QWidget *parent)
    : QWidget(parent)
    , m_maxColorCount(4)
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
    m_mainLayout=new QVBoxLayout();

    m_groupBox=new QGroupBox();

    m_groupBoxLayout=new QVBoxLayout();
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
        m_colors.at(index)=color;
        Q_EMIT ToUpdateColors(m_colors);
    }
}

void ColorChangerWidget::ChangeColors(const QVector<QColor> &colors)
{
    m_allColorListWidget->ChangeColors(colors);
}
