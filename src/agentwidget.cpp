#include "agentwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>

#include "agents/iagent.h"

AgentWidget::AgentWidget(IAgent* templateAgent) :
    QWidget(nullptr), m_templateAgent{templateAgent}, m_numberOfAgents{0}, m_tokenCount{0}
{
    QVBoxLayout* panel = new QVBoxLayout(); {
        panel->addWidget(new QLabel(m_templateAgent->getTypeName()));

        std::vector<QString> properties = m_templateAgent->getProperties();
        for (auto& property : properties) {
            QHBoxLayout* panel2 = new QHBoxLayout(); {
                panel2->addWidget(new QLabel(property));
                QSpinBox* spinBox = new QSpinBox();
                spinBox->setMinimum(0);
                connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [spinBox, this, property](int newValue) {
                    m_templateAgent->setValue(property, newValue);
                });
                panel2->addWidget(spinBox);
            }
            panel->addLayout(panel2);
        }
    }
    setLayout(panel);
}
