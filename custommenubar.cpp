#include "custommenubar.h"
#include "tableview.h"

CustomMenuBar::CustomMenuBar()
    : m_dbConnDiag(new DBConnectionDialog())
    , m_loggerDiag(new LoggerDialog())
{
    // View
    QMenu *viewMenu = addMenu("View");
    QAction *tableAction = viewMenu->addAction("Table");
    tableAction->setCheckable(true);
    TableView *tableView = new TableView();
    connect(tableView, &TableView::windowClosed, tableAction, &QAction::toggle);
    connect(tableAction, &QAction::toggled, [tableView](bool checked){
        if(checked) tableView->show();
        else tableView->hide();
    });

    // Database
    QMenu *dbMenu = addMenu("Database");

    QAction *connectAction = dbMenu->addAction("Connect...");
    connect(connectAction, &QAction::triggered, m_dbConnDiag, &QDialog::exec);
    connect(m_dbConnDiag, &DBConnectionDialog::connectionUri, &g_db, &MongoInterface::connect);

    QAction *disconnectAction = dbMenu->addAction("Disconnect");
    connect(disconnectAction, &QAction::triggered, &g_db, &MongoInterface::disconnect);

    // Acquisition
    QMenu *acquisitionMenu = addMenu("Acquisition");

    QAction *settingsAction = acquisitionMenu->addAction("Settings...");
    connect(settingsAction, &QAction::triggered, m_loggerDiag, &QDialog::exec);
    connect(m_loggerDiag, &LoggerDialog::settingsChanged, &m_logger, &SessionLogger::changeSettings);
    m_loggerDiag->loadSettings();

    QAction *startAction = acquisitionMenu->addAction("Start");
    connect(startAction, &QAction::triggered, &m_logger, &SessionLogger::start);

    QAction *stopAction = acquisitionMenu->addAction("Stop");
    connect(stopAction, &QAction::triggered, &m_logger, &SessionLogger::stop);
}

CustomMenuBar::~CustomMenuBar()
{
    delete m_dbConnDiag;
    delete m_loggerDiag;
}
