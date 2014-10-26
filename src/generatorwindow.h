#ifndef GENERATORWINDOW_H
#define GENERATORWINDOW_H

#include <QMainWindow>

#include <vector>
#include <memory>

#include "agents/IAgent.h"

class GameWidget;
class QToolBar;
class QMenu;

class GeneratorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GeneratorWindow(QWidget* parent = 0);

    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

    /**
     * @brief Crée la liste des agents.
     */
    void populateAgents();
    /**
     * @brief Fait executer à chaque agents une étape.
     */
    void nextStep();
    /**
     * @brief Enregistre la heightmap.
     */
    void saveHeightmap();

private:
    void createView();
    void createLayout();

    GameWidget* m_gameWidget;

    std::vector<std::unique_ptr<IAgent>> m_agents;

signals:

public slots:

};

#endif // GENERATORWINDOW_H
