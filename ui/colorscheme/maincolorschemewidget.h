#ifndef UI_COLORSSCHEME_COLORSCHEMEWIDGET_H
#define UI_COLORSSCHEME_COLORSCHEMEWIDGET_H

#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QProgressBar>
#include <QWidget>
#include <QDebug>
#include <QJsonValueRef>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QStandardItemModel>
#include <QTimer>
#include <QJsonArray>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QApplication>
#include <QLineEdit>
#include <QMessageBox>


#include "presenter/colorschemepresenter.h"

#include "ui/colorscheme/gradientInfo/gradientinfowidget.h"
#include "ui/colorscheme/allgradientswidget.h"
#include "ui/colorscheme/colorChange/colorchangerwidget.h"

class GradientColorChangerWidget : public QWidget {
    Q_OBJECT

public:
    explicit GradientColorChangerWidget(QSharedPointer<ColorSchemePresenter> &presenter, QWidget *parent);
    ~GradientColorChangerWidget();
private:
    void CreateObjects();
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void ConnectObjects();
Q_SIGNALS:
    void ToUpdateGradient();
private:


    QSharedPointer<ColorSchemePresenter> m_presenter;

    QHBoxLayout *m_mainLayout;

    QVBoxLayout *m_gradientsLayout;
    AllGradientsWidget *m_allGradientsWidget;
    GradientInfoWidget *m_gradientInfoWidget;

    ColorChangerWidget *m_colorChangeWidget;
};
#endif // UI_COLORSSCHEME_COLORSCHEMEWIDGET_H
