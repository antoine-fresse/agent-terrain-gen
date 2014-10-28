#include "coastlineagent.h"

#include <iostream>

#include "../heightmap.h"

#include <cstdlib>
#include <algorithm>
#include <ctime>

CoastLineAgent::CoastLineAgent() : m_life{0}, m_world{nullptr}, m_x{0}, m_y{0}, m_terminal{true}
{
}

void CoastLineAgent::spawn(HeightMap* world)
{
    m_world = world;
    int limit = getValue("vertice limit");
    int size = m_world->getSize();
    if (limit < (size * size)) {
        m_terminal = false;
        m_children[0] = copyCoastLine();
        m_children[0]->spawn(world, (size * size) / 2);
        m_children[1] = copyCoastLine();
        m_children[1]->spawn(world, (size * size) / 2);
    } else {
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
}

void CoastLineAgent::spawn(HeightMap* world, int verticeLimit)
{
    m_world = world;
    int limit = getValue("vertice limit");
    int size = m_world->getSize();
    if (limit < verticeLimit) {
        m_terminal = false;
        m_children[0] = copyCoastLine();
        m_children[0]->spawn(world, verticeLimit / 2);
        m_children[1] = copyCoastLine();
        m_children[1]->spawn(world, verticeLimit / 2);
    } else {
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
}

void CoastLineAgent::run()
{
    if (m_world != nullptr) {
        if (m_terminal) {
            int size = m_world->getSize();

            int directions[8][2] = {
                {-1, 0}, {-1, -1}, {-1, 1},
                {0, -1}, {0, 1},
                {1, -1}, {1, 0}, {1, 1},
            };
            float maxScore = 0.0f;
            int maxIndex = -1;
            for (int i = 0; i < 8; ++i) {
                int newX = m_x + directions[i][0];
                int newY = m_y + directions[i][1];
                if ((newX >= 0) && (newY >= 0) && (newX < size) && (newY < size) && (m_world->get(newX, newY) == 0.0)) {
                    float score = getScore(m_x + directions[i][0], m_y + directions[i][1]);
                    if (score > maxScore) {
                        maxScore = score;
                        maxIndex = i;
                    }
                }
            }
            if (maxIndex > -1) {
                m_x = std::max(std::min(m_x + directions[maxIndex][0], size - 1), 0);
                m_y = std::max(std::min(m_y + directions[maxIndex][1], size - 1), 0);
                float minHeight = getValue("min height");
                float maxHeight = getValue("max height");
                int height = (float)rand() / (float)RAND_MAX * (maxHeight - minHeight) + minHeight;
                m_world->set(m_x, m_y, height);
            } else {
                m_x = std::max(std::min(m_x + m_defaultDirectionX, size - 1), 0);
                m_y = std::max(std::min(m_y + m_defaultDirectionY, size - 1), 0);
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
        return m_life >= 500;
    } else {
        return m_children[0]->isDead() && m_children[1]->isDead();
    }
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
    return std::vector<QString>{"count", "vertice limit", "min height", "max height"};
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
        return 0.0f;
    }
    int size = m_world->getSize();
    if ((x >= size) || (y >= size)) {
        return 0.0f;
    }
    int bx = 0;
    int by = 0;
    if (x > (size / 2)) {
        bx = size;
    }
    if (y > (size / 2)) {
        by = size;
    }
    if (std::abs(bx - x) > std::abs(by - y)) {
        bx = x;
    } else {
        by = y;
    }
    float borderScore = getSquareDistance(bx, by, x, y);
    float attratorScore = getSquareDistance(m_attractorX, m_attractorY, x, y);
    float repulsorScore = getSquareDistance(m_repulsorX, m_repulsorY, x, y);
    return repulsorScore - attratorScore + 3.0 * borderScore;
}

std::pair<int, int> CoastLineAgent::getRandomPosition()
{
    int size = m_world->getSize();
    int x = (float)(size - 1) * (float)rand() / (float)RAND_MAX;
    int y = (float)(size - 1) * (float)rand() / (float)RAND_MAX;
    return std::make_pair<int, int>(std::move(x), std::move(y));
}

std::pair<int, int> CoastLineAgent::getRandomDirection()
{
    int directions[8][2] = {
        {-1, 0}, {-1, -1}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1},
    };
    int r = 7.0f * (float)rand() / (float)RAND_MAX;
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
