#-------------------------------------------------
#
# Project created by QtCreator 2015-04-14T22:49:24
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

TARGET = tst_lenstest
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++14

TEMPLATE = app


SOURCES += \
    tst_stm.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    person.h \
    address.h \
    common.h \
    car.h \
    account.h \
    fork.h \
    monad_syntax_experiments.h


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

# functional core targets

win32:      CONFIG(release, debug|release):    LIBS += -L$$OUT_PWD/../../cpp_functional_core/cpp_functional_core/release/ -lcpp_functional_core
else:win32: CONFIG(debug, debug|release):      LIBS += -L$$OUT_PWD/../../cpp_functional_core/cpp_functional_core/debug/   -lcpp_functional_core
else:unix:                                     LIBS += -L$$OUT_PWD/../../cpp_functional_core/cpp_functional_core/         -lcpp_functional_core

INCLUDEPATH += $$PWD/../../cpp_functional_core/cpp_functional_core
DEPENDPATH  += $$PWD/../../cpp_functional_core/cpp_functional_core

win32-g++:             CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../cpp_functional_core/cpp_functional_core/release/libcpp_functional_core.a
else:win32-g++:        CONFIG(debug, debug|release):   PRE_TARGETDEPS += $$OUT_PWD/../../cpp_functional_core/cpp_functional_core/debug/libcpp_functional_core.a
else:win32:!win32-g++: CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../cpp_functional_core/cpp_functional_core/release/cpp_functional_core.lib
else:win32:!win32-g++: CONFIG(debug, debug|release):   PRE_TARGETDEPS += $$OUT_PWD/../../cpp_functional_core/cpp_functional_core/debug/cpp_functional_core.lib
else:unix:                                             PRE_TARGETDEPS += $$OUT_PWD/../../cpp_functional_core/cpp_functional_core/libcpp_functional_core.a
