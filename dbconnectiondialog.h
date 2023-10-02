#ifndef DBCONNECTIONDIALOG_H
#define DBCONNECTIONDIALOG_H

#include "customstatusbar.h"
#include "qcheckbox.h"
#include "qlineedit.h"
#include "sessionsettings.h"
#include <QDialog>

extern CustomStatusBar *g_statusBar;
extern SessionSettings *g_settings;

class DBConnectionDialog : public QDialog
{
    Q_OBJECT
public:
    DBConnectionDialog();

private:
    QLineEdit *m_hostInput;
    QLineEdit *m_portInput;
    QLineEdit *m_dbInput;
    QLineEdit *m_authInput;
    QLineEdit *m_usernameInput;
    QLineEdit *m_passwordInput;
    QCheckBox *m_directInput;
    QCheckBox *m_SSLInput;

    void loadSettings();

public slots:
    void accept() override;

signals:
    void connectionUri(QString uri, QString db);
};

#endif // DBCONNECTIONDIALOG_H
