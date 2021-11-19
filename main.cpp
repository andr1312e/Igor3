#include "ui/dspwidget/maindspwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainDspWidget mainDspWidget(Q_NULLPTR);
    mainDspWidget.show();

    return a.exec();
}
