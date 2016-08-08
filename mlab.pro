#-------------------------------------------------
#
# Project created by QtCreator 2015-12-12T00:21:34
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mlab
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mlabwindow.cpp \
    mdihandler.cpp \
    bopmgwindow.cpp \
    windowadderwindow.cpp \
    powermetertestwindow.cpp \
    pairsavewindow.cpp \
    labport.cpp \
    bopmgport.cpp \
    simplegraphwindow.cpp \
    simplegraphwidget.cpp \
    bopmguicharwindow.cpp \
    temperaturecalcwindow.cpp \
    tsh071port.cpp \
    tsh071window.cpp \
    allsavewindow.cpp \
    elflowport.cpp \
    elflowprotocol.cpp \
    byteconverter.cpp \
    elflowwindow.cpp \
    thermocoupleport.cpp \
    thermocouplewindow.cpp \
    scratchpadwindow.cpp \
    screenshotwindow.cpp \
    generalportwindow.cpp \
    eapsuta12port.cpp \
    eaps3000window.cpp \
    eaps8000usbport.cpp \
    eaps8000usbwindow.cpp \
    eaps8000usbuicharwindow.cpp

HEADERS  += mainwindow.h \
    mlabwindow.h \
    mdihandler.h \
    bopmgwindow.h \
    windowadderwindow.h \
    powermetertestwindow.h \
    pairsavewindow.h \
    labport.h \
    easylogging++.h \
    bopmgport.h \
    simplegraphwindow.h \
    simplegraphwidget.h \
    bopmguicharwindow.h \
    temperaturecalcwindow.h \
    tsh071port.h \
    tsh071window.h \
    allsavewindow.h \
    elflowport.h \
    elflowprotocol.h \
    byteconverter.h \
    elflowwindow.h \
    thermocoupleport.h \
    thermocouplewindow.h \
    scratchpadwindow.h \
    screenshotwindow.h \
    generalportwindow.h \
    eapsuta12port.h \
    eaps3000window.h \
    eaps8000usbport.h \
    eaps8000usbwindow.h \
    eaps8000usbuicharwindow.h

FORMS    += mainwindow.ui \
    bopmgwindow.ui \
    windowadderwindow.ui \
    powermetertestwindow.ui \
    pairsavewindow.ui \
    simplegraphwindow.ui \
    bopmguicharwindow.ui \
    temperaturecalcwindow.ui \
    tsh071window.ui \
    allsavewindow.ui \
    elflowwindow.ui \
    thermocouplewindow.ui \
    scratchpadwindow.ui \
    screenshotwindow.ui \
    generalportwindow.ui \
    eaps3000window.ui \
    eaps8000usbwindow.ui \
    eaps8000usbuicharwindow.ui

CONFIG   += c++11

RESOURCES += \
    resources.qrc
