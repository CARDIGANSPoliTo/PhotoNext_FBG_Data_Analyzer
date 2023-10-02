#include "loggerdialog.h"
#include "qdialogbuttonbox.h"
#include "qgridlayout.h"
#include "qheaderview.h"
#include "qlabel.h"
#include "qpushbutton.h"

LoggerDialog::LoggerDialog()
{
    QLabel *samplingLabel = new QLabel("Sampling (ms): ");
    m_samplingInput = new QLineEdit("40");
    m_samplingInput->setValidator(new QIntValidator());

    QLabel *durationLabel = new QLabel("Duration (s): ");
    m_durationInput = new QLineEdit("60");
    m_durationInput->setValidator(new QIntValidator());

    QGridLayout *grid = new QGridLayout();
    grid->setColumnStretch(2, 10);
    grid->addWidget(samplingLabel, 0, 0);
    grid->addWidget(m_samplingInput, 0, 1);
    grid->addWidget(durationLabel, 1, 0);
    grid->addWidget(m_durationInput, 1, 1);

    m_table = new QTableWidget(4, 16);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_table->setStyleSheet(
        "QHeaderView::section{"
            "border-top:0px solid #D8D8D8;"
            "border-left:0px solid #D8D8D8;"
            "border-right:1px solid #D8D8D8;"
            "border-bottom: 1px solid #D8D8D8;"
            "background-color:white;"
            "padding:4px;"
        "}"
        "QTableCornerButton::section{"
            "border-top:0px solid #D8D8D8;"
            "border-left:0px solid #D8D8D8;"
            "border-right:1px solid #D8D8D8;"
            "border-bottom: 1px solid #D8D8D8;"
            "background-color:white;"
        "}"
        "QTableView:item:selected{"
            "background-color: #00AA00;"
        "}"
    );

    QStringList list;
    for(int i=0; i<4; i++)
        list.append("Ch" + QString::number(i));
    m_table->setVerticalHeaderLabels(list);
    list.clear();
    for(int i=0; i<16; i++)
    {
        list.append(QString("Gr%1").arg(i, 2, 10, QChar('0')));
        m_table->setColumnWidth(i, 10);
    }
    m_table->setHorizontalHeaderLabels(list);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(grid);
    layout->addWidget(m_table);
    layout->addSpacing(15);
    layout->addWidget(buttons);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(layout);
}

void LoggerDialog::accept()
{
    if(m_samplingInput->text().isEmpty() || m_durationInput->text().isEmpty() || m_table->selectedRanges().count() < 1)
    {
        g_statusBar->showMssingParamMessage();
        return;
    }

    QDialog::accept();

    QList<QPair<int, int>> sensors;
    for(auto range : m_table->selectedRanges())
    {
        for(int i = range.topRow(); i <= range.bottomRow(); i++)
            for(int j = range.leftColumn(); j<= range.rightColumn(); j++)
                sensors.append({i, j});
    }

    g_settings->saveAcquisitionSettings(m_samplingInput->text().toLong(), m_durationInput->text().toLong(), sensors);
    emit settingsChanged(m_samplingInput->text().toLong(), m_durationInput->text().toLong(), sensors);
}

void LoggerDialog::loadSettings()
{
    auto settings = g_settings->getAcquisitionSettings();
    if(settings.count() > 0)
    {
        m_samplingInput->setText(QString::number(settings[0].toLongLong()));
        m_durationInput->setText(QString::number(settings[1].toLongLong()));
        auto sensors = settings[2].value<QList<QPair<int, int>>>();
        for(auto sensor : sensors)
        {
            QModelIndex index = m_table->model()->index(sensor.first, sensor.second);
            m_table->selectionModel()->select(index, QItemSelectionModel::Select);
        }

        emit settingsChanged(m_samplingInput->text().toLong(), m_durationInput->text().toLong(), sensors);
    }
}
