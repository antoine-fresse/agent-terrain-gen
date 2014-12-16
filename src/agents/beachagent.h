#ifndef BEACHAGENT_H
#define BEACHAGENT_H

#include "abstractagent.h"
#include "../noise/noise.h"

#include <map>

class HeightMap;

class BeachAgent : public AbstractAgent
{
public:
    BeachAgent();

    void spawn(HeightMap* world);
    void run();
    bool isDead();

    QString getTypeName() const;
    std::vector<QString> getProperties();
    std::unique_ptr<IAgent> copy();

private:
    float getSquareDistance(int x, int y, int x2, int y2);
    void lookForCoastLine();

    bool hasLandAround(int x, int y);
    std::pair<int, int> getNearest(int x, int y, int x2, int y2);
    std::pair<int, int> getNearestFromCenter(int x, int y);
    bool hasCoastAround(int x, int y);
    std::pair<int,int> getBeachPoint();

    HeightMap* m_world;

    int m_directions[8][2];
    int m_directionIndex;

    int m_x;
    int m_y;

    int m_length;
    int m_depth;
    int m_life;

    bool m_foundCoastLine;

};

#endif // BEACHAGENT_H
