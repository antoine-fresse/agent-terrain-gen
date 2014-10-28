#ifndef COASTLINEAGENT_H
#define COASTLINEAGENT_H

#include "iagent.h"

#include <map>

class CoastLineAgent : public IAgent
{
public:
    CoastLineAgent();

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

#endif // COASTLINEAGENT_H
