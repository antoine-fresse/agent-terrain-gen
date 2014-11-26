#ifndef MOUNTAINAGENT_H
#define MOUNTAINAGENT_H

#include "abstractagent.h"
#include "../noise/noise.h"

#include <map>

class HeightMap;

class MountainAgent : public AbstractAgent
{
public:
    MountainAgent();

    void spawn(HeightMap* world);
    void run();
    bool isDead();

    QString getTypeName() const;
    std::vector<QString> getProperties();
    std::unique_ptr<IAgent> copy();

private:
    float getSquareDistance(int x, int y, int x2, int y2);

    int m_life;

    HeightMap* m_world;

    int m_x;
    int m_y;

    int m_ticks;

    int m_directions[8][2];
    int m_directionIndex;

    SimplexNoise m_noise;
};

#endif // MOUNTAINAGENT_H
