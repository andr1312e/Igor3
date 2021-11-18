#include "mainwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w(Q_NULLPTR);
    w.show();

    return a.exec();
}
