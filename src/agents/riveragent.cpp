#include "riveragent.h"

#include <iostream>

#include "../heightmap.h"

#include <cstdlib>
#include <algorithm>
#include <ctime>

RiverAgent::RiverAgent() : m_directionIndex{0}, m_nbTry{0}, m_foundCoastLine{false},
                            m_foundMountain{false}, m_over{false}
{
    setValue("count", 1);
    setValue("nb Try", 1);
    setValue("min Length", 10);
    setValue("Width", 10);

    int directions[8][2] = {
        {-1, 0}, {-1, -1}, {-1, 1},
        {0, 1}, {1, 1}, {1, 0},
        {1, -1}, {0, -1}
    };

    memcpy(m_directions, directions, sizeof(directions));
}

void RiverAgent::spawn(HeightMap* world)
{
    m_world = world;
    m_width = getValue("Width");
    m_maxTry = getValue("nb Try");

    auto pos = m_world->getRandomPosition();
    m_x = pos.first;
    m_y = pos.second;

    m_end = m_world->getRandomMountainPosition();
    if (m_world->getMaterial(m_end.first, m_end.second) != HeightMap::Snow) {
        m_over = true;
    } else {
        m_over = false;
    }

    m_directionIndex = rand()%8;
    m_foundCoastLine = false;
    m_foundMountain = false;
}

void RiverAgent::run()
{
    if ((m_world != nullptr) && (!m_over)) {
        if (!m_foundCoastLine) {
            lookForCoastLine();
        } else if (!m_foundMountain) {
            goToMountain();
        } else if (!m_over) {
            goBackToCoastLine();
        }
    }
}

void RiverAgent::lookForCoastLine()
{
    if (m_world->get(m_x, m_y) < 1.0f) {
        bool land = hasLandAround(m_x, m_y);
        if (land) {
            m_start = std::make_pair(m_x, m_y);
            m_foundCoastLine = true;
            return;
        } else {
            auto next = getNearestFromCenter(m_x, m_y);
            m_x = next.first;
            m_y = next.second;
        }
    } else {
        m_x = m_x + m_directions[m_directionIndex][0];
        m_y = m_y + m_directions[m_directionIndex][1];
    }
}

void RiverAgent::goToMountain()
{
    if (m_world->getMaterial(m_x, m_y) == HeightMap::Snow) {
        if (m_path.size() < (unsigned int)getValue("min Length")) {
            m_nbTry++;
            spawn(m_world);
            return;
        }
        m_foundMountain = true;
    } else {
        m_path.push_back(std::make_pair(m_x, m_y));
        auto next = getNearest(m_x, m_y, m_end.first, m_end.second);
        m_x = next.first;
        m_y = next.second;
    }
}

void RiverAgent::goBackToCoastLine()
{
    if (m_path.size() > 0) {
        auto pos = m_path.back();
        m_path.pop_back();

        int size = m_world->getSize();
        for (int dx = -m_width; dx < m_width; ++dx) {
            int newX = m_x + dx;
            for (int dy = -m_width; dy < m_width; ++dy){
                int newY = m_y + dy;
                if ((newX >= 0)&& (newX < size) && (newY >= 0) && (newY < size)) { // On est dans les limites
                    float dst = getSquareDistance(newX, newY, m_x, m_y);
                    if (dst < (m_width * m_width)) {
                        m_world->set(newX, newY, 0.0f);
                        m_world->setMaterial(newX, newY, HeightMap::Water);
                    }
                }
            }
        }

        m_x = pos.first;
        m_y = pos.second;
    } else {
        m_over = true;
    }
}

bool RiverAgent::hasLandAround(int x, int y)
{
    for (int i = 0; i < 8; ++i) {
        float h = m_world->get(x + m_directions[i][0], y + m_directions[i][1]);
        if (h >= 1.0f) {
            return true;
        }
    }
    return false;
}

std::pair<int, int> RiverAgent::getNearest(int x, int y, int x2, int y2)
{
    int cx = x2;
    int cy = y2;

    std::pair<int, int> nearest = std::make_pair(x + m_directions[0][0], y + m_directions[0][1]);
    float minDistance = getSquareDistance(x + m_directions[0][0], y + m_directions[0][1], cx, cy);

    for (int i = 1; i < 8; ++i) {
        float distance = getSquareDistance(x + m_directions[i][0], y + m_directions[i][1], cx, cy);
        if (distance < minDistance) {
            nearest = std::make_pair(x + m_directions[i][0], y + m_directions[i][1]);
            minDistance = distance;
        }
    }
    return nearest;
}

std::pair<int, int> RiverAgent::getNearestFromCenter(int x, int y)
{
    int cx = m_world->getSize() / 2;
    int cy = m_world->getSize() / 2;

    return getNearest(x, y, cx, cy);
}

bool RiverAgent::isDead()
{
    return m_over || (m_nbTry >= m_maxTry);
}

QString RiverAgent::getTypeName() const
{
    return "River";
}

std::vector<QString> RiverAgent::getProperties()
{
    return std::vector<QString>{"count", "nb Try", "min Length", "Width"};
}

std::unique_ptr<IAgent> RiverAgent::copy()
{
    RiverAgent* agent = new RiverAgent();
    agent->m_properties = m_properties;
    return std::unique_ptr<IAgent>(agent);
}

float RiverAgent::getSquareDistance(int x, int y, int x2, int y2)
{
    int dx = x2 - x;
    int dy = y2 - y;
    return (dx * dx) + (dy * dy);
}
