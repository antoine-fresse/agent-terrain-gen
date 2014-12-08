#include "generatorwindow.h"

#include "gamewidget.h"
#include "agentwidget.h"

#include <QFileDialog>
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
        m_generator.load("temp_conf");
    } catch (...) {


        m_generator.addAgent(0, new CoastLineAgent());
        m_generator.addAgent(1, new MountainAgent());
        m_generator.addAgent(2, new MountainAgent());
        m_generator.addAgent(3, new SmoothAgent());
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

    m_animationTimer.setInterval(1);
    m_generator.connect(&m_animationTimer, &QTimer::timeout, &m_generator, &Generator::tick);
}

GeneratorWindow::~GeneratorWindow()
{
    m_generator.save("temp_conf");
    for (unsigned int i = 0; i < m_agentWidgets.size(); ++i) {
        delete m_agentWidgets[i];
    }
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

    m_gameWidget = new GameWidget(120);
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
    m_agentsToolbar = new QToolBar(); {
        genAgentToolBars();
    }
    addToolBar(Qt::RightToolBarArea, m_agentsToolbar);

    QToolBar* functionToolbar = new QToolBar(); {
        functionToolbar->addWidget(m_runButton);
        functionToolbar->addWidget(m_stepButton);
        functionToolbar->addWidget(m_InstantButton);
        functionToolbar->addWidget(m_resetButton);
    }
    addToolBar(Qt::TopToolBarArea, functionToolbar);

    // AJOUT DE LA BARRE DE MENU
    QMenuBar* menuBar = new QMenuBar(); {
        QMenu* fileMenu = new QMenu("Fichier"); {
            QAction* openAction = fileMenu->addAction("Ouvrir la configuration");
            connect(openAction, &QAction::triggered, this, [this]() {
                QString file = QFileDialog::getOpenFileName(this, tr("Open File"),"", tr("Files (*.*)"));
                if (file != QString()) {
                    m_generator.load(file);
                    genAgentToolBars();
                }
            });
            QAction* saveAction = fileMenu->addAction("Enregistrer la configuration");
            connect(saveAction, &QAction::triggered, this, [this]() {
                QString file = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Images (*.*)"));
                if (file != QString()) {
                    m_generator.save(file);
                }
            });
            QAction* exportAction = fileMenu->addAction("Exporter le resultat");
            connect(exportAction, &QAction::triggered, this, &GeneratorWindow::saveHeightmap);
        }
        menuBar->addMenu(fileMenu);
    }
    setMenuBar(menuBar);
}

void GeneratorWindow::genAgentToolBars()
{
    for (unsigned int i = 0; i < m_agentWidgets.size(); ++i) {
        delete m_agentWidgets[i];
    }
    m_agentWidgets.clear();
    m_agentsToolbar->clear();
    for (int i = 0; i < m_generator.getPhasesCount(); ++i) {
        for (auto& agent : m_generator.getAgents(i)) {
            AgentWidget* widget = new AgentWidget(agent);
            m_agentsToolbar->addWidget(widget);
        }
    }
}

void GeneratorWindow::saveHeightmap()
{
    QFile file("height.raw");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        throw std::runtime_error("Generator::save : impossible de charger le fichier");
    }
    m_gameWidget->getHeightMap()->save(file);
}
