#ifndef MOUNTAINAGENT_H
#define MOUNTAINAGENT_H

#include "iagent.h"

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
    std::map<QString, int> m_properties;

    int m_life;

    HeightMap* m_world;

    int m_x;
    int m_y;

    int m_directionX;
    int m_directionY;
};

#endif // MOUNTAINAGENT_H
