#include "generator.h"

#include "heightmap.h"
#include "agents/iagent.h"

Generator::Generator() : m_heightmap{nullptr}, m_phaseAgents(3), m_isRunning{false},
                        m_hasStarted{false}, m_nextPhase{0}
{
    setOnFinish([](){});
}

Generator::~Generator()
{
    for (auto& phase : m_phaseAgents) {
        for (auto& agent : phase) {
            delete agent;
        }
    }
}

void Generator::load(const QString& filename)
{
    // TODO
}

void Generator::save(const QString& filename)
{
    // TODO
}

void Generator::addAgent(int phase, IAgent* agent)
{
    m_phaseAgents[phase].push_back(agent);
}

std::vector<IAgent*> Generator::getAgents(int phase)
{
    return m_phaseAgents[phase];
}

void Generator::reset()
{
    m_isRunning = false;
    m_hasStarted = false;
    m_nextPhase = 0;
    m_agents.clear();
    if (m_heightmap != nullptr) {
        m_heightmap->reset();
    }
}

void Generator::tick()
{
    m_hasStarted = true;
    if (m_agents.size() == 0) {
        if (m_nextPhase < 3) {
            populateNextStep();
        } else {
            m_isRunning = false;
            m_onFinish();
        }
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

void Generator::runAll()
{
    m_isRunning = true;
    while (m_isRunning) {
        tick();
    }
}

bool Generator::isStarted() const
{
    return m_hasStarted;
}

bool Generator::isOver() const
{
    return m_hasStarted && !m_isRunning;
}

HeightMap* Generator::getHeightMap() const
{
    return m_heightmap;
}

void Generator::setHeightMap(HeightMap* heightmap)
{
    m_heightmap = heightmap;
}

int Generator::getHeightMapSize() const
{
    if (m_heightmap != nullptr) {
        m_heightmap->getSize();
    }
    return 0;
}

void Generator::setHeightMapSize()
{
    // TODO
}

void Generator::populateNextStep()
{
    m_agents.clear();
    if (m_nextPhase < 3) {
        for (unsigned int i = 0; i < m_phaseAgents[m_nextPhase].size(); ++i) {
            auto& templateAgent = m_phaseAgents[m_nextPhase][i];
            int count = templateAgent->getValue("count");
            for (int j = 0; j < count; ++j) {
                m_agents.push_back(templateAgent->copy());
                m_agents.back()->spawn(m_heightmap);
            }
        }
        m_nextPhase++;
    }
}

void Generator::setOnFinish(std::function<void()> onFinish)
{
    m_onFinish = onFinish;
}
