#ifndef IAGENT_H
#define IAGENT_H

#include <memory>
#include <vector>
#include <QString>

class HeightMap;

class IAgent
{
public:
    virtual ~IAgent() {}
    virtual void spawn(HeightMap* world) = 0;
    virtual void run() = 0;
    virtual bool isDead() = 0;

    virtual int getValue(const QString& property) = 0;
    virtual void setValue(const QString& property, int value) = 0;

    virtual QString getTypeName() const = 0;
    virtual std::vector<QString> getProperties() = 0;
    virtual std::unique_ptr<IAgent> copy() = 0;

    virtual QString toString() const = 0;
    virtual void fromString(const QString& description) = 0;
};

#endif // IAGENT_H
