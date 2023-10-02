#ifndef BASECHART_H
#define BASECHART_H

#include "qlineseries.h"
#include "sessionsettings.h"
#include <QChartView>

extern SessionSettings *g_settings;

#define PEAK_OFFSET 0.5

class BaseChart : public QChartView
{
    Q_OBJECT
public:
    BaseChart();

    void setMargins(qreal left, qreal top, qreal right, qreal bottom);
    bool isActive(int channel, int grating);

protected:
    QMap<QPair<int, int>, QLineSeries*> m_series;

    void addSeries(QAbstractSeries *series);

    virtual void onShowSensor(int channel, int grating, bool show) {};
    virtual void onRemoveSensor(int channel, int grating) {};

public slots:
    void addSensor(int channel, int grating, const QColor &color);
    void showSensor(int channel, int grating, bool show);
    void setColor(int channel, int grating, const QColor &color);
    void removeSensor(int channel, int grating);
    void showChartGrid(bool state);
};

#endif // BASECHART_H
