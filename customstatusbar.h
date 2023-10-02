#ifndef CUSTOMSTATUSBAR_H
#define CUSTOMSTATUSBAR_H

#include "mongointerface.h"
#include "qlabel.h"
#include <QStatusBar>

extern MongoInterface g_db;

class CustomStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    CustomStatusBar();

private:
    MongoInterface *m_dbInterface;
    QLabel *m_dbStatus;
    QLabel *m_latency;
    QLabel *m_logStatus;

    void DBStatusThread();

public slots:
    void changeDBStatus(MongoInterface::status status);
    void showMssingParamMessage();
    void showConnFailedMessage();
    void updateLatency(qint64 latency);
    void updateLogStatus(bool state);
};

#endif // CUSTOMSTATUSBAR_H
