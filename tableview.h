#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include "mongointerface.h"
#include "qtablewidget.h"
#include <QWidget>

extern MongoInterface g_db;

class TableView : public QWidget
{
    Q_OBJECT
public:
    explicit TableView(QWidget *parent = nullptr);

private:
    QTableWidget *m_table;

    void closeEvent(QCloseEvent *event) override;

signals:
    void windowClosed();

public slots:
    void updatePeak(int channel, int grating, qreal peak);
};

#endif // TABLEVIEW_H
