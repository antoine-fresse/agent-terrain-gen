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
    void smooth(int x, int y);

    QString getTypeName() const;
    std::vector<QString> getProperties();
    std::unique_ptr<IAgent> copy();

private:
    int m_life;

    HeightMap* m_world;

    int m_x;
    int m_y;

    int m_start_x;
    int m_start_y;

    int m_maxLife;
    int m_resets;


    int m_directions[8][2];
};

#endif // SMOOTHAGENT_H

