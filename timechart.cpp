#include "timechart.h"
#include "qdatetimeaxis.h"
#include "qvalueaxis.h"

#define RETENTION_COUNT 5000
#define PEAK_OFFSET_TIME_CHART 0.01

TimeChart::TimeChart()
    : m_min{{0,0},std::numeric_limits<qreal>::max()}
    , m_max{{0,0},std::numeric_limits<qreal>::min()}
    , m_staticRange{false}
    , m_lastUpdate{QDateTime::fromMSecsSinceEpoch(0)}
    , m_sampling{TIME_SAMPLING}
    , m_range{TIME_WINDOW * 1000}
    , m_start{QDateTime::fromMSecsSinceEpoch(0)}
    , m_crosshair{this}
    , m_isMousePressed{false}
    , m_scrollingMode{false}
{
    QDateTimeAxis *x = new QDateTimeAxis();
    x->setFormat("h:mm:ss");
    x->setGridLineVisible(false);
    chart()->addAxis(x, Qt::AlignBottom);
    QValueAxis *y = new QValueAxis();
    y->setGridLineVisible(false);
    chart()->addAxis(y, Qt::AlignLeft);

    x->setRange(QDateTime::currentDateTime(), QDateTime::currentDateTime().addMSecs(m_range));
    y->setRange(0, 1);

    connect(&g_db, &MongoInterface::peakReceived, this, &TimeChart::newPeak);
}

void TimeChart::newPeak(int channel, int grating, qreal peak, qint64 timestamp)
{
    QPair<int, int> sensor = {channel, grating};
    m_lastPeaks[sensor] = peak;

    if(m_series.contains(sensor))
    {
        QDateTime now = QDateTime::fromMSecsSinceEpoch(timestamp);

        if(!m_lastSensorUpdate.contains(sensor))
            m_lastSensorUpdate[sensor] = QDateTime::fromMSecsSinceEpoch(0);

        if(now.toMSecsSinceEpoch() - m_lastSensorUpdate[sensor].toMSecsSinceEpoch() >= m_sampling)
        {
            QLineSeries *series = m_series[sensor];
            qreal value = toChartValue(sensor, peak);

            if(series->isVisible())
            {
                m_lastUpdate = now;
                adjustRange(sensor, value, timestamp);
            }

            m_lastSensorUpdate[sensor] = now;
            QPointF point(now.toMSecsSinceEpoch(), value);
            series->append(point);

            if(series->count() > RETENTION_COUNT)
            {
                series->remove(0);
            }
        }
    }
}

void TimeChart::adjustRange(QPair<int, int> sensor, qreal peak, qint64 timestamp)
{
    auto *xAxis = (QDateTimeAxis*)(chart()->axes(Qt::Horizontal)[0]);

    if(m_start.toMSecsSinceEpoch() == 0)
    {
        QDateTime now = QDateTime::fromMSecsSinceEpoch(timestamp);
        m_start = now;
        xAxis->setRange(now, now.addMSecs(m_range));
    }
    else if(m_lastUpdate.toMSecsSinceEpoch() > xAxis->max().toMSecsSinceEpoch() && !m_scrollingMode)
    {
        xAxis->setRange(m_lastUpdate.addMSecs(-m_range), m_lastUpdate);
    }

    auto *yAxis = (QValueAxis*)(chart()->axes(Qt::Vertical)[0]);

    if(peak < m_min.second)
    {
        if(!m_staticRange && !m_scrollingMode)
            yAxis->setMin(peak - PEAK_OFFSET_TIME_CHART);
        m_min = {sensor, peak};
    }
    if(peak > m_max.second)
    {
        if(!m_staticRange && !m_scrollingMode)
            yAxis->setMax(peak + PEAK_OFFSET_TIME_CHART);
        m_max = {sensor, peak};
    }

    updateCrosshairCoord();
}

void TimeChart::wheelEvent(QWheelEvent *event)
{
    qreal degree = event->angleDelta().y() / 8;
    auto *xAxis = (QDateTimeAxis*)(chart()->axes(Qt::Horizontal)[0]);

    if(xAxis->min() > m_start && degree < 0)
    {
        m_scrollingMode = true;
        chart()->scroll(degree, 0);
        if(xAxis->min() < m_start)
            xAxis->setMin(m_start);
    }
    else if(xAxis->max() < m_lastUpdate && degree > 0)
    {
        m_scrollingMode = true;
        chart()->scroll(degree, 0);
        if(xAxis->max() > m_lastUpdate)
        {
            xAxis->setMax(m_lastUpdate);
            m_scrollingMode = false;
        }
    }

    setMinMax(); //Could be done more efficently recomputing only on the delta
    updateCrosshairCoord();
}

void TimeChart::changeSampling(long sampling)
{
    m_sampling = sampling;
}

void TimeChart::changeTimeRange(long range)
{
    m_range = range;

    auto *xAxis = (QDateTimeAxis*)(chart()->axes(Qt::Horizontal)[0]);
    if(m_lastUpdate.toMSecsSinceEpoch() - m_start.toMSecsSinceEpoch() >= m_range)
    {
        auto min = QDateTime::fromMSecsSinceEpoch(m_lastUpdate.toMSecsSinceEpoch() - range);
        xAxis->setRange(min, m_lastUpdate);
    }
    else
    {
        auto max = QDateTime::fromMSecsSinceEpoch(m_start.toMSecsSinceEpoch() + range);
        xAxis->setRange(m_start, max);
    }
}

