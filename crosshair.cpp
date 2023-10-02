#include "crosshair.h"
#include "basechart.h"
#include "qdatetime.h"
#include "qdatetimeaxis.h"
#include "qvalueaxis.h"

Crosshair::Crosshair(BaseChart *chart)
    : m_chart{chart}
    , m_coordinates{0, 0}
{
    m_vLine = m_chart->scene()->addLine(QLine());
    m_vLine->setPen(QPen(QBrush(Qt::gray), 2));

    m_hLine = m_chart->scene()->addLine(QLine());
    m_hLine->setPen(QPen(QBrush(Qt::gray), 2));

    m_vLabel = m_chart->scene()->addText(QString());
    m_vLabel->setDefaultTextColor(Qt::white);
    m_vLabel->setZValue(11);

    m_hLabel = m_chart->scene()->addText(QString());
    m_hLabel->setDefaultTextColor(Qt::white);
    m_hLabel->setZValue(11);

    m_vRect = m_chart->scene()->addRect(QRectF());
    m_vRect->setPen(QPen(QBrush(Qt::white), 2));
    m_vRect->setBrush(QBrush(Qt::black));
    m_vRect->setZValue(10);

    m_hRect = m_chart->scene()->addRect(QRect());
    m_hRect->setPen(QPen(QBrush(Qt::white), 2));
    m_hRect->setBrush(QBrush(Qt::black));
    m_hRect->setZValue(10);

    setVisible(false);
}

void Crosshair::changeCoordinates(QPointF coord)
{
    m_coordinates = coord;
    paint();
}

void Crosshair::setVisible(bool state)
{
    m_vLine->setVisible(state);
    m_hLine->setVisible(state);
    m_vLabel->setVisible(state);
    m_hLabel->setVisible(state);
    m_vRect->setVisible(state);
    m_hRect->setVisible(state);
}

void TimeChartCrosshair::paint()
{
    auto chart = m_chart->chart();
    auto *xAxis = (QDateTimeAxis*)(chart->axes(Qt::Horizontal)[0]);
    auto *yAxis = (QValueAxis*)(chart->axes(Qt::Vertical)[0]);
    if(chart->series().count() > 0)
    {
        auto series = chart->series()[0];

        QPointF a(m_coordinates.x(), yAxis->max());
        QPointF b(m_coordinates.x(), yAxis->min());
        a = chart->mapToScene(chart->mapToPosition(a, series));
        b = chart->mapToScene(chart->mapToPosition(b, series));
        m_vLine->setLine(QLine(a.toPoint(), b.toPoint()));

        a = QPointF(xAxis->min().toMSecsSinceEpoch(), m_coordinates.y());
        b = QPointF(xAxis->max().toMSecsSinceEpoch(), m_coordinates.y());
        a = chart->mapToScene(chart->mapToPosition(a, series));
        b = chart->mapToScene(chart->mapToPosition(b, series));
        m_hLine->setLine(QLine(a.toPoint(), b.toPoint()));

        QDateTime time = QDateTime::fromMSecsSinceEpoch(m_coordinates.x());
        m_vLabel->setPlainText(QString("%1").arg(time.toString("hh:mm:ss")));
        a = QPointF(m_coordinates.x(), yAxis->min());
        a = chart->mapToScene(chart->mapToPosition(a, series));
        auto size = m_vLabel->boundingRect();
        a = QPointF(a.x() - size.width() / 2, a.y());
        m_vLabel->setPos(a);
        m_vRect->setRect(m_vLabel->boundingRect());
        m_vRect->setPos(a);

        m_hLabel->setPlainText(QString("%1").arg(m_coordinates.y()));
        a = QPointF(xAxis->min().toMSecsSinceEpoch(), m_coordinates.y());
        a = chart->mapToScene(chart->mapToPosition(a, series));
        size = m_hLabel->boundingRect();
        a = QPointF(a.x() - size.width(), a.y() - size.height() / 2);
        m_hLabel->setPos(a);
        m_hRect->setRect(m_hLabel->boundingRect());
        m_hRect->setPos(a);
    }
}

void FFTChartCrosshair::paint()
{
    auto chart = m_chart->chart();
    auto *xAxis = (QValueAxis*)(chart->axes(Qt::Horizontal)[0]);
    auto *yAxis = (QValueAxis*)(chart->axes(Qt::Vertical)[0]);
    if(chart->series().count() > 0)
    {
        auto series = chart->series()[0];

        QPointF a(m_coordinates.x(), yAxis->max());
        QPointF b(m_coordinates.x(), yAxis->min());
        a = chart->mapToScene(chart->mapToPosition(a, series));
        b = chart->mapToScene(chart->mapToPosition(b, series));
        m_vLine->setLine(QLine(a.toPoint(), b.toPoint()));

        a = QPointF(xAxis->min(), m_coordinates.y());
        b = QPointF(xAxis->max(), m_coordinates.y());
        a = chart->mapToScene(chart->mapToPosition(a, series));
        b = chart->mapToScene(chart->mapToPosition(b, series));
        m_hLine->setLine(QLine(a.toPoint(), b.toPoint()));

        m_vLabel->setPlainText(QString("%1").arg(m_coordinates.x()));
        a = QPointF(m_coordinates.x(), yAxis->min());
        a = chart->mapToScene(chart->mapToPosition(a, series));
        auto size = m_vLabel->boundingRect();
        a = QPointF(a.x() - size.width() / 2, a.y());
        m_vLabel->setPos(a);
        m_vRect->setRect(m_vLabel->boundingRect());
        m_vRect->setPos(a);

        m_hLabel->setPlainText(QString("%1").arg(m_coordinates.y()));
        a = QPointF(xAxis->min(), m_coordinates.y());
        a = chart->mapToScene(chart->mapToPosition(a, series));
        size = m_hLabel->boundingRect();
        a = QPointF(a.x() - size.width(), a.y() - size.height() / 2);
        m_hLabel->setPos(a);
        m_hRect->setRect(m_hLabel->boundingRect());
        m_hRect->setPos(a);
    }
}
