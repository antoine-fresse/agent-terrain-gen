#ifndef SMOOTHAGENT_H
#define SMOOTHAGENT_H

#include "abstractagent.h"

#include <map>

class HeightMap;

class SmoothAgent : public AbstractAgent
{
public:
    SmoothAgent();

    void spawn(HeightMap* world);
    void run();
    bool isDead();

    QString getTypeName() const;
    std::vector<QString> getProperties();
    std::unique_ptr<IAgent> copy();

private:
    int m_life;

    HeightMap* m_world;

    int m_x;
    int m_y;
};

#endif // SMOOTHAGENT_H
