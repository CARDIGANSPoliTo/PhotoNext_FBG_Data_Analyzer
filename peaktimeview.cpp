#include "peaktimeview.h"
#include <QGroupBox>
#include "chartlegend.h"
#include "peakchart.h"
#include "qpushbutton.h"
#include "qsplitter.h"
#include "timechart.h"

PeakTimeView::PeakTimeView(QWidget *parent)
    : QWidget{parent}
    , m_optionsDialog{new MainOptionsDialog()}
{
    // Double chart view
    QSplitter *charts = new QSplitter();
    PeakChart *chart1 = new PeakChart();
    chart1->setMargins(10,0,10,10);
    TimeChart *chart2 = new TimeChart();
    chart2->setMargins(10,10,10,0);

    charts->addWidget(chart1);
    charts->addWidget(chart2);
    charts->setOrientation(Qt::Vertical);
    charts->setStyleSheet("QSplitterHandle { background: gray }");
    charts->setHandleWidth(1);

    // Sidebar
    ChartLegend *legend = new ChartLegend(SessionSettings::PeakTimeView, "Sensors", {chart1, chart2});
    connect(&g_db, &MongoInterface::configReceived, legend, &ChartLegend::addSensor);

    QPushButton *options = new QPushButton("Options...");
    options->setFixedWidth(SIDEBAR_WIDTH);
    connect(options, &QPushButton::clicked, m_optionsDialog, &MainOptionsDialog::exec);
    connect(m_optionsDialog, &MainOptionsDialog::dynamicRangeSet, chart1, &PeakChart::setDynamicRange);
    connect(m_optionsDialog, &MainOptionsDialog::gridSet, chart1, &BaseChart::showChartGrid);
    connect(m_optionsDialog, &MainOptionsDialog::samplingChanged, chart2, &TimeChart::changeSampling);
    connect(m_optionsDialog, &MainOptionsDialog::rangeChanged, chart2, &TimeChart::changeTimeRange);
    connect(m_optionsDialog, &MainOptionsDialog::yManualRangeSet, chart2, &TimeChart::setStaticRange);
    connect(m_optionsDialog, &MainOptionsDialog::yRangeChanged, chart2, &TimeChart::changeValueRange);
    connect(m_optionsDialog, &MainOptionsDialog::gridSet, chart2, &BaseChart::showChartGrid);
    m_optionsDialog->loadSettings();

    QVBoxLayout *sidebar = new QVBoxLayout();
    sidebar->addWidget(legend);
    sidebar->addWidget(options);
    sidebar->addStretch();

    // Main layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addLayout(sidebar);
    layout->addWidget(charts);

    this->setLayout(layout);
}
