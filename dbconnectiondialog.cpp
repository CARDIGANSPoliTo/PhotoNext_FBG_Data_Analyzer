#include "dbconnectiondialog.h"
#include "qdialogbuttonbox.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qvalidator.h"

DBConnectionDialog::DBConnectionDialog()
{
    QLabel *hostLabel = new QLabel("Host:");
    m_hostInput = new QLineEdit();

    QLabel *portLabel = new QLabel("Port:");
    m_portInput = new QLineEdit();
    m_portInput->setValidator(new QIntValidator(0, 65535));

    QLabel *dbLabel = new QLabel("Database:");
    m_dbInput = new QLineEdit();

    QLabel *authLabel = new QLabel("Auth database:");
    m_authInput = new QLineEdit();
    m_authInput->setPlaceholderText("(Optional)");

    QLabel *usernameLabel = new QLabel("Username:");
    m_usernameInput = new QLineEdit();
    m_usernameInput->setPlaceholderText("(Optional)");

    QLabel *passwordLabel = new QLabel("Password:");
    m_passwordInput = new QLineEdit();
    m_passwordInput->setPlaceholderText("(Optional)");
    m_passwordInput->setEchoMode(QLineEdit::Password);

    m_directInput = new QCheckBox("Direct connection");

    m_SSLInput = new QCheckBox("SSL/TLS connection");

    QGridLayout *table = new QGridLayout();
    table->addWidget(hostLabel, 0, 0);
    table->addWidget(m_hostInput, 0, 1);
    table->addWidget(portLabel, 1, 0);
    table->addWidget(m_portInput, 1, 1);
    table->addWidget(dbLabel, 2, 0);
    table->addWidget(m_dbInput, 2, 1);
    table->addWidget(authLabel, 3, 0);
    table->addWidget(m_authInput, 3, 1);
    table->addWidget(usernameLabel, 4, 0);
    table->addWidget(m_usernameInput, 4, 1);
    table->addWidget(passwordLabel, 5, 0);
    table->addWidget(m_passwordInput, 5, 1);
    table->addWidget(m_directInput, 6, 0, 1, 2);
    table->addWidget(m_SSLInput, 7, 0, 1, 2);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("Connect");
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(table);
    layout->addSpacing(15);
    layout->addWidget(buttons);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(layout);

    loadSettings();
}

void DBConnectionDialog::accept()
{
    if(m_hostInput->text().isEmpty() || m_portInput->text().isEmpty() || m_dbInput->text().isEmpty())
    {
        g_statusBar->showMssingParamMessage();
        return;
    }

    QDialog::accept();

    QString uri = "mongodb://";

    if(!m_usernameInput->text().isEmpty())
    {
        uri.append(m_usernameInput->text());
        uri.append(":");
        uri.append(m_passwordInput->text());
        uri.append("@");
    }

    uri.append(m_hostInput->text());
    uri.append(":");
    uri.append(m_portInput->text());
    uri.append("/?");

    if(!m_authInput->text().isEmpty())
    {
        uri.append("authSource=");
        uri.append(m_authInput->text());
        uri.append("&");
    }

    if(m_directInput->isChecked())
    {
        uri.append("directConnection=true&");
    }

    if(m_SSLInput->isChecked())
    {
        uri.append("ssl=true");
    }

    g_settings->saveDbInfo(m_hostInput->text(), m_portInput->text().toInt(), m_dbInput->text());
    g_settings->saveDbAuth(m_usernameInput->text(), m_passwordInput->text(), m_authInput->text());
    g_settings->saveDbConnection(m_directInput->isChecked(), m_SSLInput->isChecked());
    qInfo() << uri;

    emit connectionUri(uri, m_dbInput->text());
}

void DBConnectionDialog::loadSettings()
{
    auto info = g_settings->getDbInfo();
    if(info.count() > 0)
    {
        m_hostInput->setText(info[0].toString());
        m_portInput->setText(info[1].toString());
        m_dbInput->setText(info[2].toString());
    }

    auto auth = g_settings->getDbAuth();
    if(auth.count() > 0)
    {
        m_usernameInput->setText(auth[0].toString());
        m_passwordInput->setText(auth[1].toString());
        m_authInput->setText(auth[2].toString());
    }

    auto conn = g_settings->getDbConnection();
    if(conn.count() > 0)
    {
        m_directInput->setChecked(conn[0].toBool());
        m_SSLInput->setChecked(conn[1].toBool());
    }
}
