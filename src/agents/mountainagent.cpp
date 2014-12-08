#include "mountainagent.h"

#include <iostream>

#include "../heightmap.h"

#include <cstdlib>
#include <algorithm>
#include <ctime>

MountainAgent::MountainAgent() : m_life{0}, m_ticks{0}, m_directionIndex{0}, m_noise{1, 1.0, 5000}
{
    setValue("max_hauteur", 300);
    setValue("variation_hauteur", 200);
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

    m_variationHauteur = getValue("variation_hauteur");

    m_slope = getValue("pente");
    m_width = getValue("largeur");
    m_maxHeight = (float)getValue("max_hauteur");

    m_height = m_maxHeight - (float)(rand()%m_variationHauteur);
    m_maxLife = getValue("life");
    m_tick = getValue("tick");

    auto pos = m_world->getRandomInlandPosition();
    m_x = pos.first;
    m_y = pos.second;

    m_directionIndex = rand()%8;
}

void MountainAgent::run()
{
    if (m_world != nullptr) {
        int size = m_world->getSize();
        if (m_ticks == m_tick) {
            m_directionIndex+=2;
            m_directionIndex = m_directionIndex % 8;
        } else if (m_ticks >= (2 * m_tick)) {
            m_directionIndex-=2;
            if (m_directionIndex < 0) {
                m_directionIndex += 8;
            }
            m_ticks = 0;
        }

        m_x = std::max(std::min(m_x + m_directions[m_directionIndex][0], size - 1), 0);
        m_y = std::max(std::min(m_y + m_directions[m_directionIndex][1], size - 1), 0);


        if (m_world->getMaterial(m_x, m_y) != HeightMap::Water) {


            if (m_variationHauteur == 0){
                m_height = m_height - (float)m_slope / (float)m_width;
                if (m_height < (m_maxHeight - (float)m_variationHauteur)){
                    m_height = m_maxHeight - (float)m_variationHauteur;
                    m_variationHauteur = 1;
                }
            }
            else{
                m_height = m_height + (float)m_slope / (float)m_width;
                if (m_height > m_maxHeight){
                    m_height = m_maxHeight;
                    m_variationHauteur = 0;
                }
            }

            int dirX = m_directions[m_directionIndex][1];
            int dirY = -m_directions[m_directionIndex][0];
            bool diag = false;// (dirY != 0) && (dirX != 0);
            for (int dr = -m_width; dr < m_width; ++dr) {
                int newX = m_x + dirX * dr;
                for(int k=0 ; k< (diag ? 2 : 1) ; k++){
                    int newY = m_y + dirY*dr + k;
                    if ((newX >= 0)&& (newX < size) && (newY >= 0) && (newY < size)) { // On est dans les limites
                        float dst = getSquareDistance(newX, newY, m_x, m_y);
                        if (dst < (m_width * m_width)) {
                            dst = std::sqrt(dst);
                            //newHeight = ((float)height * (1.0 - (float)(dst - slope) / (float)(width - slope)));
                            float newHeight = m_world->get(newX, newY) + ((float)m_width - (float)std::abs(dr))/(float)m_width * m_height;
                            newHeight = std::min(newHeight, m_maxHeight);
                            m_world->set(newX, newY, newHeight);
                            m_world->setMaterial(newX, newY, HeightMap::Snow);

                        }
                    }
                }
            }

            //smoothArea(m_x, m_y);
        }
    }
    m_ticks++;
    m_life++;
}

void MountainAgent::smoothArea(int x, int y) {


    int neighbors = m_width;

    int size = m_world->getSize();

    for (int i = -neighbors; i <= neighbors; ++i) {
        for (int j = -neighbors; j <= neighbors; ++j) {
            int newX = x + i;
            int newY = y + j;
            if ((newX >= 0) && (newY >= 0) && (newX < size) && (newY < size)) {
                smooth(newX, newY);
            }
        }
    }


}

void MountainAgent::smooth(int x, int y) {
    int count = 0;
    int neighbors = 2;
    float height = 0.0f;
    int size = m_world->getSize();
    float h = m_world->get(x, y);

    if(h <= 1)
        return;
    for (int i = -neighbors; i <= neighbors; ++i) {
        for (int j = -neighbors; j <= neighbors; ++j) {
            int newX = x + i;
            int newY = y + j;
            if ((newX >= 0) && (newY >= 0) && (newX < size) && (newY < size)) {
                height += m_world->get(newX, newY);
                count++;
            }
        }
    }

    height += 4 * h;
    count += 4;

    m_world->set(x, y, height / (float)(count));
}

bool MountainAgent::isDead()
{
    return m_life >= m_maxLife;
}

QString MountainAgent::getTypeName() const
{
    return "Mountain";
}

std::vector<QString> MountainAgent::getProperties()
{
    return std::vector<QString>{"count", "life", "max_hauteur","variation_hauteur", "largeur", "pente", "tick"};
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
