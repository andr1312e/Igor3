#include "topuserpanel.h"

TopUserPanel::TopUserPanel(DspPresenter *presenter, QWidget *parent)
    : QWidget(parent)
    , m_dspPresenter(presenter)
{
    CreateObjects();
    CreateUI();
    InsertWidgetsIntoLayouts();
    FillUI();
    ConnectObjects();
}

TopUserPanel::~TopUserPanel()
{
    delete m_buttonsLayout;
    delete m_mainLayout;

    delete m_selectFileButton;
    delete m_openColorPanelButton;

    delete m_gradientsComboBox;

    delete m_gradientColorChangerWidget;
}

void TopUserPanel::CreateObjects()
{
    m_colorPresenter=QSharedPointer<ColorSchemePresenter>(new ColorSchemePresenter(nullptr));
}

void TopUserPanel::CreateUI()
{
    m_mainLayout= new QVBoxLayout(this);
    m_buttonsLayout=new QHBoxLayout (this);
    m_labelsLayout=new QGridLayout(this);

    m_selectFileButton=new QPushButton(this);
    m_openColorPanelButton=new QPushButton(this);
    m_saveToGifButton=new QPushButton(this);

    m_label=new QLabel(this);

    m_sensorAzmLabel=new QLabel(this);
    m_sensorAzmValue=new QLabel(this);

    m_sensorUgmLabel=new QLabel(this);
    m_sensorUgmValue=new QLabel(this);

    m_gradientsComboBox=new QComboBox(this);

    m_gradientColorChangerWidget=new GradientColorChangerWidget(m_colorPresenter, Q_NULLPTR);
}

void TopUserPanel::InsertWidgetsIntoLayouts()
{
    m_labelsLayout->addWidget(m_sensorAzmLabel, 0, 0);
    m_labelsLayout->addWidget(m_sensorAzmValue, 0, 1);
    m_labelsLayout->addWidget(m_sensorUgmLabel, 1, 0);
    m_labelsLayout->addWidget(m_sensorUgmValue, 1, 1);

    m_buttonsLayout->addWidget(m_selectFileButton);
    m_buttonsLayout->addWidget(m_label);
    m_buttonsLayout->addLayout(m_labelsLayout);
    m_buttonsLayout->addWidget(m_openColorPanelButton);
    m_buttonsLayout->addWidget(m_saveToGifButton);

    m_mainLayout->addLayout(m_buttonsLayout);
    m_mainLayout->addWidget(m_gradientsComboBox);

    setLayout(m_mainLayout);
}

void TopUserPanel::FillUI()
{
    m_selectFileButton->setText("Выберите файл");
    m_openColorPanelButton->setText("Изменить цвета градиентов");

    m_sensorAzmLabel->setText("Азимут:");
    m_sensorAzmLabel->setToolTip("Угол, отложенный от направления на север по часовой стрелке");
    m_sensorAzmValue->setText("0");

    m_sensorUgmLabel->setText("Угол места");
    m_sensorUgmLabel->setToolTip("Угловая высота наблюдаемого объекта");
    m_sensorUgmValue->setText("0");

    m_label->setText("Файл не загружен");
    m_gradientsComboBox->setEditable(false);
    RepaintComboBox();

    m_saveToGifButton->setText("Сохранить в гиф");
}

void TopUserPanel::ConnectObjects()
{
    connect(m_openColorPanelButton, &QPushButton::clicked, m_gradientColorChangerWidget, &GradientColorChangerWidget::show);
    connect(m_selectFileButton, &QPushButton::clicked, this, &TopUserPanel::OnSelectFileButtonClicked);
    connect(m_gradientColorChangerWidget, &GradientColorChangerWidget::ToUpdateGradient, this, &TopUserPanel::OnGradrientUpdate);
    connect(m_gradientsComboBox, QOverload<int>::of(&QComboBox::activated), this, &TopUserPanel::OnComboBoxUpdated);
    connect(m_saveToGifButton, &QPushButton::clicked, this, &TopUserPanel::ToSaveIntoGif);
}

void TopUserPanel::OnSelectFileButtonClicked()
{
    QString dspFilePath = QFileDialog::getOpenFileName(nullptr, "Open DSP", QApplication::applicationDirPath(), "*.bin *.dsp");
    if (dspFilePath.isEmpty())
        return;
    m_label->setText("Файл загружен:\n" + dspFilePath);
    m_dspPresenter->ReadDspFromFile(dspFilePath);
}

void TopUserPanel::OnGradrientUpdate()
{
    RepaintComboBox();
    if (m_gradientsComboBox->count()>0)
    {
        OnComboBoxUpdated(0);
    }
}

void TopUserPanel::OnComboBoxUpdated(int index)
{
    QString gradientName=m_gradientsComboBox->itemText(index);
    const ColorRanges colorRange=m_colorPresenter->GetColorRanges(gradientName);
    Q_EMIT ToChangeGradient(colorRange);
}

void TopUserPanel::SetAdditionalInfo(const float &Sensor_Azm, const float &Sensor_Ugm)
{
    m_sensorAzmValue->setText(QString::number(Sensor_Azm));
    m_sensorUgmValue->setText(QString::number(Sensor_Ugm));
}

void TopUserPanel::AddGradientToComboBox(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges)
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

    m_gradientsComboBox->addItem(pixmap, gradientName);
}

void TopUserPanel::RepaintComboBox()
{
    m_gradientsComboBox->clear();
    const QList<QString> gradients=m_colorPresenter->GetAllGradientsNames();
    for (const QString &gradient: gradients)
    {
        const QVector<QColor> colors=m_colorPresenter->GetColorVector(gradient);
        const QVector<int> ranges=m_colorPresenter->GetRanges(gradient);
        AddGradientToComboBox(gradient, colors, ranges);
    }
    m_gradientsComboBox->setIconSize(m_pixmapSize);
}
