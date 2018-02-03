# stm targets

win32:       CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/cpp_stm/cpp_stm/release/ -lcpp_stm
else:win32:  CONFIG(debug, debug|release):   LIBS += -L$$OUT_PWD/../../lib/cpp_stm/cpp_stm/debug/   -lcpp_stm
else:unix:                                   LIBS += -L$$OUT_PWD/../../lib/cpp_stm/cpp_stm/         -lcpp_stm

INCLUDEPATH += $$PWD/../../lib/cpp_stm/cpp_stm
DEPENDPATH  += $$PWD/../../lib/cpp_stm/cpp_stm

win32-g++:             CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cpp_stm/cpp_stm/release/libcpp_stm.a
else:win32-g++:        CONFIG(debug, debug|release):   PRE_TARGETDEPS += $$OUT_PWD/../../lib/cpp_stm/cpp_stm/debug/libcpp_stm.a
else:win32:!win32-g++: CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cpp_stm/cpp_stm/release/cpp_stm.lib
else:win32:!win32-g++: CONFIG(debug, debug|release):   PRE_TARGETDEPS += $$OUT_PWD/../../lib/cpp_stm/cpp_stm/debug/cpp_stm.lib
else:unix:                                             PRE_TARGETDEPS += $$OUT_PWD/../../lib/cpp_stm/cpp_stm/libcpp_stm.a
