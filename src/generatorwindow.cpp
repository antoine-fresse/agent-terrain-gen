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

GeneratorWindow::GeneratorWindow(QWidget *parent) :
    QMainWindow(parent), m_gameWidget{nullptr}, m_isRunning{false}, m_coastStepOver{false}, m_hasStarted{false}
{
    srand(time(nullptr));
    m_stageAgents.push_back(new CoastLineAgent()); // AJOUTER AGENTS ICI

    setWindowTitle("Generateur");
    createView();
    createLayout();
    m_animationTimer.setInterval(10);
    connect(&m_animationTimer, &QTimer::timeout, this, &GeneratorWindow::runAll);
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
    m_resetButton = new QPushButton("Reset");
    connect(m_runButton, &QPushButton::clicked, [this]() {
        if (!m_isRunning) {
            if (!m_hasStarted) {
                populateFirstStep();
                m_hasStarted = true;
            }
            m_isRunning = true;
            m_stepButton->setDisabled(true);
            m_runButton->setText("Stop");
            m_animationTimer.start();
        } else {
            m_isRunning = false;
            m_animationTimer.stop();
            m_runButton->setText("Run");
            m_stepButton->setDisabled(false);
        }
    });
    connect(m_stepButton, &QPushButton::clicked, [this]() {
        if (!m_hasStarted) {
            populateFirstStep();
            m_hasStarted = true;
        }
        runAll();
    });
    connect(m_resetButton, &QPushButton::clicked, [this]() {
        m_hasStarted = false;
        m_coastStepOver = false;
        m_isRunning = false;
        m_animationTimer.stop();
        m_runButton->setText("Run");
        m_stepButton->setDisabled(false);
        m_agents.clear();
        m_gameWidget->getHeightMap()->reset();
    });
}

void GeneratorWindow::createLayout()
{
    // AJOUT DU WIDGET OPENGL
    setCentralWidget(m_gameWidget);

    // AJOUT DES TOOLBARS
    QToolBar* agentsToolbar = new QToolBar(); {
        for (auto& agent : m_stageAgents) {
            AgentWidget* widget = new AgentWidget(agent);
            agentsToolbar->addWidget(widget);
        }
    }
    addToolBar(Qt::RightToolBarArea, agentsToolbar);

    agentsToolbar = new QToolBar(); {
        agentsToolbar->addWidget(m_runButton);
        agentsToolbar->addWidget(m_stepButton);
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

void GeneratorWindow::runAll()
{
    if ((m_agents.size() == 0) && (!m_coastStepOver)) {
        m_coastStepOver = true;
        populateSecondStep();
    } else if ((m_agents.size() == 0) && (m_coastStepOver)) {
        m_coastStepOver = false;
        m_isRunning = false;
        m_animationTimer.stop();
        m_runButton->setText("Run");
        m_stepButton->setDisabled(false);
    }
    auto it = m_agents.begin();
    while (it != m_agents.end()) {
        (*it)->run();
        if ((*it)->isDead()) {
            it = m_agents.erase(it);
        } else {
            ++it;
        }
    }
}

void GeneratorWindow::saveHeightmap()
{
    // ENREGISTRE LA HEIGHTMAP
    // TODO
}

void GeneratorWindow::populateFirstStep()
{
    m_agents.clear();
    m_coastStepOver = false;

    if (m_stageAgents.size() > 0) {
        auto& templateAgent = m_stageAgents[0];
        m_agents.push_back(templateAgent->copy());
        m_agents.back()->spawn(m_gameWidget->getHeightMap());
    }
}

void GeneratorWindow::populateSecondStep()
{
    if (m_coastStepOver && (m_stageAgents.size() > 1)) {
        m_agents.clear();
        for (unsigned int i = 1; i < m_stageAgents.size(); ++i) {
            auto& templateAgent = m_stageAgents[i];
            int count = templateAgent->getValue("count");
            for (int j = 0; j < count; ++j) {
                m_agents.push_back(templateAgent->copy());
                m_agents.back()->spawn(m_gameWidget->getHeightMap());
            }
        }
    }
}
