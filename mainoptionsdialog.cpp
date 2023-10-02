#include "mainoptionsdialog.h"
#include "qdialogbuttonbox.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qvalidator.h"
#include "timechart.h"

MainOptionsDialog::MainOptionsDialog()
{
    m_dynamicRange = new QCheckBox("Dynamic peak chart range");

    m_yStaticRange = new QCheckBox("Manual wavelength range:");
    m_yRangeMin = new QLineEdit();
    connect(m_yStaticRange, &QCheckBox::toggled, m_yRangeMin, &QLineEdit::setEnabled);
    m_yRangeMin->setEnabled(false);
    m_yRangeMin->setPlaceholderText("Min");
    m_yRangeMin->setValidator(new QDoubleValidator());
    m_yRangeMax = new QLineEdit();
    connect(m_yStaticRange, &QCheckBox::toggled, m_yRangeMax, &QLineEdit::setEnabled);
    m_yRangeMax->setEnabled(false);
    m_yRangeMax->setPlaceholderText("Max");
    m_yRangeMax->setValidator(new QDoubleValidator());

    QLabel *samplingLabel = new QLabel("Time series sampling (ms):");
    m_samplingInput = new QLineEdit(QString("%1").arg(TIME_SAMPLING));
    m_samplingInput->setValidator(new QIntValidator());

    QLabel *rangeLabel = new QLabel("Time series range (s):");
    m_rangeInput = new QLineEdit(QString("%1").arg(TIME_WINDOW));
    m_rangeInput->setValidator(new QIntValidator());

    m_showGrid = new QCheckBox("Show chart grid");

    QGridLayout *table = new QGridLayout();
    table->addWidget(m_dynamicRange, 0, 0, 1, 2);
    table->addWidget(m_yStaticRange, 1, 0, 1, 2);
    table->addWidget(m_yRangeMin, 2, 0);
    table->addWidget(m_yRangeMax, 2, 1);
    table->addWidget(samplingLabel, 3, 0);
    table->addWidget(m_samplingInput, 3, 1);
    table->addWidget(rangeLabel, 4, 0);
    table->addWidget(m_rangeInput, 4, 1);
    table->addWidget(m_showGrid, 5, 0);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("Apply");
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(table);
    layout->addSpacing(15);
    layout->addWidget(buttons);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(layout);
}

void MainOptionsDialog::accept()
{
    QDialog::accept();

    auto dynamic = m_dynamicRange->isChecked();
    auto sampling = m_samplingInput->text().toLong();
    auto range = m_rangeInput->text().toLong();
    auto manual = m_yStaticRange->isChecked();
    auto min = m_yRangeMin->text().toDouble();
    auto max = m_yRangeMax->text().toDouble();
    auto grid = m_showGrid->isChecked();

    g_settings->savePeakOptions(sampling, range, dynamic, manual, min, max, grid);

    emit dynamicRangeSet(dynamic);
    emit samplingChanged(sampling);
    emit rangeChanged(range * 1000);
    emit yManualRangeSet(manual);
    emit yRangeChanged(min, max);
    emit gridSet(grid);
}

void MainOptionsDialog::loadSettings()
{
    auto options = g_settings->getPeakOptions();
    if(options.count() > 0)
    {
        m_samplingInput->setText(QString("%1").arg(options[0].toLongLong()));
        m_rangeInput->setText(QString("%1").arg(options[1].toLongLong()));
        m_dynamicRange->setChecked(options[2].toBool());
        m_yStaticRange->setChecked(options[3].toBool());
        m_yRangeMin->setText(QString("%1").arg(options[4].toDouble()));
        m_yRangeMax->setText(QString("%1").arg(options[5].toDouble()));
        m_showGrid->setChecked(options[6].toBool());

        emit dynamicRangeSet(m_dynamicRange->isChecked());
        emit samplingChanged(m_samplingInput->text().toLong());
        emit rangeChanged(m_rangeInput->text().toLong() * 1000);
        emit yManualRangeSet(m_yStaticRange->isChecked());
        emit yRangeChanged(m_yRangeMin->text().toDouble(), m_yRangeMax->text().toDouble());
        emit gridSet(m_showGrid->isChecked());
    }
}
