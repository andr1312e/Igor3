#include "allcolorswidget.h"

AllColorsWidget::AllColorsWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentIdChecked(-1)
{
    CreateUI();
    InsertWidgetsIntoLayouts();
    FillUI();
    ConnectObjects();
}

AllColorsWidget::~AllColorsWidget()
{
    delete m_colorsLayout;
    delete m_mainLayout;

    qDeleteAll(m_buttonColorsList);
    delete m_mainGroupBox;
}

void AllColorsWidget::CreateUI()
{
    m_mainLayout=new QVBoxLayout(this);

    m_mainGroupBox=new QGroupBox(this);

    m_colorsLayout=new QHBoxLayout(this);
    for (int i=0; i<4; i++)
    {
        m_buttonColorsList.push_back(new ColorPreview(this, i));
    }
}

void AllColorsWidget::InsertWidgetsIntoLayouts()
{
    for (auto button:m_buttonColorsList)
    {
        m_colorsLayout->addWidget(button);
    }

   m_mainGroupBox->setLayout(m_colorsLayout);

    m_mainLayout->addWidget(m_mainGroupBox);

    setLayout(m_mainLayout);
}

void AllColorsWidget::FillUI()
{
    m_mainGroupBox->setTitle("Выберите цвет из доступных ниже:");
}

void AllColorsWidget::ConnectObjects()
{
    for (ColorPreview* button:m_buttonColorsList)
    {
        connect(button, &ColorPreview::ToColorButtonClicked, this, &AllColorsWidget::OnColorButtonClicked);
    }
}

void AllColorsWidget::OnColorButtonClicked(const int buttonId)
{
    SetButtonChecked(buttonId);
    m_currentIdChecked=buttonId;
}

void AllColorsWidget::OnSetColorToButton(const QColor &color)
{
    if(m_currentIdChecked==-1)
    {
        return;
    }
    else
    {
        m_buttonColorsList.at(m_currentIdChecked)->setColor(color);
    }
}

void AllColorsWidget::SetColors(const QVector<QColor> &colors)
{
    int colorsCount=colors.count();
    for (int i=0; i<colorsCount; i++)
    {
        m_buttonColorsList[i]->setColor(colors.at(i));
    }
    SetToDefault(colorsCount);
}

const QVector<QColor> AllColorsWidget::GetColors()
{
    QVector<QColor> colors;
    for (ColorPreview* button :m_buttonColorsList)
    {
        if(button->isEnabled())
        {
           colors.append(button->color());
        }
        else
        {
            break;
        }
    }
    return colors;
}

int AllColorsWidget::GetCurrentIndex()
{
    return m_currentIdChecked;
}

void AllColorsWidget::SetToDefault(int count)
{
    SetAllButtonUnchecked();
    SetButtonsEnabled(count);
}

void AllColorsWidget::SetButtonsEnabled(int count)
{
    for (int i=0; i<count; ++i)
    {
        m_buttonColorsList.at(i)->setEnabled(true);
    }
    for (int i=count; i<m_buttonColorsList.count(); ++i)
    {
        m_buttonColorsList.at(i)->setColor(Qt::gray);
        m_buttonColorsList.at(i)->setDisabled(true);
    }
}

void AllColorsWidget::SetButtonChecked(const int buttonId)
{
    SetAllButtonUnchecked();
    m_buttonColorsList.at(buttonId)->SetChecked(true);
}

void AllColorsWidget::SetAllButtonUnchecked()
{
    for (ColorPreview* button:m_buttonColorsList)
    {
        button->SetChecked(false);
    }
    m_currentIdChecked=-1;
}
