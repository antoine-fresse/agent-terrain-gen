#ifndef MOUNTAINAGENT_H
#define MOUNTAINAGENT_H

#include "iagent.h"
#include "../noise/noise.h"

#include <map>

class HeightMap;

class MountainAgent : public IAgent
{
public:
    MountainAgent();

    void spawn(HeightMap* world);
    void run();
    bool isDead();

    int getValue(const QString& property);
    void setValue(const QString& property, int value);

    QString getTypeName() const;
    std::vector<QString> getProperties();
    std::unique_ptr<IAgent> copy();

private:
    float getSquareDistance(int x, int y, int x2, int y2);

    std::map<QString, int> m_properties;

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
