#ifndef IAGENT_H
#define IAGENT_H

class HeightMap;

class IAgent
{
public:
    virtual ~IAgent() {}
    virtual void spawn(HeightMap* world) = 0;
    virtual void run() = 0;
};

#endif // IAGENT_H
