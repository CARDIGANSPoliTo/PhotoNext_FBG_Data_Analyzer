#include "fftchart.h"
#include "qvalueaxis.h"
#include <synchapi.h>

FFTChart::FFTChart()
    : m_sampling{FFT_SAMPLING}
    , m_window{SAMPLING_WINDOW}
    , m_crosshair{this}
    , m_isMousePressed{false}
    , m_max{{0,0},0}
    , m_plotDC{false}
{
    QValueAxis *x = new QValueAxis();
    x->setGridLineVisible(false);
    chart()->addAxis(x, Qt::AlignBottom);
    QValueAxis *y = new QValueAxis();
    y->setGridLineVisible(false);
    chart()->addAxis(y, Qt::AlignLeft);

    auto hz = 1/(m_sampling/1000.0);
    x->setRange(0, hz/2);
    y->setRange(0, 1);

    connect(&g_db, &MongoInterface::peakReceived, this, &FFTChart::updatePeak);
    connect(&g_db, &MongoInterface::connectionClosed, this, &FFTChart::reset);
}

FFTChart::~FFTChart()
{
    for(auto fft : m_fftEngines)
        delete fft;
}

void FFTChart::updatePeak(int channel, int grating, qreal peak, qint64 timestamp)
{    
    QPair<int, int> sensor = {channel, grating};

    if(!m_series.contains(sensor) || !m_series[sensor]->isVisible())
        return;

    if(!m_lastPeaks.contains(sensor))
    {
        m_lastPeaks[sensor] = {peak, timestamp};
        addSample(sensor, peak);
        return;
    }

    int delta = timestamp - m_lastPeaks[sensor].timestamp;
    for(int i=0; i < delta / m_sampling; i++)
    {
        m_lastPeaks[sensor].timestamp += m_sampling;

        if(m_lastPeaks[sensor].timestamp == timestamp)
            addSample(sensor, peak);
        else
            addSample(sensor, m_lastPeaks[sensor].peak);
    }

    m_lastPeaks[sensor].peak = peak;
}

void FFTChart::addSample(QPair<int,int> sensor, qreal peak)
{
    m_buffers[sensor].append(peak);
    if(m_buffers[sensor].count() == m_window )
        computeSpectrum(sensor);
}

void FFTChart::reset()
{
    m_buffers.clear();
    m_max.second = 0;
}

void FFTChart::computeSpectrum(QPair<int,int> sensor)
{
    if(!m_fftEngines.contains(sensor))
        m_fftEngines[sensor] = new ffft::FFTReal<qreal>(m_window);

    qreal f[m_window];
    m_fftEngines[sensor]->do_fft(f, m_buffers[sensor].constData());

    QList<QPointF> points;
    qreal max;
    for(int i = 0; i<m_window/2; i++)
    {
        qreal abs;
        if(i == 0)
        {
            if(!m_plotDC) continue;
            abs = f[i] / m_window;
        }
        else
        {
            qreal real = f[i];
            qreal img = f[m_window/2 + i];
            abs = qSqrt(real*real + img*img) / m_window;
        }

        qreal freq = i * (1/(m_sampling/1000.0)) / m_window;

        if(i == 0)
            max = abs;
        else if(abs > max)
            max = abs;

        points.append(QPointF(freq, abs));
    }

    m_series[sensor]->clear();
    m_series[sensor]->append(points);
    adjustRange(sensor, max);

    m_buffers[sensor].clear();
}

void FFTChart::adjustRange(QPair<int,int> sensor, qreal max)
{
    auto *yAxis = (QValueAxis*)(chart()->axes(Qt::Vertical)[0]);
    m_maxes[sensor] = max;

    if(max > m_max.second)
    {
        yAxis->setMax(max);
        m_max = {sensor, max};
    }

    if(sensor == m_max.first && max < m_max.second)
        setMax();
}

void FFTChart::setMax()
{
    auto *yAxis = (QValueAxis*)(chart()->axes(Qt::Vertical)[0]);
    m_max.second = 0;
    for(auto it=m_maxes.begin(); it!=m_maxes.end(); it++)
    {
        if(!m_series[it.key()]->isVisible())
            continue;
        if(it.value() > m_max.second)
            m_max = {it.key(), it.value()};
    }
    yAxis->setMax(m_max.second);
}

void FFTChart::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && m_buffers.count() > 0)
    {
        m_mousePosition = event->position();
        updateCrosshairCoord();

        m_crosshair.setVisible(true);
        m_isMousePressed = true;
    }
}

void FFTChart::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isMousePressed)
    {
        m_mousePosition = event->position();
        updateCrosshairCoord();
    }
}

void FFTChart::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_crosshair.setVisible(false);
        m_isMousePressed = false;
    }
}

void FFTChart::updateCrosshairCoord()
{
    auto series = chart()->series()[0];
    auto pos = chart()->mapFromParent(m_mousePosition);
    auto coord = chart()->mapToValue(pos, series);

    coord = checkCoordConstraints(coord, pos);
    m_crosshair.changeCoordinates(coord);
}

QPointF FFTChart::checkCoordConstraints(QPointF coord, const QPointF &pos)
{
    auto *xAxis = (QValueAxis*)(chart()->axes(Qt::Horizontal)[0]);
    auto *yAxis = (QValueAxis*)(chart()->axes(Qt::Vertical)[0]);
    auto xMin = xAxis->min();
    auto xMax = xAxis->max();
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

void FFTChart::clearChart()
{
    for(auto series : chart()->series())
    {
        ((QLineSeries*)series)->clear();
    }
}

void FFTChart::changeOptions(bool dc, long sampling, long window)
{
    m_plotDC = dc;
    m_max.second = 0;

    if(sampling != m_sampling || window != m_window)
    {
        clearChart();

        for(auto fft : m_fftEngines)
        {
            delete fft;
            fft = new ffft::FFTReal<qreal>(window);
        }

        auto hz = 1/(sampling/1000.0);
        chart()->axes(Qt::Horizontal)[0]->setRange(0, hz/2);

        m_buffers.clear();
    }

    m_sampling = sampling;
    m_window = window;
}

void FFTChart::clearBuffer(int channel, int grating, bool clear)
{
    if(!clear) return;

    m_buffers[{channel, grating}].clear();
}

void FFTChart::onShowSensor(int channel, int grating, bool show)
{
    auto *yAxis = (QValueAxis*)(chart()->axes(Qt::Vertical)[0]);
    QPair<int,int> sensor = {channel, grating};

    if(show)
    {
        if(!m_maxes.contains(sensor))
            return;
        setMax();
    }
    else if(m_max.first == sensor)
    {
        setMax();
    }
}
