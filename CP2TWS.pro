QT       += core gui
QT       += serialport
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += static
CONFIG += c++11
TARGET = CP2TWS
RC_ICONS = CP2TWS_white.ico
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    comdialog.cpp \
    helpdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    ueberdialog.cpp \
    veranstaltungsdialog.cpp \
    worker.cpp \
    zeitdialog.cpp

HEADERS += \
    comdialog.h \
    helpdialog.h \
    mainwindow.h \
    ueberdialog.h \
    veranstaltungsdialog.h \
    worker.h \
    zeitdialog.h

FORMS += \
    comdialog.ui \
    helpdialog.ui \
    mainwindow.ui \
    ueberdialog.ui \
    veranstaltungsdialog.ui \
    zeitdialog.ui

# Default rules for deployment.
QMAKE_CXXFLAGS  += -s
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
