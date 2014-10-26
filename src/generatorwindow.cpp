#include "generatorwindow.h"

#include "gamewidget.h"

#include <QToolBar>
#include <QPushButton>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>

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
    setCentralWidget(m_gameWidget);

    QToolBar* agentsToolbar = new QToolBar();
    QPushButton* button = new QPushButton("");
    agentsToolbar->addWidget(button);
    addToolBar(Qt::RightToolBarArea, agentsToolbar);
}
