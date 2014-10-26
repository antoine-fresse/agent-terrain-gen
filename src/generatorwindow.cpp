#include "generatorwindow.h"

#include "gamewidget.h"

#include <QToolBar>
#include <QPushButton>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>

#include <iostream>

GeneratorWindow::GeneratorWindow(QWidget *parent) :
    QMainWindow(parent), m_gameWidget{nullptr}
{
    createView();
    createLayout();
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

}

void GeneratorWindow::createLayout()
{
    // AJOUT DU WIDGET OPENGL
    setCentralWidget(m_gameWidget);

    // AJOUT DES TOOLBARS
    QToolBar* agentsToolbar = new QToolBar(); {
        QPushButton* button = new QPushButton("");
        agentsToolbar->addWidget(button);
    }
    addToolBar(Qt::RightToolBarArea, agentsToolbar);

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

void GeneratorWindow::nextStep()
{

}

void GeneratorWindow::saveHeightmap()
{
    // ENREGISTRE LA HEIGHTMAP
    // TODO
}

void GeneratorWindow::populateAgents()
{
    // TODO
    // REMPLI LE TABLEAU D'AGENTS
}
