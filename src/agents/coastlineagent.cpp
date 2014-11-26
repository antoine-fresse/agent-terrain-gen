#include "coastlineagent.h"

#include <iostream>

#include "../heightmap.h"

#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <limits>

CoastLineAgent::CoastLineAgent() : m_life{0}, m_world{nullptr}, m_x{0}, m_y{0}, m_terminal{true}, m_root{false}, m_noise{20, 0.2, 5000}
{
    setValue("count", 1);
    setValue("vertice limit", 2000);
    setValue("min height", 70);
    setValue("max height", 75);
}

void CoastLineAgent::spawn(HeightMap* world)
{
    m_world = world;
    int size = m_world->getSize();

    m_terminal = true;
    m_root = true;

    m_x = size / 2;
    m_y = size / 2;

    m_vertices = getValue("vertice limit");

    auto attractor = getRandomPosition();
    m_attractorX = std::get<0>(attractor);
    m_attractorY = std::get<1>(attractor);
    auto repulsor = getRandomPosition();
    m_repulsorX = std::get<0>(repulsor);
    m_repulsorY = std::get<1>(repulsor);
    auto direction = getRandomDirection();
    m_defaultDirectionX = std::get<0>(direction);
    m_defaultDirectionY = std::get<1>(direction);
}

void CoastLineAgent::spawn(HeightMap* world, int verticeLimit)
{
    m_world = world;
    int size = m_world->getSize();
    m_vertices = verticeLimit;

    m_terminal = true;
    m_x = (size - 1) * (float)rand() / (float)RAND_MAX;
    m_y = (size - 1) * (float)rand() / (float)RAND_MAX;

    auto attractor = getRandomPosition();
    m_attractorX = std::get<0>(attractor);
    m_attractorY = std::get<1>(attractor);
    auto repulsor = getRandomPosition();
    m_repulsorX = std::get<0>(repulsor);
    m_repulsorY = std::get<1>(repulsor);
    auto direction = getRandomDirection();
    m_defaultDirectionX = std::get<0>(direction);
    m_defaultDirectionY = std::get<1>(direction);
}

void CoastLineAgent::run()
{
    if (m_world != nullptr) {
        if ((m_life == 400) && (m_terminal) && (m_vertices > 2)) {
            m_life = 0;
            m_terminal = false;
            m_children[0] = copyCoastLine();
            m_children[0]->spawn(m_world, m_vertices / 2);
            m_children[0]->m_x = m_x;
            m_children[0]->m_y = m_y;
            m_children[1] = copyCoastLine();
            m_children[1]->spawn(m_world, m_vertices / 2);
            m_children[1]->m_x = m_x;
            m_children[1]->m_y = m_y;
        }
        if (m_terminal) {
            int size = m_world->getSize();

            int directions[8][2] = {
               {-1, 0}, {-1, -1}, {-1, 1},
               {0, -1}, {0, 1},
               {1, -1}, {1, 0}, {1, 1},
            };
            // Le score à utiliser en priorité
            float maxScore = std::numeric_limits<float>::min();
            int maxIndex = -1;
            // Le score à utiliser si le premier n'est pas satisfaisant
            float fallbackScore = std::numeric_limits<float>::max();
            int fallbackIndex = -1;
            int neighborLand = 0;
            for (int i = 0; i < 8; ++i) {
                int newX = m_x + directions[i][0];
                int newY = m_y + directions[i][1];
                if ((newX >= 0) && (newY >= 0) && (newX < size) && (newY < size)) {
                    float score = getScore(m_x + directions[i][0], m_y + directions[i][1]);
                    if (m_world->get(newX, newY) == 0.0) {
                        if ((maxIndex == -1) || (score > maxScore)) {
                            maxScore = score;
                            maxIndex = i;
                        }
                    } else {
                        neighborLand++;
                    }
                    float borderScore = getSquareDistance(size / 2, size / 2, newX, newY);
                    if (borderScore < fallbackScore) {
                        fallbackScore = borderScore;
                        fallbackIndex = i;
                    }
                }
            }
            if ((maxIndex > -1) && ((neighborLand > 0) || m_root)) {
                m_x = std::max(std::min(m_x + directions[maxIndex][0], size - 1), 0);
                m_y = std::max(std::min(m_y + directions[maxIndex][1], size - 1), 0);

                float minHeight = getValue("min height");
                float maxHeight = getValue("max height");
                int height = (m_noise.getNoise(m_x, m_y) + 1.0) * 0.5 * (maxHeight - minHeight) + minHeight;
                m_world->set(m_x, m_y, height);
            } else {
                m_x = std::max(std::min(m_x + directions[fallbackIndex][0], size - 1), 0);
                m_y = std::max(std::min(m_y + directions[fallbackIndex][1], size - 1), 0);
            }
        } else {
            for (int i = 0; i < 2; ++i) {
                m_children[i]->run();
            }
        }
    }
    m_life++;
}

