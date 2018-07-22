#-------------------------------------------------
#
# Project created by QtCreator 2014-12-18T22:56:58
#
#-------------------------------------------------

QT -= core gui

TARGET = cpp_stm
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++1z
QMAKE_CXXFLAGS += -nostdinc++

INCLUDEPATH += /usr/include/x86_64-linux-gnu/c++/7
INCLUDEPATH += /usr/include/c++/7

SOURCES += \
    context.cpp \
    stupid_guid.cpp

HEADERS += \
    tvar.h \
    stm_interpreter.h \
    context.h \
    stupid_guid.h \
    stm_bind.h \
    stm.h \
    stm_runtime.h \
    unit.h \
    id.h \
    stm_optional.h \
    stm_free_church.h \
    stm_free.h \
    stm_free_functor.h \
    stm_church.h \
    stm_church_functor.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
