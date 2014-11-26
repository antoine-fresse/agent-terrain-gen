#include "generatorwindow.h"

#include "gamewidget.h"
#include "agentwidget.h"

#include <QToolBar>
#include <QPushButton>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <ctime>

#include "agents/coastlineagent.h"
#include "agents/smoothagent.h"
#include "agents/mountainagent.h"

GeneratorWindow::GeneratorWindow(QWidget *parent) :
    QMainWindow(parent), m_gameWidget{nullptr}
{
    srand(time(nullptr));
    try {
        m_generator.load("conf.txt");
    } catch (...) {
        m_generator.addAgent(0, new CoastLineAgent());
        m_generator.addAgent(1, new SmoothAgent());
        m_generator.addAgent(1, new MountainAgent());
        // AJOUTER AGENTS ICI
    }

    setWindowTitle("Generateur");
    createView();
    createLayout();

    m_generator.setHeightMap(m_gameWidget->getHeightMap());
    m_generator.setOnFinish([this]() {
        m_animationTimer.stop();
        m_runButton->setText("Run");
        m_stepButton->setDisabled(false);
    });

    m_animationTimer.setInterval(10);
    m_generator.connect(&m_animationTimer, &QTimer::timeout, &m_generator, &Generator::tick);
}

GeneratorWindow::~GeneratorWindow()
{
    m_generator.save("conf.txt");
}

void GeneratorWindow::keyPressEvent(QKeyEvent* event)
{
    m_gameWidget->keyPressEvent(event);
}

void GeneratorWindow::keyReleaseEvent(QKeyEvent* event)
{
    m_gameWidget->keyReleaseEvent(event);
}

void GeneratorWindow::createView()
{
    QGLFormat format;
    format.setVersion(3, 3);
    format.setProfile(QGLFormat::CoreProfile);

    m_gameWidget = new GameWidget(60);
    m_gameWidget->setFormat(format);

    m_runButton = new QPushButton("Run");
    m_stepButton = new QPushButton("Tick");
    m_InstantButton = new QPushButton("Instant");
    m_resetButton = new QPushButton("Reset");
    connect(m_runButton, &QPushButton::clicked, [this]() {
        if (!m_animationTimer.isActive()) {
            m_stepButton->setDisabled(true);
            m_runButton->setText("Stop");
            m_animationTimer.start();
        } else {
            m_animationTimer.stop();
            m_runButton->setText("Run");
            m_stepButton->setDisabled(false);
        }
    });
    connect(m_stepButton, &QPushButton::clicked, [this]() {
        m_generator.tick();
    });
    connect(m_InstantButton, &QPushButton::clicked, [this]() {
        m_generator.runAll();
    });
    connect(m_resetButton, &QPushButton::clicked, [this]() {
        m_generator.reset();

        m_animationTimer.stop();
        m_runButton->setText("Run");
        m_stepButton->setDisabled(false);
    });
}

void GeneratorWindow::createLayout()
{
    // AJOUT DU WIDGET OPENGL
    setCentralWidget(m_gameWidget);

    // AJOUT DES TOOLBARS
    QToolBar* agentsToolbar = new QToolBar(); {
        for (int i = 0; i < 3; ++i) {
            for (auto& agent : m_generator.getAgents(i)) {
                AgentWidget* widget = new AgentWidget(agent);
                agentsToolbar->addWidget(widget);
            }
        }
    }
    addToolBar(Qt::RightToolBarArea, agentsToolbar);

    agentsToolbar = new QToolBar(); {
        agentsToolbar->addWidget(m_runButton);
        agentsToolbar->addWidget(m_stepButton);
        agentsToolbar->addWidget(m_InstantButton);
        agentsToolbar->addWidget(m_resetButton);
    }
    addToolBar(Qt::TopToolBarArea, agentsToolbar);

    // AJOUT DE LA BARRE DE MENU
    QMenuBar* menuBar = new QMenuBar(); {
        QMenu* fileMenu = new QMenu("Fichier"); {
            QAction* openAction = fileMenu->addAction("Ouvrir la configuration");
            QAction* saveAction = fileMenu->addAction("Enregistrer la configuration");
            QAction* exportAction = fileMenu->addAction("Exporter le resultat");
            connect(exportAction, &QAction::triggered, this, &GeneratorWindow::saveHeightmap);
        }
        menuBar->addMenu(fileMenu);
    }
    setMenuBar(menuBar);
}

void GeneratorWindow::saveHeightmap()
{
    // ENREGISTRE LA HEIGHTMAP
    // TODO
}
