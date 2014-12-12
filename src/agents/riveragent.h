#ifndef RIVERAGENT_H
#define RIVERAGENT_H

#include "abstractagent.h"
#include "../noise/noise.h"

#include <map>

class HeightMap;

class RiverAgent : public AbstractAgent
{
public:
    RiverAgent();

    void spawn(HeightMap* world);
    void run();
    bool isDead();

    QString getTypeName() const;
    std::vector<QString> getProperties();
    std::unique_ptr<IAgent> copy();

private:
    float getSquareDistance(int x, int y, int x2, int y2);
    void lookForCoastLine();
    void goToMountain();
    void goBackToCoastLine();
    bool hasLandAround(int x, int y);
    std::pair<int, int> getNearest(int x, int y, int x2, int y2);
    std::pair<int, int> getNearestFromCenter(int x, int y);

    HeightMap* m_world;

    int m_directions[8][2];
    int m_directionIndex;

    int m_x;
    int m_y;

    int m_nbTry;
    int m_maxTry;

    int m_width;

    bool m_foundCoastLine;
    bool m_foundMountain;
    bool m_over;

    std::pair<int, int> m_start;
    std::pair<int, int> m_end;
    std::vector<std::pair<int, int>> m_path;
};

#endif // MOUNTAINAGENT_H
