#ifndef GENERATORWINDOW_H
#define GENERATORWINDOW_H

#include <QMainWindow>

#include <list>
#include <vector>
#include <memory>

#include <QTimer>

#include "agents/iagent.h"

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

    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

    /**
     * @brief Fait executer à chaque agents une étape.
     */
    void runAll();
    /**
     * @brief Enregistre la heightmap.
     */
    void saveHeightmap();

private:
    void createView();
    void createLayout();
    /**
     * @brief Crée la liste des agents pour la première étape.
     */
    void populateFirstStep();
    void populateSecondStep();

    QPushButton* m_runButton;
    QPushButton* m_stepButton;
    QPushButton* m_InstantButton;
    QPushButton* m_resetButton;

    // Le widget affichant la heightmap
    GameWidget* m_gameWidget;
    // Tableau ordonnée des agents à créer pour chaque étape
    std::vector<IAgent*> m_stageAgents;
    // Les widgets affichant les propiétés des agents
    std::vector<AgentWidget*> m_agentWidgets;
    // La génération est-elle en train de tourner
    bool m_isRunning;
    // La première étape est-elle finie ?
    bool m_coastStepOver;
    //La génération est-elle démarrée ?
    bool m_hasStarted;
    // Liste des agents à faire évoluer
    std::list<std::unique_ptr<IAgent>> m_agents;
    // Le timer permettant d'animer la simulation
    QTimer m_animationTimer;

signals:

public slots:

};

#endif // GENERATORWINDOW_H
