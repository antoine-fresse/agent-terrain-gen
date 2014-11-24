
SOURCES += \
    main.cpp \
    heightmap.cpp \
    camera.cpp \
    gamewidget.cpp \
    generatorwindow.cpp \
    agentwidget.cpp \
    agents/coastlineagent.cpp \
    agents/mountainagent.cpp \
    agents/smoothagent.cpp \
    noise/noise.cpp \
    noise/octave.cpp \
    generator.cpp

QMAKE_CXXFLAGS+=-fopenmp
QMAKE_LFLAGS+=-fopenmp

QMAKE_CXXFLAGS+=-openmp
QMAKE_LFLAGS+=-openmp
    
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
    agents/coastlineagent.h \
    agents/mountainagent.h \
    agents/smoothagent.h \
    noise/noise.h \
    noise/octave.h \
    noise/random.h \
    generator.h


RESOURCES += \
    gestionnaire.qrc
