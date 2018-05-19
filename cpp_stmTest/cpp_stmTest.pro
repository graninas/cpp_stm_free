#-------------------------------------------------
#
# Project created by QtCreator 2015-04-14T22:49:24
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

TARGET = tst_lenstest
CONFIG += console
CONFIG -= app_bundle

CONFIG += c++1z
QMAKE_CXXFLAGS += -nostdinc++

INCLUDEPATH += /usr/include/x86_64-linux-gnu/c++/7
INCLUDEPATH += /usr/include/c++/7

TEMPLATE = app


SOURCES += \
    tst_stm.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    common.h

# stm targets

win32:      CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../cpp_stm/release/ -lcpp_stm
else:win32: CONFIG(debug, debug|release):   LIBS += -L$$OUT_PWD/../cpp_stm/debug/   -lcpp_stm
else:unix:                                  LIBS += -L$$OUT_PWD/../cpp_stm/         -lcpp_stm

INCLUDEPATH += $$PWD/../cpp_stm
DEPENDPATH  += $$PWD/../cpp_stm

win32-g++:              CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../cpp_stm/release/libcpp_stm.a
else:win32-g++:         CONFIG(debug, debug|release):   PRE_TARGETDEPS += $$OUT_PWD/../cpp_stm/debug/libcpp_stm.a
else:win32:!win32-g++:  CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../cpp_stm/release/cpp_stm.lib
else:win32:!win32-g++:  CONFIG(debug, debug|release):   PRE_TARGETDEPS += $$OUT_PWD/../cpp_stm/debug/cpp_stm.lib
else:unix:                                              PRE_TARGETDEPS += $$OUT_PWD/../cpp_stm/libcpp_stm.a
