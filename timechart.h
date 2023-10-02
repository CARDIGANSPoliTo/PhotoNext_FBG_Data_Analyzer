#ifndef TIMECHART_H
#define TIMECHART_H

#include "basechart.h"
#include "crosshair.h"
#include "mongointerface.h"
#include "qdatetime.h"
#include "qthread.h"

#define TIME_WINDOW 30
#define TIME_SAMPLING 1000

extern MongoInterface g_db;
extern SessionSettings *g_settings;

class TimeChart : public BaseChart
{
    Q_OBJECT
public:
    TimeChart();

protected:
    QMap<QPair<int, int>, qreal> m_lastPeaks;

private:
    QPair<QPair<int, int>, qreal> m_min;
    QPair<QPair<int, int>, qreal> m_max;
    QPair<qreal, qreal> m_yRange;
    bool m_staticRange;
    QMap<QPair<int, int>, QDateTime> m_lastSensorUpdate;
    QDateTime m_lastUpdate;
    QDateTime m_start;
    long m_sampling;
    long m_range;
    TimeChartCrosshair m_crosshair;
    bool m_isMousePressed;
    QPointF m_mousePosition;
    bool m_scrollingMode;

    void adjustRange(QPair<int, int> sensor, qreal peak, qint64 timestamp);
    void updateCrosshairCoord();
    QPointF checkCoordConstraints(QPointF coord, const QPointF &pos);
    virtual qreal toChartValue(QPair<int, int> sensor, qreal peak) { return peak; };
    void onShowSensor(int channel, int grating, bool show) override;
    void onRemoveSensor(int channel, int grating) override;
    void setMinMax();

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

public slots:
    void newPeak(int channel, int grating, qreal peak, qint64 timestamp);
    void changeSampling(long sampling);
    void changeTimeRange(long range);
    void changeValueRange(qreal min, qreal max);
    void setStaticRange(bool state);
};

class StrainChart : public TimeChart
{
    Q_OBJECT
private:
    QMap<QPair<int, int>, qreal> m_constants;
    QMap<QPair<int, int>, qreal> m_initialStrains;
    QMap<QPair<int, int>, qreal> m_initialPeaks;
    QMap<QPair<int, int>, QPair<QPair<int, int>, qreal>> m_tempSensors;

    qreal toChartValue(QPair<int, int> sensor, qreal peak) override
    {
        if(m_constants.contains(sensor))
        {
            qreal dW = peak - m_initialPeaks[sensor];
            qreal numerator;
            if(m_tempSensors.contains(sensor))
            {
                auto tempSensor = m_tempSensors[sensor].first;
                if(m_lastPeaks.contains(tempSensor))
                {
                    qreal dWT = m_lastPeaks[tempSensor] - m_tempSensors[sensor].second;
                    numerator = dW - dWT;
                }
                else numerator = dW;
            }
            else
            {
                numerator = dW;
            }

            return numerator / m_constants[sensor] + m_initialStrains[sensor];
        }

        return 0;
    };

public slots:
    void saveMeasureParams(int channel, int grating, qreal k, qreal s0, qreal sPeak, QPair<int, int> tSensor = {-1, -1}, qreal tPeak = -1)
    {
        QPair<int, int> sensor = {channel, grating};
        m_constants[sensor] = k;
        m_initialStrains[sensor] = s0;
        m_initialPeaks[sensor] = sPeak == -1 ? m_lastPeaks[sensor] : sPeak;
        if(tSensor.first != -1)
        {
            m_tempSensors[sensor] = {tSensor, tPeak == -1 ? m_lastPeaks[sensor] : tPeak};
            g_settings->saveStrainSensor(channel, grating, k, s0, m_initialPeaks[sensor], tSensor, m_tempSensors[sensor].second);
        }
        else
        {
            if(m_tempSensors.contains(sensor))
                m_tempSensors.remove(sensor);

            g_settings->saveStrainSensor(channel, grating, k, s0, m_initialPeaks[sensor]);
        }
    }
};

class TemperatureChart : public TimeChart
{
    Q_OBJECT
private:
    QMap<QPair<int, int>, qreal> m_constants;
    QMap<QPair<int, int>, qreal> m_initialTemps;
    QMap<QPair<int, int>, qreal> m_initialPeaks;

    qreal toChartValue(QPair<int, int> sensor, qreal peak) override
    {
        qreal dW = peak - m_initialPeaks[sensor];
        return dW / m_constants[sensor] + m_initialTemps[sensor];
    };

public slots:
    void saveMeasureParams(int channel, int grating, qreal k, qreal t0, qreal iPeak = -1)
    {
        QPair<int, int> sensor = {channel, grating};
        m_constants[sensor] = k;
        m_initialTemps[sensor] = t0;
        m_initialPeaks[sensor] = iPeak == -1 ? m_lastPeaks[sensor] : iPeak;
        g_settings->saveTempSensor(channel, grating, k, t0, m_initialPeaks[sensor]);
    }
};

#endif // TIMECHART_H
