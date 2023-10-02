#ifndef CUSTOMMENUBAR_H
#define CUSTOMMENUBAR_H

#include "dbconnectiondialog.h"
#include "loggerdialog.h"
#include "sessionlogger.h"
#include <QMenuBar>

extern MongoInterface g_db;

class CustomMenuBar : public QMenuBar
{
    Q_OBJECT
public:
    CustomMenuBar();
    ~CustomMenuBar();

private:
    DBConnectionDialog *m_dbConnDiag;
    LoggerDialog *m_loggerDiag;
    SessionLogger m_logger;
};

#endif // CUSTOMMENUBAR_H
