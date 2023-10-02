#ifndef MOCKCHART_H
#define MOCKCHART_H

#include "basechart.h"

class MockChart : public BaseChart
{
    Q_OBJECT
public:
    explicit MockChart(const QColor &color = QColor::Invalid);
    ~MockChart();

    void addLineSeries(QMap<float, float> points, const QColor &color = QColor::Invalid);
    void addPeak(float peak, float x, const QColor &color = QColor::Invalid);
    void reset();

private:
    qreal xMax = 8;
    qreal yMax = 4;

    void checkMax(qreal x, qreal y);

signals:

};

#endif // MOCKCHART_H
