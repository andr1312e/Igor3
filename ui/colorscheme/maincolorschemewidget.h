#ifndef COLORSCHEMEWIDGET_H
#define COLORSCHEMEWIDGET_H

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


#include "presenter/colorscheme/colorschemepresenter.h"

#include "ui/colorscheme/gradientinfowidget.h"
#include "ui/colorscheme/allgradientswidget.h"
#include "ui/colorscheme/colorChange/colorchangerwidget.h"

class MainColorSchemeWidget : public QWidget {
    Q_OBJECT

public:
    explicit MainColorSchemeWidget(QWidget *parent);
    ~MainColorSchemeWidget();
private:
    void CreateObjects();
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void ConnectObjects();
private Q_SLOTS:
    void OnGradientChange(const QString &gradientName);
private:

    const QMargins m_margin;
    ColorSchemePresenter *m_presenter;

    QHBoxLayout *m_mainLayout;

    QVBoxLayout *m_gradientsLayout;
    AllGradientsWidget *m_allGradientsWidget;
    GradientInfoWidget *m_gradientInfoWidget;

    ColorChangerWidget *m_colorChangeWidget;
};
#endif // COLORSCHEMEWIDGET_H
