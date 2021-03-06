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

SOURCES += \
    stm/context.cpp

HEADERS += \
    stm/tvar.h \
    stm/context.h \
    stm/stmf/functor.h \
    stm/stmf/stmf.h \
    stm/impl/runtime.h \
    stm/types.h \
    stm/free/stml.h \
    stm/free/bind.h \
    stm/free/interpreter.h \
    stm/church/stml.h \
    stm/church/interpreter.h \
    stm/church/functor.h \
    stm/free/stm.h \
    stm/church/stm.h \
    stm/stm.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES +=
