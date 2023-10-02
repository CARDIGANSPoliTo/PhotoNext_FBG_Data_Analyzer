#include "peakchart.h"
#include "qeasingcurve.h"
#include "qvalueaxis.h"

PeakChart::PeakChart()
    : m_min{{0,0},0}
    , m_max{{0,0},1}
    , m_dynamicRange{false}
{
    QValueAxis *x = new QValueAxis();
    x->setGridLineVisible(false);
    chart()->addAxis(x, Qt::AlignBottom);
    QValueAxis *y = new QValueAxis();
    y->setGridLineVisible(false);
    chart()->addAxis(y, Qt::AlignLeft);

    connect(&g_db, &MongoInterface::peakReceived, this, &PeakChart::newPeak);
    x->setRange(0, 1);
    y->setRange(0, 1.10);
}

void PeakChart::newPeak(int channel, int grating, qreal peak)
{
    QPair<int, int> sensor = {channel, grating};
    if(m_series.contains(sensor))
    {
        m_peaks[sensor] = peak;

        QLineSeries *series = m_series[sensor];
        series->clear();

        if(series->isVisible())
            adjustRange(sensor, peak);

        QEasingCurve curve(QEasingCurve::InOutQuad);
        series->append(1000, 0);
        for(int i=10; i>0; i-=1){
            float fI = i / 10.0;
            float val = curve.valueForProgress(1 - fI);
            series->append(peak - fI/4, val);
        }
        for(int i=0; i<=10; i+=1){
            float fI = i / 10.0;
            float val = curve.valueForProgress(1 - fI);
            series->append(peak + fI/4, val);
        }
        series->append(2000, 0);

        setLabel(channel, grating);
    }
}

QPair<QPair<int, int>, qreal> PeakChart::min()
{
    QPair<QPair<int, int>, qreal> min = m_max;
    for(auto it = m_peaks.begin(); it != m_peaks.end(); it++)
    {
        if(!m_series[it.key()]->isVisible())
            continue;

        if(it.value() < min.second)
            min = {it.key(), it.value()};
    }
    return min;
}

QPair<QPair<int, int>, qreal> PeakChart::max()
{
    QPair<QPair<int, int>, qreal> max = m_min;
    for(auto it = m_peaks.begin(); it != m_peaks.end(); it++)
    {
        if(!m_series[it.key()]->isVisible())
            continue;

        if(it.value() > max.second)
            max = {it.key(), it.value()};
    }
    return max;
}

void PeakChart::adjustRange(QPair<int, int> sensor, qreal peak)
{
    QValueAxis *xAxis = (QValueAxis*)(chart()->axes(Qt::Horizontal)[0]);

    if(peak < m_min.second || m_min.second == 0)
    {
        xAxis->setMin(peak - PEAK_OFFSET);
        m_min = {sensor, peak};
    }
    if(peak > m_max.second)
    {
        xAxis->setMax(peak + PEAK_OFFSET);
        m_max = {sensor, peak};
    }

    if(m_dynamicRange)
    {
        if(sensor == m_min.first && peak > m_min.second)
        {
            m_min = min();
            xAxis->setMin(m_min.second - PEAK_OFFSET);
        }
        if(sensor == m_max.first && peak < m_max.second)
        {
            m_max = max();
            xAxis->setMax(m_max.second + PEAK_OFFSET);
        }
    }
}

void PeakChart::setLabel(int channel, int grating)
{
    QPair<int, int> sensor = {channel, grating};
    qreal peak = m_peaks[sensor];
    QLineSeries *series = m_series[sensor];

    if(!m_labels.contains(sensor))
    {
        auto label = new QGraphicsSimpleTextItem(QString("%1").arg(peak));
        label->setBrush(QBrush(Qt::white));
        QPointF point = mapFromParent(chart()->mapToPosition(QPointF(peak, 1), series));
        point.setX(point.x() - label->boundingRect().width() / 2);
        point.setY(point.y() - sceneRect().height() * 0.06);
        label->setPos(point);
        label->setVisible(false);
        scene()->addItem(label);
        m_labels[sensor] = label;

        connect(series, &QLineSeries::hovered, [=](QPointF point, bool state)
        {
            label->setVisible(state);

            QPen pen = series->pen();
            pen.setWidth(state ? 3 : 2);
            series->setPen(pen);
        });
    }
    else
    {
        auto label = m_labels[sensor];
        QPointF point = mapFromParent(chart()->mapToPosition(QPointF(peak, 1), series));
        point.setX(point.x() - label->boundingRect().width() / 2);
        point.setY(point.y() - sceneRect().height() * 0.06);
        label->setPos(point);
        label->setText(QString("%1").arg(peak));
    }
}

void PeakChart::resizeEvent(QResizeEvent *event)
{
    BaseChart::resizeEvent(event);

    for(auto sensor : m_labels.keys())
    {
        setLabel(sensor.first, sensor.second);
    }
}

void PeakChart::setDynamicRange(bool state)
{
    m_dynamicRange = state;
}
