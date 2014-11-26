#ifndef ABSTRACTAGENT_H
#define ABSTRACTAGENT_H

#include "iagent.h"
#include <map>
#include <QStringList>
#include <iostream>

class AbstractAgent : public IAgent
{
public:

    int getValue(const QString& property) {
        return m_properties[property];
    }

    void setValue(const QString& property, int value) {
        m_properties[property] = value;
    }

    QString toString() const {
        QString res = getTypeName();

        for (auto& pair : m_properties) {
            std::cout << pair.first.toStdString() << ":" << QString::number(pair.second).toStdString() << std::endl;
            res += "!" + pair.first + ":" + QString::number(pair.second);
        }
        return res;
    }

    virtual void fromString(const QString& description) {
        m_properties.clear();
        QStringList l = description.split("!", QString::SkipEmptyParts);
        if (l.at(0) != getTypeName()) {
            throw std::runtime_error("AbstractAgent::fromString : Erreur de type");
        }
        for (int i = 1; i < l.size(); ++i) {
            QStringList pair = l.at(i).split(":", QString::SkipEmptyParts);
            setValue(pair.at(0), pair.at(1).toInt());
        }

    }

protected:
    std::map<QString, int> m_properties;

};

#endif // ABSTRACTAGENT_H
