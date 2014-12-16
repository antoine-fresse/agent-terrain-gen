
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
	generator.cpp \
    agents/riveragent.cpp \
    agents/beachagent.cpp

unix{
	QMAKE_CXXFLAGS+=-fopenmp
	QMAKE_LFLAGS+=-fopenmp
}
win32{
	QMAKE_CXXFLAGS += -openmp
}


target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
QT += opengl
INSTALLS += target
CONFIG += c++11

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
	generator.h \
	agents/abstractagent.h \
    agents/riveragent.h \
    agents/beachagent.h


RESOURCES += \
	gestionnaire.qrc
