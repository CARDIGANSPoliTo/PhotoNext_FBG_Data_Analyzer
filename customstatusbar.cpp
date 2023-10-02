#include "customstatusbar.h"

CustomStatusBar::CustomStatusBar()
    : m_dbStatus(new QLabel("Disconnected"))
    , m_latency(new QLabel("0 ms"))
    , m_logStatus(new QLabel("Idle"))
{
    addPermanentWidget(m_logStatus);
    addPermanentWidget(m_dbStatus);
    addPermanentWidget(m_latency);

    connect(&g_db, &MongoInterface::statusChanged, this, &CustomStatusBar::changeDBStatus);
    connect(&g_db, &MongoInterface::connectionFailed, this, &CustomStatusBar::showConnFailedMessage);
    connect(&g_db, &MongoInterface::latencyComputed, this, &CustomStatusBar::updateLatency);
}

void CustomStatusBar::changeDBStatus(MongoInterface::status status)
{
    switch (status) {
    case MongoInterface::status::connected:
        m_dbStatus->setText("Connected");
        break;
    case MongoInterface::status::disconnected:
        m_dbStatus->setText("Disconnected");
        break;
    case MongoInterface::status::connecting:
        m_dbStatus->setText("Connecting");
        break;
    }
}

void CustomStatusBar::showMssingParamMessage()
{
    showMessage("Missing required parameters", 3000);
}

void CustomStatusBar::showConnFailedMessage()
{
    showMessage("Connection failed", 3000);
}

void CustomStatusBar::updateLatency(qint64 latency)
{
    m_latency->setText(QString("%1 ms").arg(latency));
}

void CustomStatusBar::updateLogStatus(bool state)
{
    if(state)
        m_logStatus->setText("Recording");
    else
        m_logStatus->setText("Idle");
}