void TimeChart::changeValueRange(qreal min, qreal max)
{
    auto *yAxis = (QValueAxis*)(chart()->axes(Qt::Vertical)[0]);
    if(m_staticRange)
        yAxis->setRange(min, max);

    m_yRange = {min, max};
}

void TimeChart::setStaticRange(bool state)
{
    m_staticRange = state;
    if(!m_staticRange)
    {
        auto *yAxis = (QValueAxis*)(chart()->axes(Qt::Vertical)[0]);
        yAxis->setRange(m_min.second, m_max.second);
    }
}

void TimeChart::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && m_start.toMSecsSinceEpoch() > 0)
    {
        m_mousePosition = event->position();
        updateCrosshairCoord();

        m_crosshair.setVisible(true);
        m_isMousePressed = true;
    }
}

void TimeChart::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isMousePressed)
    {
        m_mousePosition = event->position();
        updateCrosshairCoord();
    }
}

void TimeChart::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_crosshair.setVisible(false);
        m_isMousePressed = false;
    }
}

void TimeChart::updateCrosshairCoord()
{
    auto series = chart()->series()[0];
    auto pos = chart()->mapFromParent(m_mousePosition);
    auto coord = chart()->mapToValue(pos, series);

    coord = checkCoordConstraints(coord, pos);
    m_crosshair.changeCoordinates(coord);
}

QPointF TimeChart::checkCoordConstraints(QPointF coord, const QPointF &pos)
{
    auto *xAxis = (QDateTimeAxis*)(chart()->axes(Qt::Horizontal)[0]);
    auto *yAxis = (QValueAxis*)(chart()->axes(Qt::Vertical)[0]);
    auto xMin = xAxis->min().toMSecsSinceEpoch();
    auto xMax = xAxis->max().toMSecsSinceEpoch();
    auto yMin = yAxis->min();
    auto yMax = yAxis->max();

    if(!chart()->plotArea().contains(pos))
    {
        if(coord.x() < xMin)
        {
            coord.setX(xMin);
        }
        else if(coord.x() > xMax)
        {
            coord.setX(xMax);
        }

        if(coord.y() < yMin)
        {
            coord.setY(yMin);
        }
        else if(coord.y() > yMax)
        {
            coord.setY(yMax);
        }
    }

    return coord;
}

void TimeChart::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_scrollingMode = false;

        auto *xAxis = (QDateTimeAxis*)(chart()->axes(Qt::Horizontal)[0]);
        xAxis->setRange(m_lastUpdate.addMSecs(-m_range), m_lastUpdate);
        setMinMax();
    }
}

void TimeChart::onShowSensor(int channel, int grating, bool show)
{
    auto *xAxis = (QDateTimeAxis*)(chart()->axes(Qt::Horizontal)[0]);
    auto *yAxis = (QValueAxis*)(chart()->axes(Qt::Vertical)[0]);
    QPair<int,int> sensor = {channel, grating};
    auto points = m_series[sensor]->points();

    if(points.count() == 0)
        return;

    if(show)
    {
        int i = points.count()-1;
        if(m_scrollingMode)
            while(i>=0 && points[i].x() > xAxis->max().toMSecsSinceEpoch()) i--;

        for(; i>=0 && points[i].x() >= xAxis->min().toMSecsSinceEpoch(); i--)
        {
            auto peak = points[i].y();
            if(peak < m_min.second)
            {
                yAxis->setMin(peak - PEAK_OFFSET_TIME_CHART);
                m_min = {sensor, peak};
            }
            if(peak > m_max.second)
            {
                yAxis->setMax(peak + PEAK_OFFSET_TIME_CHART);
                m_max = {sensor, peak};
            }
        }
    }
    else if(m_min.first == sensor || m_max.first == sensor)
    {
        setMinMax();
    }
}

void TimeChart::onRemoveSensor(int channel, int grating)
{
    QPair<int,int> sensor = {channel, grating};
    if(m_min.first == sensor || m_max.first == sensor)
        setMinMax();
}

void TimeChart::setMinMax()
{
    auto *xAxis = (QDateTimeAxis*)(chart()->axes(Qt::Horizontal)[0]);
    auto *yAxis = (QValueAxis*)(chart()->axes(Qt::Vertical)[0]);

    m_min = {{0,0},std::numeric_limits<qreal>::max()};
    m_max = {{0,0},std::numeric_limits<qreal>::min()};

    for(auto it=m_series.begin(); it!=m_series.end(); it++)
    {
        auto series = it.value();
        if(!series->isVisible())
            continue;

        auto points = series->points();
        if(points.count() == 0)
            continue;

        int i = points.count()-1;
        if(m_scrollingMode)
            while(i>=0 && points[i].x() > xAxis->max().toMSecsSinceEpoch()) i--;

        for(; i>=0 && points[i].x() >= xAxis->min().toMSecsSinceEpoch(); i--)
        {
            auto peak = points[i].y();
            if(peak < m_min.second)
            {
                yAxis->setMin(peak - PEAK_OFFSET_TIME_CHART);
                m_min = {it.key(), peak};
            }
            if(peak > m_max.second)
            {
                yAxis->setMax(peak + PEAK_OFFSET_TIME_CHART);
                m_max = {it.key(), peak};
            }
        }
    }
}
