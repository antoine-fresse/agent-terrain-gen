#include "coastlineagent.h"

#include <iostream>

#include "../heightmap.h"

#include <cstdlib>
#include <algorithm>
#include <ctime>

CoastLineAgent::CoastLineAgent() : m_life{0}, m_world{nullptr}, m_x{0}, m_y{0}
{
}

void CoastLineAgent::spawn(HeightMap* world)
{
    m_world = world;
    int size = m_world->getSize();
    m_x = (size - 1) * (float)rand() / (float)RAND_MAX;
    m_y = (size - 1) * (float)rand() / (float)RAND_MAX;
}

void CoastLineAgent::run()
{
    if (m_world != nullptr) {
        int height = getValue("height");
        m_world->set(m_x, m_y, height);
        int size = m_world->getSize();
        int randX = 3.0 * (float)rand() / (float)RAND_MAX;
        int randY = 3.0 * (float)rand() / (float)RAND_MAX;
        int delta[] = {-1, 0, 1};
        m_x = std::max(std::min(m_x + delta[randX], size - 1), 0);
        m_y = std::max(std::min(m_y + delta[randY], size - 1), 0);
    }
    m_life++;
}

bool CoastLineAgent::isDead()
{
    return m_life >= 1000;
}

int CoastLineAgent::getValue(const QString& property)
{
    return m_properties[property];
}

void CoastLineAgent::setValue(const QString& property, int value)
{
    m_properties[property] = value;
}

QString CoastLineAgent::getTypeName() const
{
    return "CoastLine";
}
std::vector<QString> CoastLineAgent::getProperties()
{
    return std::vector<QString>{"count", "height"};
}

std::unique_ptr<IAgent> CoastLineAgent::copy()
{
    CoastLineAgent* agent = new CoastLineAgent();
    agent->m_properties = m_properties;
    return std::unique_ptr<IAgent>(agent);
}
