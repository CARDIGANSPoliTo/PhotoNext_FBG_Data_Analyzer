#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include "qgraphicsitem.h"

class BaseChart;

class Crosshair
{
public:
    Crosshair(BaseChart *chart);

    void changeCoordinates(QPointF coord);
    void setVisible(bool state);

private:
    virtual void paint() = 0;

protected:
    BaseChart *m_chart;
    QPointF m_coordinates;
    QGraphicsLineItem *m_vLine;
    QGraphicsLineItem *m_hLine;
    QGraphicsTextItem *m_vLabel;
    QGraphicsTextItem *m_hLabel;
    QGraphicsRectItem *m_vRect;
    QGraphicsRectItem *m_hRect;
};

class TimeChartCrosshair : public Crosshair
{
public:
    TimeChartCrosshair(BaseChart *chart) : Crosshair{chart} {};
private:
    void paint() override;
};

class FFTChartCrosshair : public Crosshair
{
public:
    FFTChartCrosshair(BaseChart *chart) : Crosshair{chart} {};
private:
    void paint() override;
};

#endif // CROSSHAIR_H
