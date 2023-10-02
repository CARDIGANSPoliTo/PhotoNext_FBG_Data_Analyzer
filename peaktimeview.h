#ifndef PEAKTIMEVIEW_H
#define PEAKTIMEVIEW_H

#include "mainoptionsdialog.h"
#include "mongointerface.h"
#include <QWidget>

extern MongoInterface g_db;

class PeakTimeView : public QWidget
{
    Q_OBJECT
public:
    explicit PeakTimeView(QWidget *parent = nullptr);

private:
    MainOptionsDialog *m_optionsDialog;

signals:

};

#endif // PEAKTIMEVIEW_H
