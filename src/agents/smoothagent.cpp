#include "smoothagent.h"

#include <iostream>

#include "../heightmap.h"

#include <cstdlib>
#include <algorithm>
#include <ctime>

SmoothAgent::SmoothAgent() : m_life{0}
{
    setValue("count", 3000);
    setValue("life", 1000);
    setValue("nb_resets", 1);
    int directions[8][2] = {
           {-1, 0}, {-1, -1}, {-1, 1},
           {0, -1}, {0, 1},
           {1, -1}, {1, 0}, {1, 1},
       };
    memcpy(m_directions, directions, sizeof(directions));
}

void SmoothAgent::spawn(HeightMap* world)
{
    m_world = world;

    auto pos = m_world->getRandomInlandPosition();

    m_x = pos.first ;
    m_y = pos.second;

    m_start_x = m_x;
    m_start_y = m_y;

    m_maxLife = getValue("life");
    m_resets = getValue("nb_resets");
}

void SmoothAgent::run()
{
    if (m_world != nullptr) {
        int size = m_world->getSize();


        int r = rand() % 8;

        int neighbors = 2;

        int dx = m_directions[r][0]*neighbors;
        int dy = m_directions[r][1]*neighbors;

        for (int i = -neighbors; i <= neighbors; ++i) {
            for (int j = -neighbors; j <= neighbors; ++j) {
                int newX = m_x + i;
                int newY = m_y + j;
                if ((newX >= 0) && (newY >= 0) && (newX < size) && (newY < size)) {
                    smooth(newX, newY);
                }
            }
        }

        m_x = std::max(std::min(m_x + dx, size - 1), 0);
        m_y = std::max(std::min(m_y + dy, size - 1), 0);


    }
    m_life++;

    if(m_life % (m_maxLife/(m_resets+1)) == 0){
        m_x = m_start_x;
        m_y = m_start_y;
    }
}

void SmoothAgent::smooth(int x, int y) {
    int count = 0;
    int neighbors = 2;
    float height = 0.0f;
    int size = m_world->getSize();

    for (int i = -neighbors; i <= neighbors; ++i) {
        for (int j = -neighbors; j <= neighbors; ++j) {
            int newX = x + i;
            int newY = y + j;
            if ((newX >= 0) && (newY >= 0) && (newX < size) && (newY < size)
                && ((i + j) <= neighbors)) {
                height += m_world->get(newX, newY);
                count++;
            }
        }
    }
    height += 2 * m_world->get(x, y);

    m_world->set(x, y, height / (float)(count + 2));
}

bool SmoothAgent::isDead()
{
    return m_life >= m_maxLife;
}

QString SmoothAgent::getTypeName() const
{
    return "Smooth";
}

std::vector<QString> SmoothAgent::getProperties()
{
    return std::vector<QString>{"count", "life", "nb_resets"};
}

std::unique_ptr<IAgent> SmoothAgent::copy()
{
    SmoothAgent* agent = new SmoothAgent();
    agent->m_properties = m_properties;
    return std::unique_ptr<IAgent>(agent);
}
