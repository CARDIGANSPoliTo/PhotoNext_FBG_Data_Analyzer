#ifndef PEAKCHART_H
#define PEAKCHART_H

#include "basechart.h"
#include "mongointerface.h"

extern MongoInterface g_db;

class PeakChart : public BaseChart
{
    Q_OBJECT
public:
    PeakChart();

private:
    QPair<QPair<int, int>, qreal> m_min;
    QPair<QPair<int, int>, qreal> m_max;
    QMap<QPair<int, int>, qreal> m_peaks;
    QMap<QPair<int, int>, QGraphicsSimpleTextItem*> m_labels;
    bool m_dynamicRange;

    QPair<QPair<int, int>, qreal> min();
    QPair<QPair<int, int>, qreal> max();
    void adjustRange(QPair<int, int> sensor, qreal peak);
    void setLabel(int channel, int grating);
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void newPeak(int channel, int grating, qreal peak);
    void setDynamicRange(bool state);
};

#endif // PEAKCHART_H
