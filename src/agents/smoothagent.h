#ifndef SMOOTHAGENT_H
#define SMOOTHAGENT_H

#include "iagent.h"

#include <map>

class HeightMap;

class SmoothAgent : public IAgent
{
public:
    SmoothAgent();

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
};

#endif // SMOOTHAGENT_H
