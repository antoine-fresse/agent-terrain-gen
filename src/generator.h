#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <memory>
#include <list>
#include <functional>

#include <QObject>

class HeightMap;
class IAgent;

class Generator : public QObject
{
    Q_OBJECT
public:
    Generator();

    ~Generator();

    /**
     * @brief Charge le générateur à partir d'un fichier.
     * @param filename Le fichier à charger.
     */
    void load(const QString& filename);
    /**
     * @brief Sauvegarde le générateur dans un fichier.
     * @param filename Le fichier dans lequel sauvegarder.
     */
    void save(const QString& filename);
    /**
     * @brief Ajoute un agent dans une phase.
     * @param phase Le numéro de la phase (0, 1, 2).
     * @param agent L'agent à ajouter.
     */
    void addAgent(int phase, IAgent* agent);
    /**
     * @brief Renvoie les agents d'une phase.
     * @param phase Le numéro de la phase
     */
    std::vector<IAgent*> getAgents(int phase);
    /**
     * @brief Remet la génération à 0.
     */
    void reset();
    /**
     * @brief Fait éxécuter à chaque agent un tick.
     */
    void tick();
    /**
     * @brief Execute la génération en une fois.
     */
    void runAll();
    /**
     * @brief Renvoie si la génération a été démarré.
     */
    bool isStarted() const;
    /**
     * @brief Renvoie si la génération a terminé.
     */
    bool isOver() const;
    /**
     * @brief Renvoie la heightmap.
     */
    HeightMap* getHeightMap() const;
    /**
     * @brief Modifie la heightmap.
     */
    void setHeightMap(HeightMap* heightmap);
    /**
     * @brief Renvoie la taille de la heightmap.
     */
    int getHeightMapSize() const;
    /**
     * @brief Modifie la taille de la heightmap.
     */
    void setHeightMapSize();
    /**
     * @brief Assigne un callback lorsque la génération se termine.
     * @param onFinish Le callback à appeler.
     */
    void setOnFinish(std::function<void()> onFinish);

private:
    /**
     * @brief Crée la liste des agents pour la prochaine étape.
     */
    void populateNextStep();

    // La heightmap à modifier
    HeightMap* m_heightmap;
    // Tableau ordonnée des agents à créer pour chaque phase
    std::vector<std::vector<IAgent*>> m_phaseAgents;
    // La génération est-elle en train de tourner
    bool m_isRunning;
    //La génération est-elle démarrée ?
    bool m_hasStarted;
    // L'index de la prochaine phase à executer
    int m_nextPhase;
    // Liste des agents à faire évoluer
    std::list<std::unique_ptr<IAgent>> m_agents;

    std::function<void()> m_onFinish;

signals:

public slots:
};

#endif // GENERATOR_H
