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

SOURCES +=

HEADERS += \
    stm.h \
    tvar.h \
    bind.h \
    stm_free_stack.h \
    stm_free.h \
    free.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

#INCLUDEPATH += /usr/include/c++/7

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
