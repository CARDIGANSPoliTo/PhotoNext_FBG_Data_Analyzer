#include "mockchart.h"
#include "qlineseries.h"
#include "qeasingcurve.h"
#include "qvalueaxis.h"

MockChart::MockChart(const QColor &color)
{
    QValueAxis *x = new QValueAxis();
    x->setGridLineVisible(false);
    chart()->addAxis(x, Qt::AlignBottom);
    QValueAxis *y = new QValueAxis();
    y->setGridLineVisible(false);
    chart()->addAxis(y, Qt::AlignLeft);

    QLineSeries *series = new QLineSeries();
    series->append(0, 0);
    series->append(2, 2);
    series->append(4, 4);
    series->append(6, 2);
    series->append(8, 2);

    if(color.isValid())
        series->setPen(QPen(QBrush(color), 2));
    addSeries(series);
}

void MockChart::addLineSeries(QMap<float, float> points, const QColor &color)
{
    QLineSeries *series = new QLineSeries();
    foreach(const float x, points.keys())
    {
        float y = points[x];

        checkMax(x, y);
        series->append(x, y);
    }

    if(color.isValid())
        series->setPen(QPen(QBrush(color), 2));
    addSeries(series);
}

void MockChart::addPeak(float peak, float x, const QColor &color)
{
    checkMax(x+1, peak);

    QEasingCurve curve(QEasingCurve::InOutQuad);
    QLineSeries *series = new QLineSeries();
    series->append(0,0);
    for(int i=10; i>0; i-=1){
        float fI = i / 10.0;
        float val = curve.valueForProgress(1 - fI) * peak;
        series->append(x - fI/4, val);
    }
    for(int i=0; i<=10; i+=1){
        float fI = i / 10.0;
        float val = curve.valueForProgress(1 - fI) * peak;
        series->append(x + fI/4, val);
    }
    series->append(xMax,0);

    if(color.isValid())
        series->setPen(QPen(QBrush(color), 2));
    addSeries(series);
}

void MockChart::checkMax(qreal x, qreal y)
{
    if(x > xMax)
    {
        xMax = x;
        chart()->axes(Qt::Horizontal)[0]->setRange(0, x);
    }
    if(y > yMax)
    {
        yMax = y;
        chart()->axes(Qt::Vertical)[0]->setRange(0, y);
    }
}

void MockChart::reset()
{
    chart()->removeAllSeries();
}

MockChart::~MockChart()
{

}
