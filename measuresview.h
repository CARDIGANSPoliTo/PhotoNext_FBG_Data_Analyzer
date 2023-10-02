#ifndef MEASURESVIEW_H
#define MEASURESVIEW_H

#include "chartlegend.h"
#include "measureoptionsdialog.h"
#include "mongointerface.h"
#include "timechart.h"
#include <QWidget>

extern MongoInterface g_db;
extern SessionSettings *g_settings;

class MeasuresView : public QWidget
{
    Q_OBJECT
public:
    explicit MeasuresView(QWidget *parent = nullptr);

private:
    QVector<QPair<int, int>> m_sensors;
    MeasureOptionsDialog m_optionsDialog;
    ChartLegend *m_strainLegend;
    ChartLegend *m_tempLegend;
    StrainChart *m_strainChart;
    TemperatureChart *m_tempChart;
    QMap<QPair<int, int>, QVector<QPair<int, int>>> m_sensorsQueue;

public slots:
    void checkSensor(int channel, int grating);
};

#endif // MEASURESVIEW_H
