#ifndef MEASUREOPTIONSDIALOG_H
#define MEASUREOPTIONSDIALOG_H

#include "qcheckbox.h"
#include "qlineedit.h"
#include "sessionsettings.h"
#include <QDialog>

extern SessionSettings *g_settings;

class MeasureOptionsDialog : public QDialog
{
    Q_OBJECT
public:
    MeasureOptionsDialog();

    void loadSettings();

private:
    QLineEdit *m_samplingInput;
    QLineEdit *m_rangeInput;
    QCheckBox *m_sStaticRange;
    QLineEdit *m_sRangeMin;
    QLineEdit *m_sRangeMax;
    QCheckBox *m_tStaticRange;
    QLineEdit *m_tRangeMin;
    QLineEdit *m_tRangeMax;
    QCheckBox *m_showGrid;

public slots:
    void accept() override;

signals:
    void samplingChanged(long sampling);
    void rangeChanged(long range);
    void sManualRangeSet(bool state);
    void sRangeChanged(qreal min, qreal max);
    void tManualRangeSet(bool state);
    void tRangeChanged(qreal min, qreal max);
    void gridSet(bool state);
};

#endif // MEASUREOPTIONSDIALOG_H
