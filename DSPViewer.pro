#-------------------------------------------------
#
# Project created by QtCreator 2021-11-03T10:13:07
#
#-------------------------------------------------

QT       += core gui widgets

CONFIG += c++latest
TARGET = DSPViewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include($$PWD/colorCircles/color_widgets.pri)

SOURCES += \
        main.cpp \
    circlepalette.cpp \
    customplotter.cpp \
    mainwidget.cpp \
    model/colorpoint.cpp \
    model/spectrogramdata.cpp \
    presenter/colorscheme/colorschemepresenter.cpp \
    ui/colorscheme/allgradientswidget.cpp \
    ui/colorscheme/gradientinfowidget.cpp \
    ui/colorscheme/maincolorschemewidget.cpp \
    ui/dspsettingwidget.cpp \
    ui/colorscheme/gradientlabel.cpp \
    ui/colorscheme/rangeslider.cpp \
    ui/colorscheme/colorChange/colorchangerwidget.cpp \
    ui/colorscheme/colorChange/allcolorswidget.cpp

HEADERS += \
    circlepalette.h \
    colorchemepresenter.h \
    customplotter.h \
    enums/SliderHandleSelected.h \
    enums/colormap.h \
    enums/prestoPalette.h \
    enums/rangesliderpointscount.h \
    mainwidget.h \
    model/colorpoint.h \
    model/spectrogramdata.h \
    presenter/colorscheme/colorschemepresenter.h \
    structs/AmpBlockDSP.h \
    structs/POIDistSpeedPortrait.h \
    structs/POIDistSpeedPortraitHeader.h \
    structs/RawBlockDSP.h \
    structs/colorranges.h \
    structs/fftwinfo.h \
    structs/timeval64.h \
    ui/colorscheme/allgradientswidget.h \
    ui/colorscheme/gradientinfowidget.h \
    ui/colorscheme/maincolorschemewidget.h \
    ui/dspsettingwidget.h \
    ui/colorscheme/gradientlabel.h \
    ui/colorscheme/rangeslider.h \
    ui/colorscheme/colorChange/colorchangerwidget.h \
    ui/colorscheme/colorChange/allcolorswidget.h

unix {
    LIBS += -L/usr/lib/ -lqwt-qt5 -lqwtmathml-qt5
    LIBS += -L/usr/lib/x86_64-linux-gnu/ -lfreetype -lfftw3
    INCLUDEPATH += /usr/include/qwt headers
}
