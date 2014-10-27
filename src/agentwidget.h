#ifndef AGENTWIDGET_H
#define AGENTWIDGET_H

#include <QWidget>
#include <QString>

class IAgent;

class AgentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AgentWidget(IAgent* templateAgent);

private:
    void updateNumberOfAgents();

    IAgent* m_templateAgent;
    int m_numberOfAgents;
    int m_tokenCount;

signals:

public slots:

};

#endif // AGENTWIDGET_H
