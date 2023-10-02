#ifndef FFTCHART_H
#define FFTCHART_H

#include "basechart.h"
#include "crosshair.h"
#include "ffft/FFTReal.h"
#include "mongointerface.h"
#include "qthread.h"

#define FFT_SAMPLING 50
#define SAMPLING_WINDOW 1024

extern MongoInterface g_db;

class FFTChart : public BaseChart
{
    Q_OBJECT
public:
    FFTChart();
    ~FFTChart();

private:
    typedef struct {
        qreal peak;
        qint64 timestamp;
    } peak_t;

    QMap<QPair<int,int>,peak_t> m_lastPeaks;
    long m_sampling; // ms
    QMap<QPair<int,int>,QVector<qreal>> m_buffers;
    long m_window;
    QMap<QPair<int,int>,ffft::FFTReal<qreal>*> m_fftEngines;
    FFTChartCrosshair m_crosshair;
    bool m_isMousePressed;
    QPointF m_mousePosition;
    QPair<QPair<int,int>,qreal> m_max;
    QMap<QPair<int,int>,qreal> m_maxes;
    bool m_plotDC;

    void addSample(QPair<int,int> sensor, qreal peak);
    void reset();
    void computeSpectrum(QPair<int,int> sensor);
    void adjustRange(QPair<int, int> sensor, qreal max);
    void updateCrosshairCoord();
    QPointF checkCoordConstraints(QPointF coord, const QPointF &pos);
    void clearChart();
    void setMax();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void onShowSensor(int channel, int grating, bool show) override;

public slots:
    void updatePeak(int channel, int grating, qreal peak, qint64 timestamp);
    void changeOptions(bool dc, long sampling, long window);
    void clearBuffer(int channel, int grating, bool clear = true);
};

#endif // FFTCHART_H
