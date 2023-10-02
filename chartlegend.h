#ifndef CHARTLEGEND_H
#define CHARTLEGEND_H

#include <QGroupBox>
#include "basechart.h"
#include "QHBoxLayout"
#include "qlabel.h"
#include "qrandom.h"
#include "sessionsettings.h"

extern SessionSettings *g_settings;

#define SIDEBAR_WIDTH 150

class ChartLegend : public QGroupBox
{
    Q_OBJECT
public:
    ChartLegend(SessionSettings::viewEnum view, QString name, QList<BaseChart*> charts, bool editable = false, QWidget *parent = nullptr);

    QList<BaseChart*> m_charts;

private:
    QVBoxLayout *m_layout;
    bool m_editable;
    QRandomGenerator m_rand;
    SessionSettings::viewEnum m_view;

    void addItems(QList<QString> names, QList<QColor> colors);

public slots:
    void addSensor(int channel, int grating);

signals:
    void visibilityChanged(int channel, int grating, bool state);
};

class LegendRow : public QWidget
{
    Q_OBJECT

public:
    LegendRow(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setLayout(new QHBoxLayout());
        layout()->setContentsMargins(0,0,0,0);
    }

private:
    void enterEvent(QEnterEvent* ev) override { emit hovered(); }
    void leaveEvent(QEvent* ev) override { emit left(); }

signals:
    void hovered();
    void left();
};

class LegendRemoveButton : public QWidget
{
    Q_OBJECT

public:
    LegendRemoveButton(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QHBoxLayout *layout = new QHBoxLayout();
        QLabel *label = new QLabel("X");
        label->setStyleSheet("QLabel { color : red; }");
        layout->addWidget(label);
        layout->setContentsMargins(0,0,0,0);
        setLayout(layout);
    }

private:
    void mouseReleaseEvent(QMouseEvent *event) override { emit clicked(); }

signals:
    void clicked();
};

#endif // CHARTLEGEND_H
