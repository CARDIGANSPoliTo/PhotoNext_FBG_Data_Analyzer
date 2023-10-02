#include "measuresview.h"
#include <QGroupBox>
#include "measuredialog.h"
#include "qpushbutton.h"
#include "qsplitter.h"

MeasuresView::MeasuresView(QWidget *parent)
    : QWidget{parent}
{
    connect(&g_db, &MongoInterface::configReceived, this, &MeasuresView::checkSensor);
    connect(&g_db, &MongoInterface::configReceived, [&](int channel, int grating){
        m_sensors.append({channel, grating});
    });

    // Double chart view
    QSplitter *charts = new QSplitter();
    m_strainChart = new StrainChart();
    m_strainChart->setMargins(10,0,10,10);
    m_tempChart = new TemperatureChart();
    m_tempChart->setMargins(10,10,10,0);

    charts->addWidget(m_strainChart);
    charts->addWidget(m_tempChart);
    charts->setOrientation(Qt::Vertical);
    charts->setStyleSheet("QSplitterHandle { background: gray }");
    charts->setHandleWidth(1);

    // Legends  
    m_strainLegend = new ChartLegend(SessionSettings::MeasuresView, "Strains", {m_strainChart}, true);
    m_tempLegend = new ChartLegend(SessionSettings::MeasuresView, "Temperatures", {m_tempChart}, true);

    // Add button
    QPushButton *addButton = new QPushButton("Add...");
    addButton->setFixedWidth(SIDEBAR_WIDTH);
    connect(addButton, &QPushButton::clicked, [=]{
        MeasureDialog dialog{m_sensors};

        connect(&dialog, &MeasureDialog::strainAdded, m_strainLegend, &ChartLegend::addSensor);
        connect(&dialog, &MeasureDialog::strainAdded, [this](int channel, int grating, qreal k, qreal s0, QPair<int,int> tSensor)
        {
            m_strainChart->saveMeasureParams(channel, grating, k, s0, -1, tSensor, -1);
        });
        connect(&dialog, &MeasureDialog::temperatureAdded, m_tempLegend, &ChartLegend::addSensor);
        connect(&dialog, &MeasureDialog::temperatureAdded, [this](int channel, int grating, qreal k, qreal t0)
        {
            m_tempChart->saveMeasureParams(channel, grating, k, t0);
        });

        dialog.exec();
    });

    // Options button
    QPushButton *options = new QPushButton("Options...");
    options->setFixedWidth(SIDEBAR_WIDTH);
    connect(options, &QPushButton::clicked, &m_optionsDialog, &MeasureOptionsDialog::exec);
    connect(&m_optionsDialog, &MeasureOptionsDialog::samplingChanged, m_strainChart, &TimeChart::changeSampling);
    connect(&m_optionsDialog, &MeasureOptionsDialog::samplingChanged, m_tempChart, &TimeChart::changeSampling);
    connect(&m_optionsDialog, &MeasureOptionsDialog::rangeChanged, m_strainChart, &TimeChart::changeTimeRange);
    connect(&m_optionsDialog, &MeasureOptionsDialog::rangeChanged, m_tempChart, &TimeChart::changeTimeRange);
    connect(&m_optionsDialog, &MeasureOptionsDialog::sManualRangeSet, m_strainChart, &TimeChart::setStaticRange);
    connect(&m_optionsDialog, &MeasureOptionsDialog::tManualRangeSet, m_tempChart, &TimeChart::setStaticRange);
    connect(&m_optionsDialog, &MeasureOptionsDialog::sRangeChanged, m_strainChart, &TimeChart::changeValueRange);
    connect(&m_optionsDialog, &MeasureOptionsDialog::tRangeChanged, m_tempChart, &TimeChart::changeValueRange);
    connect(&m_optionsDialog, &MeasureOptionsDialog::gridSet, m_strainChart, &TimeChart::showChartGrid);
    connect(&m_optionsDialog, &MeasureOptionsDialog::gridSet, m_tempChart, &TimeChart::showChartGrid);
    m_optionsDialog.loadSettings();

    // Sidebar
    QVBoxLayout *sidebar = new QVBoxLayout();
    sidebar->addWidget(m_strainLegend);
    sidebar->addWidget(m_tempLegend);
    sidebar->addWidget(addButton);
    sidebar->addWidget(options);
    sidebar->addStretch();

    // Main layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addLayout(sidebar);
    layout->addWidget(charts);

    this->setLayout(layout);
}

void MeasuresView::checkSensor(int channel, int grating)
{
    auto sensor = g_settings->getMeasureSensor(channel, grating);
    if(sensor.count() > 0)
    {
        switch(sensor[0].value<SessionSettings::sensorEnum>())
        {
        case SessionSettings::Strain:
            if(sensor.count() > 4)
            {
                QPair<int, int> tSensor = sensor[4].value<QPair<int, int>>();
                if(m_sensors.contains(tSensor))
                {
                    m_strainLegend->addSensor(channel, grating);
                    m_strainChart->saveMeasureParams(channel, grating,
                                                     sensor[1].toDouble(), sensor[2].toDouble(), sensor[3].toDouble(),
                                                     tSensor, sensor[5].toDouble());
                }
                else
                {
                    m_sensorsQueue[tSensor].append({channel, grating});
                }
            }
            else
            {
                m_strainLegend->addSensor(channel, grating);
                m_strainChart->saveMeasureParams(channel, grating, sensor[1].toDouble(), sensor[2].toDouble(), sensor[3].toDouble());
            }

            break;
        case SessionSettings::Temperature:
            m_tempLegend->addSensor(channel, grating);
            m_tempChart->saveMeasureParams(channel, grating, sensor[1].toDouble(), sensor[2].toDouble(), sensor[3].toDouble());

            QPair<int, int> tSensor = {channel, grating};
            if(m_sensorsQueue.contains(tSensor))
            {
                for(auto sSensor : m_sensorsQueue[tSensor])
                    checkSensor(sSensor.first, sSensor.second);

                m_sensorsQueue.remove(tSensor);
            }

            break;
        }
    }
}
