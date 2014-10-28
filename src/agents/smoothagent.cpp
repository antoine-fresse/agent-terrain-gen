#include "smoothagent.h"

#include <iostream>

#include "../heightmap.h"

#include <cstdlib>
#include <algorithm>
#include <ctime>

SmoothAgent::SmoothAgent() : m_life{0}
{
}

void SmoothAgent::spawn(HeightMap* world)
{
    m_world = world;

    int size = m_world->getSize();
    m_x = (size - 1) * (float)rand() / (float)RAND_MAX;
    m_y = (size - 1) * (float)rand() / (float)RAND_MAX;
}

void SmoothAgent::run()
{
    if (m_world != nullptr) {
        int size = m_world->getSize();

        int directions[8][2] = {
            {-1, 0}, {-1, -1}, {-1, 1},
            {0, -1}, {0, 1},
            {1, -1}, {1, 0}, {1, 1},
        };
        int r = 7.0f * (float)rand() / (float)RAND_MAX;
        int dx = directions[r][0];
        int dy = directions[r][1];

        m_x = std::max(std::min(m_x + dx, size - 1), 0);
        m_y = std::max(std::min(m_y + dy, size - 1), 0);

        float height = 0.0f;
        int count = 0;
        for (int i = 0; i < 8; ++i) {
            int newX = m_x + directions[i][0];
            int newY = m_y + directions[i][1];
            if ((newX >= 0) && (newY >= 0) && (newX < size) && (newY < size)) {
                height += m_world->get(newX, newY);
                count++;
            }
        }
        height += m_world->get(m_x, m_y);

        m_world->set(m_x, m_y, height / (float)(count + 1));
    }
    m_life++;
}

bool SmoothAgent::isDead()
{
    return m_life >= getValue("life");
}

int SmoothAgent::getValue(const QString& property)
{
    return m_properties[property];
}

void SmoothAgent::setValue(const QString& property, int value)
{
    m_properties[property] = value;
}

QString SmoothAgent::getTypeName() const
{
    return "Smooth";
}

std::vector<QString> SmoothAgent::getProperties()
{
    return std::vector<QString>{"count", "life"};
}

std::unique_ptr<IAgent> SmoothAgent::copy()
{
    SmoothAgent* agent = new SmoothAgent();
    agent->m_properties = m_properties;
    return std::unique_ptr<IAgent>(agent);
}
