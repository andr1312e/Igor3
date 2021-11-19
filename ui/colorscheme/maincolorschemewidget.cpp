#include "ui/colorscheme/maincolorschemewidget.h"



GradientColorChangerWidget::GradientColorChangerWidget(QSharedPointer<ColorSchemePresenter> presenter, QWidget *parent)
    : QWidget(parent)
    , m_margin(QMargins(10, 0, 15, 0))
    , m_presenter(presenter)
{
    CreateObjects();
    CreateUI();
    InsertWidgetsIntoLayouts();
    FillUI();
    ConnectObjects();
}

GradientColorChangerWidget::~GradientColorChangerWidget()
{
    delete m_gradientsLayout;
    delete m_mainLayout;

    delete m_allGradientsWidget;
    delete m_gradientInfoWidget;
    delete m_colorChangeWidget;
}

void GradientColorChangerWidget::CreateObjects()
{

}

void GradientColorChangerWidget::CreateUI()
{
    m_mainLayout=new QHBoxLayout();

    m_gradientsLayout=new QVBoxLayout();
    m_allGradientsWidget=new AllGradientsWidget(m_presenter, m_margin, this);
    m_gradientInfoWidget=new GradientInfoWidget(m_presenter, this);

    m_colorChangeWidget=new ColorChangerWidget(this);
}

void GradientColorChangerWidget::InsertWidgetsIntoLayouts()
{
    m_gradientsLayout->addWidget(m_allGradientsWidget);
    m_gradientsLayout->addWidget(m_gradientInfoWidget);

    m_mainLayout->addLayout(m_gradientsLayout);
    m_mainLayout->addWidget(m_colorChangeWidget);

    setLayout(m_mainLayout);
}

void GradientColorChangerWidget::FillUI()
{
    setWindowTitle("Настройка цветовой схемы");
    auto colorList=m_presenter->GetAllGradientsNames();
    if(!colorList.isEmpty())
    {
        QString gradientName=colorList.front();
        auto colors=m_presenter->GetColorVector(gradientName);
        auto ranges=m_presenter->GetRanges(gradientName);
        m_gradientInfoWidget->OnChangeGradient(gradientName, colors, ranges);
        m_colorChangeWidget->OnChangeGradient(gradientName, colors, ranges);
    }
}

void GradientColorChangerWidget::ConnectObjects()
{
    connect(m_allGradientsWidget, &AllGradientsWidget::ToAppendGradient, m_gradientInfoWidget, &GradientInfoWidget::OnAppendGradient);
    connect(m_allGradientsWidget, &AllGradientsWidget::ToAppendGradient, m_colorChangeWidget, &ColorChangerWidget::OnAppendGradient);
    connect(m_allGradientsWidget, &AllGradientsWidget::ToChangeGradient, m_gradientInfoWidget, &GradientInfoWidget::OnChangeGradient);
    connect(m_allGradientsWidget, &AllGradientsWidget::ToChangeGradient, m_colorChangeWidget, &ColorChangerWidget::OnChangeGradient);

    connect(m_gradientInfoWidget, &GradientInfoWidget::ToUpdateGradient, m_allGradientsWidget, &AllGradientsWidget::OnUpdateGradient);
    connect(m_gradientInfoWidget, &GradientInfoWidget::ToUpdateGradient, this, &GradientColorChangerWidget::ToUpdateGradient);
    connect(m_gradientInfoWidget, &GradientInfoWidget::ToColorsCountChanged, m_colorChangeWidget, &ColorChangerWidget::OnColorsCountChanged);
    connect(m_colorChangeWidget, &ColorChangerWidget::ToUpdateColors, m_gradientInfoWidget, &GradientInfoWidget::OnUpdateColors);
}
