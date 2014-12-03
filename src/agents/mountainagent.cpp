#include "mountainagent.h"

#include <iostream>

#include "../heightmap.h"

#include <cstdlib>
#include <algorithm>
#include <ctime>

MountainAgent::MountainAgent() : m_life{0}, m_ticks{0}, m_directionIndex{0}, m_noise{1, 1.0, 5000}
{
    setValue("max_hauteur", 200);
    setValue("variation_hauteur", 100);
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
    m_height = getValue("max_hauteur") - ((float)rand() / (float)RAND_MAX)*getValue("variation_hauteur");
    m_variationHauteur = std::round( (float)rand()/(float)RAND_MAX );

    //std::cout << m_height << std::endl;

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
            float width = getValue("largeur");
            float slope = getValue("pente");

            if (m_variationHauteur == 0){
                m_height = m_height - (float)slope / (float)width;
                if (m_height < getValue("max_hauteur") - getValue("variation_hauteur")){
                    m_height = getValue("max_hauteur") - getValue("variation_hauteur");
                    m_variationHauteur = 1;
                }
            }
            else{
                m_height = m_height + (float)slope / (float)width;
                if (m_height > getValue("max_hauteur")){
                    m_height = getValue("max_hauteur");
                    m_variationHauteur = 0;
                }
            }

            float height = m_height;

            int dirX = m_directions[m_directionIndex][1];
            int dirY = -m_directions[m_directionIndex][0];

            for (int dr = -width; dr < width; ++dr) {
                int newX = m_x + dirX * dr;
                int newY = m_y + dirY * dr;
                if ((newX >= 0)&& (newX < size) && (newY >= 0) && (newY < size)) { // On est dans les limites
                    float dst = getSquareDistance(newX, newY, m_x, m_y);
                    if (dst < (width * width)) {
                        int newHeight = height;// + m_noise.getNoise(m_ticks, 0);
                        //if (dst > (slope * slope)) {
                            dst = std::sqrt(dst);
                            //newHeight = ((float)height * (1.0 - (float)(dst - slope) / (float)(width - slope)));
                            newHeight = (width - std::abs(dr))/width * height;
                        //}

                        m_world->set(newX, newY, m_world->get(newX, newY) + newHeight);
                        m_world->setMaterial(newX, newY, HeightMap::Snow);

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
                            int newHeight = height;// + m_noise.getNoise(m_ticks, 0);
                            //if (dst > (slope * slope)) {
                                dst = std::sqrt(dst);
                                //newHeight = (float)height * (1.0 - (float)(dst - slope) / (float)(width - slope));
                                newHeight = (width - std::abs(dr))/width * height;
                            //}
                            m_world->set(newX, newY, m_world->get(newX, newY) + newHeight);
                            m_world->setMaterial(newX, newY, HeightMap::Snow);
                        }
                    }
                }
            }
            smoothArea(m_x, m_y);
        }
    }
    m_ticks++;
    m_life++;
}

void MountainAgent::smoothArea(int x, int y) {


    int neighbors = getValue("largeur");

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
    return m_life >= getValue("life");
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
