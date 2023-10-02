#ifndef MAINOPTIONSDIALOG_H
#define MAINOPTIONSDIALOG_H

#include "qcheckbox.h"
#include "qlineedit.h"
#include "sessionsettings.h"
#include <QDialog>

extern SessionSettings *g_settings;

class MainOptionsDialog : public QDialog
{
    Q_OBJECT
public:
    MainOptionsDialog();

    void loadSettings();

private:
    QCheckBox *m_dynamicRange;
    QLineEdit *m_samplingInput;
    QLineEdit *m_rangeInput;
    QCheckBox *m_yStaticRange;
    QLineEdit *m_yRangeMin;
    QLineEdit *m_yRangeMax;
    QCheckBox *m_showGrid;

public slots:
    void accept() override;

signals:
    void dynamicRangeSet(bool state);
    void samplingChanged(long sampling);
    void rangeChanged(long range);
    void yManualRangeSet(bool state);
    void yRangeChanged(qreal min, qreal max);
    void gridSet(bool state);
};

#endif // MAINOPTIONSDIALOG_H
