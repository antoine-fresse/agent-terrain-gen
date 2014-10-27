
SOURCES += \
    main.cpp \
    heightmap.cpp \
    camera.cpp \
    gamewidget.cpp \
    generatorwindow.cpp \
    agentwidget.cpp \
    agents/coastlineagent.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
QT += opengl
INSTALLS += target
QMAKE_CXXFLAGS += -std=c++11

HEADERS += \
    heightmap.h \
    camera.h \
    gamewidget.h \
    generatorwindow.h \
    agents/iagent.h \
    agentwidget.h \
    agents/coastlineagent.h


RESOURCES += \
    gestionnaire.qrc
