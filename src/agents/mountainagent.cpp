#include "mountainagent.h"

#include <iostream>

#include "../heightmap.h"

#include <cstdlib>
#include <algorithm>
#include <ctime>

MountainAgent::MountainAgent() : m_life{0}
{
    setValue("hauteur", 50);
    setValue("count", 20);
    setValue("life", 500);
}

void MountainAgent::spawn(HeightMap* world)
{
    m_world = world;

    int size = m_world->getSize();
    m_x = (float)(size - 1) * (float)rand() / (float)RAND_MAX;
    m_y = (float)(size - 1) * (float)rand() / (float)RAND_MAX;

    int directions[9][2] = {
        {-1, 0}, {-1, -1}, {-1, 1},
        {0, -1}, {0, 0}, {0, 1},
        {1, -1}, {1, 0}, {1, 1},
    };
    int r = std::round(8.0f * (float)rand() / (float)RAND_MAX);
    m_directionX = directions[r][0];
    m_directionY = directions[r][1];
}

void MountainAgent::run()
{
    if (m_world != nullptr) {
        int size = m_world->getSize();

        m_x = std::max(std::min(m_x + m_directionX, size - 1), 0);
        m_y = std::max(std::min(m_y + m_directionY, size - 1), 0);

        if (m_world->get(m_x, m_y) > 1) {
            int directions[9][2] = {
                {-1, 0}, {-1, -1}, {-1, 1},
                {0, -1}, {0, 0}, {0, 1},
                {1, -1}, {1, 0}, {1, 1},
            };
            float height = getValue("hauteur");
            for (int i = 0; i < 9; ++i) {
                int newX = m_x + directions[i][0];
                int newY = m_y + directions[i][1];
                if ((newX >= 0) && (newY >= 0) && (newX < size) && (newY < size)) {
                    m_world->set(newX, newY, height);
                }
            }
        }
    }
    m_life++;
}

bool MountainAgent::isDead()
{
    return m_life >= getValue("life");
}

int MountainAgent::getValue(const QString& property)
{
    return m_properties[property];
}

void MountainAgent::setValue(const QString& property, int value)
{
    m_properties[property] = value;
}

QString MountainAgent::getTypeName() const
{
    return "Mountain";
}

std::vector<QString> MountainAgent::getProperties()
{
    return std::vector<QString>{"count", "life", "hauteur", "largeur", "pente"};
}

std::unique_ptr<IAgent> MountainAgent::copy()
{
    MountainAgent* agent = new MountainAgent();
    agent->m_properties = m_properties;
    return std::unique_ptr<IAgent>(agent);
}
