#ifndef USERCONTROLWIDGET_H
#define USERCONTROLWIDGET_H

#include <QHBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSharedDataPointer>

#include "structs/colorscheme/colorranges.h"

#include "presenter/dsppresenter.h"
#include "presenter/colorscheme/colorschemepresenter.h"

#include "ui/dspwidget/dsphistorywidget.h"
#include "ui/colorscheme/maincolorschemewidget.h"

class TopUserPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TopUserPanel(DspPresenter *presenter, QWidget *parent);
    ~TopUserPanel();
private:
    void CreateObjects();
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void RepaintComboBox();
    void ConnectObjects();
Q_SIGNALS:
    void ToChangeGradient(const ColorRanges &range);
    void ToShowDspHistoryWidget();
private Q_SLOTS:
    void OnSelectFileButtonClicked();
    void OnGradrientUpdate();
    void OnComboBoxUpdated(int index);
private:
    void AddGradientToComboBox(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges);
private:
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonsLayout;

    QPushButton *m_selectFileButton;
    QPushButton *m_openHistoryButton;
    QPushButton *m_openColorPanelButton;

    QLabel *m_label;
    QComboBox *m_gradientsComboBox;

    GradientColorChangerWidget *m_gradientColorChangerWidget;

private:
    DspPresenter *m_sdpPresenter;
    QSharedPointer<ColorSchemePresenter> m_colorPresenter;
    const QSize m_pixmapSize=QSize(100,14);
};

#endif // USERCONTROLWIDGET_H
