#ifndef LOGGERDIALOG_H
#define LOGGERDIALOG_H

#include "customstatusbar.h"
#include "qlineedit.h"
#include "qtablewidget.h"
#include "sessionsettings.h"
#include <QDialog>

extern CustomStatusBar *g_statusBar;
extern SessionSettings *g_settings;

class LoggerDialog : public QDialog
{
    Q_OBJECT
public:
    LoggerDialog();

    void loadSettings();

private:
    QLineEdit *m_samplingInput;
    QLineEdit *m_durationInput;
    QTableWidget *m_table;

    void accept() override;

signals:
    void settingsChanged(int sampling, int duration, QList<QPair<int, int>> sensors);
};

#endif // LOGGERDIALOG_H
