#include "Beachagent.h"

#include <iostream>

#include "../heightmap.h"

#include <cstdlib>
#include <algorithm>
#include <ctime>

BeachAgent::BeachAgent() : m_directionIndex{0}, m_foundCoastLine{false}
{
    setValue("count", 5);
    setValue("minLength", 40);
    setValue("maxLength", 30);
    setValue("minDepth", 10);
    setValue("maxDepth", 15);

    int directions[8][2] = {
        {-1, 0}, {-1, -1}, {-1, 1},
        {0, 1}, {1, 1}, {1, 0},
        {1, -1}, {0, -1}
    };

    memcpy(m_directions, directions, sizeof(directions));
}

void BeachAgent::spawn(HeightMap* world)
{

    m_world = world;
    m_length = rand()%(getValue("maxLength") - getValue("minLength")) + getValue("minLength");
    m_depth = rand()%(getValue("maxDepth") - getValue("minDepth")) + getValue("minDepth");

    int side = rand()%4 * m_world->getSize();
    std::pair<int,int> pos;
    switch(side){
        case 0:
        pos=std::make_pair<int,int>(0,rand()%m_world->getSize());
        break;
        case 1:
        pos=std::make_pair<int,int>(m_world->getSize()-1,rand()%m_world->getSize());
        break;
        case 2:
        pos=std::make_pair<int,int>(rand()%m_world->getSize(),0);
        break;
        case 3:
        pos=std::make_pair<int,int>(rand()%m_world->getSize(),m_world->getSize()-1);
        break;
    }

    m_x = pos.first;
    m_y = pos.second;
    m_life = 0;

    m_foundCoastLine = false;

}

void BeachAgent::run()
{
    if ((m_world != nullptr) && m_life < m_length) {
        if (!m_foundCoastLine) {
            lookForCoastLine();
        } else{
            m_life++;
            auto next = getBeachPoint();

            int dirX = next.first - m_x;
            int dirY = next.second - m_y;

            for(int l=0 ; l<m_depth ; l++){
                int newX = m_x+dirX*l;
                int newY = m_y+dirY*l;
                if(!m_world->isValid(newX, newY)) continue;

                m_world->set(newX, newY, 20.0f);
                m_world->setMaterial(newX, newY, HeightMap::Sand);
            }

            std::pair<int,int> pos(m_x,m_y);
            for(int i=-1;i<2;i++){
                for(int j=-1 ;j<2 ;j++){
                    if( i==0 && j==0 ) continue;
                        int newX = m_x+i;
                        int newY = m_y+j;
                        if(!m_world->isValid(newX, newY)) continue;

                        if(m_world->getMaterial(newX,newY) == HeightMap::Water) {
                            if(hasCoastAround(newX,newY)){
                                pos = std::pair<int, int>(newX,newY);
                            }
                        }

                }
            }
            m_x = pos.first;
            m_y = pos.second;

        }
    }
}

std::pair<int,int> BeachAgent::getBeachPoint()
{
    for (int i = 0; i < 8; ++i) {
        if(!m_world->isValid(m_x + m_directions[i][0], m_y + m_directions[i][1])) continue;
        HeightMap::Material m = m_world->getMaterial(m_x + m_directions[i][0], m_y + m_directions[i][1]);
        if (m != HeightMap::Water && m != HeightMap::Sand ) {
            return std::make_pair<int,int>(m_x + m_directions[i][0], m_y + m_directions[i][1]);
        }
    }
    return std::pair<int,int>(m_x, m_y);
}

bool BeachAgent::hasCoastAround(int x, int y)
{
    for (int i = 0; i < 8; ++i) {
        if(!m_world->isValid(x + m_directions[i][0], y + m_directions[i][1])) continue;

        HeightMap::Material m = m_world->getMaterial(x + m_directions[i][0], y + m_directions[i][1]);
        if (m != HeightMap::Sand && m != HeightMap::Water) {
            return true;
        }
    }
    return false;
}


bool BeachAgent::hasLandAround(int x, int y)
{
    for (int i = 0; i < 8; ++i) {
        if(!m_world->isValid(x + m_directions[i][0], y + m_directions[i][1])) continue;

        float h = m_world->get(x + m_directions[i][0], y + m_directions[i][1]);
        if (h >= 1.0f) {
            return true;
        }
    }
    return false;
}

void BeachAgent::lookForCoastLine()
{
    if (m_world->get(m_x, m_y) < 1.0f) {
        bool land = hasLandAround(m_x, m_y);
        if (land) {
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


std::pair<int, int> BeachAgent::getNearest(int x, int y, int x2, int y2)
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

std::pair<int, int> BeachAgent::getNearestFromCenter(int x, int y)
{
    int cx = m_world->getSize() / 2;
    int cy = m_world->getSize() / 2;

    return getNearest(x, y, cx, cy);
}

bool BeachAgent::isDead()
{
    return m_life >= m_length;
}

QString BeachAgent::getTypeName() const
{
    return "Beach";
}

std::vector<QString> BeachAgent::getProperties()
{
    return std::vector<QString>{"count", "minLength", "maxLength", "minDepth", "maxDepth"};
}

std::unique_ptr<IAgent> BeachAgent::copy()
{
    BeachAgent* agent = new BeachAgent();
    agent->m_properties = m_properties;
    return std::unique_ptr<IAgent>(agent);
}

float BeachAgent::getSquareDistance(int x, int y, int x2, int y2)
{
    int dx = x2 - x;
    int dy = y2 - y;
    return (dx * dx) + (dy * dy);
}
