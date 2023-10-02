#include "basechart.h"
#include "qgraphicslayout.h"
#include "qlineseries.h"
#include "qvalueaxis.h"

BaseChart::BaseChart()
{
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->setTheme(QChart::ChartThemeDark);

    setChart(chart);
    setRenderHint(QPainter::Antialiasing);

    connect(g_settings, &SessionSettings::sensorColorChanged, this, &BaseChart::setColor);
}

void BaseChart::setMargins(qreal left, qreal top, qreal right, qreal bottom)
{
    chart()->layout()->setContentsMargins(left, top, right, bottom);
}

void BaseChart::addSensor(int channel, int grating, const QColor &color)
{
    if(m_series.find({channel, grating}) == m_series.end())
    {
        QLineSeries *series = new QLineSeries();
        series->setPen(QPen(QBrush(color), 2));

        m_series[{channel, grating}] = series;
        addSeries(series);
    }
}

void BaseChart::addSeries(QAbstractSeries *series)
{
    chart()->addSeries(series);
    series->attachAxis(chart()->axes(Qt::Horizontal)[0]);
    series->attachAxis(chart()->axes(Qt::Vertical)[0]);
}

void BaseChart::showSensor(int channel, int grating, bool show)
{
    m_series[{channel, grating}]->setVisible(show);
    onShowSensor(channel, grating, show);
}

void BaseChart::setColor(int channel, int grating, const QColor &color)
{
    QPair<int, int> sensor = {channel, grating};
    if(m_series.contains(sensor))
        m_series[sensor]->setPen(QPen(QBrush(color), 2));
}

void BaseChart::removeSensor(int channel, int grating)
{
    QLineSeries *series = m_series[{channel, grating}];
    chart()->removeSeries(series);
    delete series;
    m_series.remove({channel, grating});
    onRemoveSensor(channel, grating);
}

bool BaseChart::isActive(int channel, int grating)
{
    if(m_series.find({channel, grating}) == m_series.end())
        return false;

    return true;
}

void BaseChart::showChartGrid(bool state)
{
    chart()->axes(Qt::Horizontal)[0]->setGridLineVisible(state);
    chart()->axes(Qt::Vertical)[0]->setGridLineVisible(state);
}
