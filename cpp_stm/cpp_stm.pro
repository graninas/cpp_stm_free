#-------------------------------------------------
#
# Project created by QtCreator 2014-12-18T22:56:58
#
#-------------------------------------------------

QT -= core gui

TARGET = cpp_stm
TEMPLATE = lib
CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

SOURCES +=

HEADERS += \
    stm.h \
    tvar.h \
    bind.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
