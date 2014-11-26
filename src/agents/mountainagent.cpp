#include "mountainagent.h"

#include <iostream>

#include "../heightmap.h"

#include <cstdlib>
#include <algorithm>
#include <ctime>

MountainAgent::MountainAgent() : m_life{0}, m_ticks{0}, m_directionIndex{0}, m_noise{1, 1.0, 5000}
{
    setValue("hauteur", 200);
    setValue("count", 10);
    setValue("life", 150);
    setValue("largeur", 30);
    setValue("pente", 10);
    setValue("tick", 20);

    int directions[8][2] = {
        {-1, 0}, {-1, -1}, {-1, 1},
        {0, 1}, {1, 1}, {1, 0},
        {1, -1}, {0, -1}
    };

    memcpy(m_directions, directions, sizeof(directions));
}

void MountainAgent::spawn(HeightMap* world)
{
    m_world = world;

    int size = m_world->getSize();
    m_x = (float)(size - 1) * (float)rand() / (float)RAND_MAX;
    m_y = (float)(size - 1) * (float)rand() / (float)RAND_MAX;

    m_directionIndex = std::round(7.0f * (float)rand() / (float)RAND_MAX);
}

void MountainAgent::run()
{
    if (m_world != nullptr) {
        int size = m_world->getSize();
        if (m_ticks == getValue("tick")) {
            m_directionIndex++;
            m_directionIndex = m_directionIndex % 8;
        } else if (m_ticks >= (2 * getValue("tick"))) {
            m_directionIndex--;
            if (m_directionIndex < 0) {
                m_directionIndex += 8;
            }
            m_ticks = 0;
        }

        m_x = std::max(std::min(m_x + m_directions[m_directionIndex][0], size - 1), 0);
        m_y = std::max(std::min(m_y + m_directions[m_directionIndex][1], size - 1), 0);

        if (m_world->get(m_x, m_y) > 1) {
            float height = getValue("hauteur");
            float width = getValue("largeur");
            float slope = getValue("pente");
            int dirX = m_directions[m_directionIndex][1];
            int dirY = -m_directions[m_directionIndex][0];

            for (int dr = -width; dr < width; ++dr) {
                int newX = m_x + dirX * dr;
                int newY = m_y + dirY * dr;
                if ((newX >= 0)&& (newX < size) && (newY >= 0) && (newY < size)) { // On est dans les limites
                    float dst = getSquareDistance(newX, newY, m_x, m_y);
                    if (dst < (width * width)) {
                        int newHeight = height + m_noise.getNoise(m_ticks, 0);
                        if (dst > (slope * slope)) {
                            dst = std::sqrt(dst);
                            newHeight = (float)height * (1.0 - (float)(dst - slope) / (float)(width - slope));
                        }
                        if (newHeight > m_world->get(newX, newY)) {
                            m_world->set(newX, newY, newHeight /*+ m_noise.getNoise(newX, newY) * 2.0*/);
                        }
                    }
                }
            }
            if ((dirY != 0) && (dirX != 0)) {
                for (int dr = -width; dr < width; ++dr) {
                    int newX = m_x + dirX * dr;
                    int newY = m_y + dirY * dr + 1;
                    if ((newX >= 0)&& (newX < size) && (newY >= 0) && (newY < size)) { // On est dans les limites
                        float dst = getSquareDistance(newX, newY, m_x, m_y);
                        if (dst < (width * width)) {
                            int newHeight = height + m_noise.getNoise(m_ticks, 0);
                            if (dst > (slope * slope)) {
                                dst = std::sqrt(dst);
                                newHeight = (float)height * (1.0 - (float)(dst - slope) / (float)(width - slope));
                            }
                            if (newHeight > m_world->get(newX, newY)) {
                                m_world->set(newX, newY, newHeight /*+ m_noise.getNoise(newX, newY) * 2.0*/);
                            }
                        }
                    }
                }
            }
        }
    }
    m_ticks++;
    m_life++;
}

bool MountainAgent::isDead()
{
    return m_life >= getValue("life");
}

QString MountainAgent::getTypeName() const
{
    return "Mountain";
}

std::vector<QString> MountainAgent::getProperties()
{
    return std::vector<QString>{"count", "life", "hauteur", "largeur", "pente", "tick"};
}

std::unique_ptr<IAgent> MountainAgent::copy()
{
    MountainAgent* agent = new MountainAgent();
    agent->m_properties = m_properties;
    return std::unique_ptr<IAgent>(agent);
}

float MountainAgent::getSquareDistance(int x, int y, int x2, int y2)
{
    int dx = x2 - x;
    int dy = y2 - y;
    return (dx * dx) + (dy * dy);
}