bool CoastLineAgent::isDead()
{
    if (m_terminal) {
        return m_life >= 401;
    } else {
        return m_children[0]->isDead() && m_children[1]->isDead();
    }
}


QString CoastLineAgent::getTypeName() const
{
    return "CoastLine";
}

std::vector<QString> CoastLineAgent::getProperties()
{
    return std::vector<QString>{"vertice limit", "min height", "max height"};
}

std::unique_ptr<IAgent> CoastLineAgent::copy()
{
    CoastLineAgent* agent = new CoastLineAgent();
    agent->m_properties = m_properties;
    return std::unique_ptr<IAgent>(agent);
}

std::unique_ptr<CoastLineAgent> CoastLineAgent::copyCoastLine()
{
    CoastLineAgent* agent = new CoastLineAgent();
    agent->m_properties = m_properties;
    return std::unique_ptr<CoastLineAgent>(agent);
}

float CoastLineAgent::getScore(int x, int y)
{
    if ((x < 0) || (y < 0)) {
        return std::numeric_limits<float>::min();
    }
    int size = m_world->getSize();
    if ((x >= size) || (y >= size)) {
        return std::numeric_limits<float>::min();
    }
    int bx = size / 2;
    int by = size / 2;
    float borderScore = getSquareDistance(bx, by, x, y);
    float attratorScore = getSquareDistance(m_attractorX, m_attractorY, x, y);
    float repulsorScore = getSquareDistance(m_repulsorX, m_repulsorY, x, y);
    return repulsorScore - attratorScore - 3.0 * borderScore;
}

std::pair<int, int> CoastLineAgent::getRandomPosition()
{
    int size = m_world->getSize();
    return std::make_pair<int, int>((float)(size - 1) * (float)rand() / (float)RAND_MAX,
                                    (float)(size - 1) * (float)rand() / (float)RAND_MAX);
}

std::pair<int, int> CoastLineAgent::getRandomInlandPosition()
{
    int size = m_world->getSize();
    while (1) {
        int x = (float)(size - 1) * (float)rand() / (float)RAND_MAX;
        int y = (float)(size - 1) * (float)rand() / (float)RAND_MAX;
        if (m_world->get(x, y) > 0) {
            return std::make_pair<int, int>(std::move(x), std::move(y));
        }
    }
    return std::make_pair<int, int>(0, 0);
}

std::pair<int, int> CoastLineAgent::getRandomDirection()
{
    int directions[8][2] = {
        {-1, 0}, {-1, -1}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1},
    };
    int r = std::round(7.0f * (float)rand() / (float)RAND_MAX);
    int x = directions[r][0];
    int y = directions[r][1];
    return std::make_pair<int, int>(std::move(x), std::move(y));
}

float CoastLineAgent::getSquareDistance(int x, int y, int x2, int y2)
{
    int dx = x2 - x;
    int dy = y2 - y;
    return (dx * dx) + (dy * dy);
}
