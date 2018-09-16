#-------------------------------------------------
#
# Project created by QtCreator 2018-09-11T23:12:08
#
#-------------------------------------------------

QT       += core gui widgets svg

TARGET = cmmk_ctrl
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

CONFIG += c++14 debug

isEmpty(PREFIX) {
  PREFIX = /usr/local
}

DATADIR = $$PREFIX/share
BINDIR = $$PREFIX/bin
LIBDIR = $$PREFIX/lib

DEFINES += DATADIR=\\\"$$DATADIR\\\"

SOURCES += \
        main.cc \
        mainwindow.cc \
        widgets/KeyboardWidget.cc \
        widgets/KeyboardButton.cc \
        widgets/SpeedSlider.cc \
        widgets/ColorButton.cc

HEADERS += \
        mainwindow.hh \
        widgets/KeyboardWidget.hh \
        widgets/KeyboardButton.hh \
        widgets/SpeedSlider.hh \
        widgets/ColorButton.hh

FORMS += \
        mainwindow.ui

LIBS += \
        -lusb-1.0 \
        -L../libcmmk/out \
        -lcmmk

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = $$BINDIR
!isEmpty(target.path): INSTALLS += target

# Copy assets and library
INSTALLS += assets deps

assets.path = $$DATADIR/cmmk_ctrl/img
assets.files = img/*.svg

deps.path = $$LIBDIR
deps.files = libcmmk/out/libcmmk.so
