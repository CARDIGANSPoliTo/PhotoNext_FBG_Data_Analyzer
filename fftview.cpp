#include "fftview.h"
#include "fftchart.h"
#include "mockchart.h"
#include "chartlegend.h"
#include "qpushbutton.h"

FFTView::FFTView(QWidget *parent)
    : QWidget{parent}
{
    // Chart
    FFTChart *chart = new FFTChart();
    chart->setMargins(10,0,10,0);

    // Sidebar
    ChartLegend *legend = new ChartLegend(SessionSettings::FFTView, "Sensors", {chart});
    connect(&g_db, &MongoInterface::configReceived, legend, &ChartLegend::addSensor);
    connect(legend, &ChartLegend::visibilityChanged, chart, &FFTChart::clearBuffer);

    QPushButton *options = new QPushButton("Options...");
    options->setFixedWidth(SIDEBAR_WIDTH);
    connect(options, &QPushButton::clicked, &m_optionsDialog, &FFTOptionsDialog::exec);
    connect(&m_optionsDialog, &FFTOptionsDialog::optionsChanged, chart, &FFTChart::changeOptions);
    connect(&m_optionsDialog, &FFTOptionsDialog::gridSet, chart, &BaseChart::showChartGrid);
    m_optionsDialog.loadSettings();

    QVBoxLayout *sidebar = new QVBoxLayout();
    sidebar->addWidget(legend);
    sidebar->addWidget(options);
    sidebar->addStretch();

    // Main layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addLayout(sidebar);
    layout->addWidget(chart);

    this->setLayout(layout);
}
