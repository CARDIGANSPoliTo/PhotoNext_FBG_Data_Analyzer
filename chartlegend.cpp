#include "chartlegend.h"
#include "QCheckBox"
#include "colorpicker.h"
#include "qpushbutton.h"
#include "qdatetime.h"

ChartLegend::ChartLegend(SessionSettings::viewEnum view, QString name, QList<BaseChart*> charts, bool editable, QWidget *parent)
    : QGroupBox{parent},
      m_layout(new QVBoxLayout()),
      m_charts(charts),
      m_editable(editable),
      m_rand(QDateTime::currentSecsSinceEpoch()),
      m_view(view)
{   
    setTitle(name);
    setMaximumWidth(SIDEBAR_WIDTH);
    setMinimumWidth(SIDEBAR_WIDTH);
    setLayout(m_layout);
}

void ChartLegend::addSensor(int channel, int grating)
{
    if(m_charts[0]->isActive(channel, grating))
        return;

    QString name = QString("Ch%1Gr%2")
            .arg(channel, 2, 10, QChar('0'))
            .arg(grating, 2, 10, QChar('0'));
    LegendRow *row = new LegendRow();
    QCheckBox *cb = new QCheckBox(name);
    foreach(BaseChart *chart, m_charts)
    {
        connect(cb, &QCheckBox::clicked, [=](bool checked){
            chart->showSensor(channel, grating, checked);
            emit visibilityChanged(channel, grating, !checked);
            g_settings->saveSensorVisibility(m_view, channel, grating, checked);
        });
    }
    int state = g_settings->getSensorVisibility(m_view, channel, grating);
    if(state == -1) state = true;
    cb->setChecked(state);
    row->layout()->addWidget(cb);

    QColor color = g_settings->getSensorColor(channel, grating);
    if(color == QColor::Invalid)
        color = QColor(m_rand.bounded(0,255), m_rand.bounded(0,255), m_rand.bounded(0,255));

    ColorPicker *picker = new ColorPicker();
    connect(g_settings, &SessionSettings::sensorColorChanged, [=](int ch, int gr, QColor color, QColor master)
    {
        if(ch == channel && gr == grating)
            picker->setColor(color, master);
    });
    foreach(BaseChart *chart, m_charts)
    {
        chart->addSensor(channel, grating, color);
        chart->showSensor(channel, grating, state);

        connect(picker, &ColorPicker::colorChanged, [=](const QColor &color){
            g_settings->saveSensorColor(channel, grating, color);
        });
    }
    g_settings->saveSensorColor(channel, grating, color); // Update channel's gratings
    picker->setMinimumWidth(20);
    row->layout()->addWidget(picker);

    if(m_editable)
    {
        LegendRemoveButton *remButton = new LegendRemoveButton();
        foreach(BaseChart *chart, m_charts)
        {
            connect(remButton, &LegendRemoveButton::clicked, [=]
            {
                chart->removeSensor(channel, grating);
            });
        }
        connect(remButton, &LegendRemoveButton::clicked, [=]
        {
            m_layout->removeWidget(row);
            delete row;

            g_settings->deleteMeasureSensor(channel, grating);
            g_settings->saveSensorVisibility(m_view, channel, grating, true);
        });
        connect(row, &LegendRow::hovered, remButton, &QPushButton::show);
        connect(row, &LegendRow::left, remButton, &QPushButton::hide);
        remButton->hide();
        row->layout()->addWidget(remButton);
    }

    ((QHBoxLayout*)row->layout())->addStretch();
    m_layout->addWidget(row);
}
