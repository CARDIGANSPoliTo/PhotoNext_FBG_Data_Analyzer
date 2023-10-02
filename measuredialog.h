#ifndef MEASUREDIALOG_H
#define MEASUREDIALOG_H

#include "customstatusbar.h"
#include "mongointerface.h"
#include "qcombobox.h"
#include <QDialog>

extern MongoInterface g_db;
extern CustomStatusBar *g_statusBar;

class MeasureDialog : public QDialog
{
    Q_OBJECT
public:
    MeasureDialog(QVector<QPair<int, int> > sensors);

private:
    enum type { temperature, strain };

    QComboBox *m_fbgCombo;
    QLineEdit *m_k1Input;
    QLineEdit *m_t0Input;
    QLineEdit *m_k2Input;
    QLineEdit *m_s0Input;
    QComboBox *m_tmpCombo;
    QComboBox *m_typeCombo;

    void accept() override;

signals:
    void temperatureAdded(int channel, int grating, qreal k, qreal t0);
    void strainAdded(int channel, int grating, qreal k, qreal s0, QPair<int, int> tSensor = {-1, -1});
};

#endif // MEASUREDIALOG_H
