#include "measureoptionsdialog.h"
#include "qdialogbuttonbox.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qvalidator.h"
#include "timechart.h"

MeasureOptionsDialog::MeasureOptionsDialog()
{
    m_sStaticRange = new QCheckBox("Manual strain range:");
    m_sRangeMin = new QLineEdit();
    connect(m_sStaticRange, &QCheckBox::toggled, m_sRangeMin, &QLineEdit::setEnabled);
    m_sRangeMin->setEnabled(false);
    m_sRangeMin->setPlaceholderText("Min");
    m_sRangeMin->setValidator(new QDoubleValidator());
    m_sRangeMax = new QLineEdit();
    connect(m_sStaticRange, &QCheckBox::toggled, m_sRangeMax, &QLineEdit::setEnabled);
    m_sRangeMax->setEnabled(false);
    m_sRangeMax->setPlaceholderText("Max");
    m_sRangeMax->setValidator(new QDoubleValidator());

    m_tStaticRange = new QCheckBox("Manual temperature range:");
    m_tRangeMin = new QLineEdit();
    connect(m_tStaticRange, &QCheckBox::toggled, m_tRangeMin, &QLineEdit::setEnabled);
    m_tRangeMin->setEnabled(false);
    m_tRangeMin->setPlaceholderText("Min");
    m_tRangeMin->setValidator(new QDoubleValidator());
    m_tRangeMax = new QLineEdit();
    connect(m_tStaticRange, &QCheckBox::toggled, m_tRangeMax, &QLineEdit::setEnabled);
    m_tRangeMax->setEnabled(false);
    m_tRangeMax->setPlaceholderText("Max");
    m_tRangeMax->setValidator(new QDoubleValidator());

    QLabel *samplingLabel = new QLabel("Time series sampling (ms):");
    m_samplingInput = new QLineEdit(QString("%1").arg(TIME_SAMPLING));
    m_samplingInput->setValidator(new QIntValidator());

    QLabel *rangeLabel = new QLabel("Time series range (s):");
    m_rangeInput = new QLineEdit(QString("%1").arg(TIME_WINDOW));
    m_rangeInput->setValidator(new QIntValidator());

    m_showGrid = new QCheckBox("Show chart grid");

    QGridLayout *table = new QGridLayout();
    table->addWidget(m_sStaticRange, 0, 0, 1, 2);
    table->addWidget(m_sRangeMin, 1, 0);
    table->addWidget(m_sRangeMax, 1, 1);
    table->addWidget(m_tStaticRange, 2, 0, 1, 2);
    table->addWidget(m_tRangeMin, 3, 0);
    table->addWidget(m_tRangeMax, 3, 1);
    table->addWidget(samplingLabel, 4, 0);
    table->addWidget(m_samplingInput, 4, 1);
    table->addWidget(rangeLabel, 5, 0);
    table->addWidget(m_rangeInput, 5, 1);
    table->addWidget(m_showGrid, 6, 0);

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

void MeasureOptionsDialog::accept()
{
    QDialog::accept();

    auto sampling = m_samplingInput->text().toLong();
    auto range = m_rangeInput->text().toLong();
    auto sManual = m_sStaticRange->isChecked();
    auto sMin = m_sRangeMin->text().toDouble();
    auto sMax = m_sRangeMax->text().toDouble();
    auto tManual = m_tStaticRange->isChecked();
    auto tMin = m_tRangeMin->text().toDouble();
    auto tMax = m_tRangeMax->text().toDouble();
    auto grid = m_showGrid->isChecked();

    g_settings->saveMeasuresOptions(sampling, range, sManual, {sMin, sMax}, tManual, {tMin, tMax}, grid);

    emit samplingChanged(sampling);
    emit rangeChanged(range * 1000);
    emit sManualRangeSet(sManual);
    emit sRangeChanged(sMin, sMax);
    emit tManualRangeSet(tManual);
    emit tRangeChanged(tMin, tMax);
    emit gridSet(grid);
}

void MeasureOptionsDialog::loadSettings()
{
    auto options = g_settings->getMeasuresOptions();
    if(options.count() > 0)
    {
        m_samplingInput->setText(QString("%1").arg(options[0].toLongLong()));
        m_rangeInput->setText(QString("%1").arg(options[1].toLongLong()));
        m_sStaticRange->setChecked(options[2].toBool());
        m_sRangeMin->setText(QString("%1").arg(options[3].toDouble()));
        m_sRangeMax->setText(QString("%1").arg(options[4].toDouble()));
        m_tStaticRange->setChecked(options[5].toBool());
        m_tRangeMin->setText(QString("%1").arg(options[6].toDouble()));
        m_tRangeMax->setText(QString("%1").arg(options[7].toDouble()));
        m_showGrid->setChecked(options[8].toBool());

        emit samplingChanged(m_samplingInput->text().toLong());
        emit rangeChanged(m_rangeInput->text().toLong() * 1000);
        emit sManualRangeSet(m_sStaticRange->isChecked());
        emit sRangeChanged(m_sRangeMin->text().toDouble(), m_sRangeMax->text().toDouble());
        emit tManualRangeSet(m_tStaticRange->isChecked());
        emit tRangeChanged(m_tRangeMin->text().toDouble(), m_tRangeMax->text().toDouble());
        emit gridSet(m_showGrid->isChecked());
    }
}
