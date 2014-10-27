#include "coastlineagent.h"

#include <iostream>

CoastLineAgent::CoastLineAgent()
{
}

void CoastLineAgent::spawn(HeightMap* world)
{

}

void CoastLineAgent::run()
{

}

bool CoastLineAgent::isDead()
{
    return true;
}

int CoastLineAgent::getValue(const QString& property)
{
    return m_properties[property];
}

void CoastLineAgent::setValue(const QString& property, int value)
{
    m_properties[property] = value;
}

QString CoastLineAgent::getTypeName() const
{
    return "CoastLine";
}
std::vector<QString> CoastLineAgent::getProperties()
{
    return std::vector<QString>{};
}

std::unique_ptr<IAgent> CoastLineAgent::copy()
{
    CoastLineAgent* agent = new CoastLineAgent();
    agent->m_properties = m_properties;
    return std::unique_ptr<IAgent>(agent);
}
