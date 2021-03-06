#-------------------------------------------------
#
# Project created by QtCreator 2021-11-03T10:13:07
#
#-------------------------------------------------

QT       += core gui widgets concurrent

VERSION = 2.5.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG += c++latest
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
    presenter/dsppresenter.cpp \
    ui/dspwidget/bottomdspcontrolpanel.cpp \
    ui/dspwidget/customplotter.cpp \
    model/spectrogramdata.cpp \
    presenter/colorschemepresenter.cpp \
    ui/colorscheme/allgradientswidget.cpp \
    ui/colorscheme/gradientInfo/gradientinfowidget.cpp \
    ui/colorscheme/gradientInfo/gradientlabel.cpp \
    ui/colorscheme/gradientInfo/rangeslider.cpp \
    ui/colorscheme/maincolorschemewidget.cpp \
    ui/colorscheme/colorChange/colorchangerwidget.cpp \
    ui/colorscheme/colorChange/allcolorswidget.cpp \
    ui/dspwidget/maindspwidget.cpp \
    ui/dspwidget/spektroPlotter/huecolormap.cpp \
    ui/dspwidget/spektroPlotter/linearcolormapindexed.cpp \
    ui/dspwidget/spektroPlotter/linearcolormaprgb.cpp \
    ui/dspwidget/spektroPlotter/pointviewer.cpp \
    ui/dspwidget/topuserpanel.cpp

HEADERS += \
    enums/colorscheme/rangeslidermarkselected.h \
    enums/colorscheme/rangesliderpointscount.h \
    enums/dsp/modekvants.h \
    presenter/dsppresenter.h \
    structs/RegFileHeader.h \
    ui/dspwidget/bottomdspcontrolpanel.h \
    ui/dspwidget/customplotter.h \
    enums/colormap.h \
    model/spectrogramdata.h \
    presenter/colorschemepresenter.h \
    structs/AmpBlockDSP.h \
    structs/POIDistSpeedPortrait.h \
    structs/POIDistSpeedPortraitHeader.h \
    structs/RawBlockDSP.h \
    structs/colorscheme/colorranges.h \
    structs/fftwinfo.h \
    structs/timeval64.h \
    ui/colorscheme/allgradientswidget.h \
    ui/colorscheme/gradientInfo/gradientinfowidget.h \
    ui/colorscheme/gradientInfo/gradientlabel.h \
    ui/colorscheme/gradientInfo/rangeslider.h \
    ui/colorscheme/maincolorschemewidget.h \
    ui/colorscheme/colorChange/colorchangerwidget.h \
    ui/colorscheme/colorChange/allcolorswidget.h \
    ui/colorscheme/basiccolors.h \
    ui/dspwidget/maindspwidget.h \
    ui/dspwidget/spektroPlotter/alphacolormap.h \
    ui/dspwidget/spektroPlotter/huecolormap.h \
    ui/dspwidget/spektroPlotter/linearcolormapindexed.h \
    ui/dspwidget/spektroPlotter/linearcolormaprgb.h \
    ui/dspwidget/spektroPlotter/pointviewer.h \
    ui/dspwidget/topuserpanel.h

unix {
    LIBS += -L/usr/lib/ -lqwt-qt5 -lqwtmathml-qt5  -lMagick++-6.Q16 -lMagickCore-6.Q16 -lMagickWand-6.Q16
    LIBS += -L/usr/lib/x86_64-linux-gnu/ -lfreetype -lfftw3
    INCLUDEPATH += /usr/include/qwt headers
    INCLUDEPATH += /usr/include/ImageMagick-6 headers #"file not found with <angled> include use quotes instead"
    INCLUDEPATH += /usr/include/x86_64-linux-gnu/ImageMagick-6 headers
    INCLUDEPATH += /usr/include/ImageMagick-6/wand headers
    INCLUDEPATH += /usr/include/ImageMagick-6/magick headers
}
