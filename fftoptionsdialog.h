#ifndef FFTOPTIONSDIALOG_H
#define FFTOPTIONSDIALOG_H

#include "customstatusbar.h"
#include "qcheckbox.h"
#include "qlineedit.h"
#include <QDialog>

extern CustomStatusBar *g_statusBar;

class FFTOptionsDialog : public QDialog
{
    Q_OBJECT
public:
    FFTOptionsDialog();

    void loadSettings();

private:
    QCheckBox *m_dc;
    QLineEdit *m_samplingInput;
    QLineEdit *m_samplesInput;
    QCheckBox *m_showGrid;

    void accept() override;

signals:
    void optionsChanged(bool dc, long sampling, long window);
    void gridSet(bool state);
};

#endif // FFTOPTIONSDIALOG_H
