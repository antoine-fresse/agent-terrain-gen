#ifndef GENERATORWINDOW_H
#define GENERATORWINDOW_H

#include <QMainWindow>

#include <list>
#include <vector>
#include <memory>

#include <QTimer>

#include "agents/iagent.h"
#include "generator.h"

class GameWidget;
class AgentWidget;
class QToolBar;
class QMenu;
class QPushButton;

class GeneratorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GeneratorWindow(QWidget* parent = 0);

    ~GeneratorWindow();

    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

    /**
     * @brief Enregistre la heightmap.
     */
    void saveHeightmap();



private:
    void createView();
    void createLayout();
    void genAgentToolBars();

    QPushButton* m_runButton;
    QPushButton* m_stepButton;
    QPushButton* m_InstantButton;
    QPushButton* m_resetButton;

    QToolBar* m_agentsToolbar;

    // Le widget affichant la heightmap
    GameWidget* m_gameWidget;
    // Les widgets affichant les propiétés des agents
    std::vector<AgentWidget*> m_agentWidgets;
    // Le générateur
    Generator m_generator;
    // Le timer permettant d'animer la simulation
    QTimer m_animationTimer;

signals:

public slots:

};

#endif // GENERATORWINDOW_H
