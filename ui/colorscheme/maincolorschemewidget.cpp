#include "ui/colorscheme/maincolorschemewidget.h"



MainColorSchemeWidget::MainColorSchemeWidget(QWidget *parent)
    : QWidget(parent)
    , m_margin(QMargins(10, 0, 15, 0))
{
    CreateObjects();
    CreateUI();
    InsertWidgetsIntoLayouts();
    FillUI();
    ConnectObjects();

}

MainColorSchemeWidget::~MainColorSchemeWidget()
{
    delete m_gradientsLayout;
    delete m_mainLayout;

    delete m_presenter;

    delete m_allGradientsWidget;
    delete m_gradientInfoWidget;
    delete m_colorChangeWidget;
}

void MainColorSchemeWidget::CreateObjects()
{
    m_presenter=new ColorSchemePresenter(nullptr);
}

void MainColorSchemeWidget::CreateUI()
{
    m_mainLayout=new QHBoxLayout();

    m_gradientsLayout=new QVBoxLayout();
    m_allGradientsWidget=new AllGradientsWidget(m_presenter, m_margin, this);
    m_gradientInfoWidget=new GradientInfoWidget(m_presenter, this);

    m_colorChangeWidget=new ColorChangerWidget(this);
}

void MainColorSchemeWidget::InsertWidgetsIntoLayouts()
{
    m_gradientsLayout->addWidget(m_allGradientsWidget);
    m_gradientsLayout->addWidget(m_gradientInfoWidget);

    m_mainLayout->addLayout(m_gradientsLayout);
    m_mainLayout->addWidget(m_colorChangeWidget);

    setLayout(m_mainLayout);
}

void MainColorSchemeWidget::FillUI()
{
    setWindowTitle("Настройка цветовой схемы");
}

void MainColorSchemeWidget::ConnectObjects()
{
    connect(m_allGradientsWidget, &AllGradientsWidget::ToAppendGradient, m_gradientInfoWidget, &GradientInfoWidget::OnAppendGradient);
    connect(m_allGradientsWidget, &AllGradientsWidget::ToCurrentGradientChanged, this, &MainColorSchemeWidget::OnGradientChange);
    connect(m_gradientInfoWidget, &GradientInfoWidget::ToRepaintComboBox, m_allGradientsWidget, &AllGradientsWidget::OnRepaintComboBox);
    connect(m_colorChangeWidget, &ColorChangerWidget::ToUpdateColors, this, &MainColorSchemeWidget::OnUpdateColors);
}

void MainColorSchemeWidget::OnGradientChange(const QString &gradientName)
{
    auto colors=m_presenter->GetColorVector(gradientName);
    auto ranges=m_presenter->GetRanges(gradientName);
    m_gradientInfoWidget->ChangeGradient(gradientName, colors, ranges);
    m_colorChangeWidget->ChangeColors(colors);
}

void MainColorSchemeWidget::OnUpdateColors(const QVector<QColor> &colors)
{

}
