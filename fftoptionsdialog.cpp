#include "fftoptionsdialog.h"
#include "qdialogbuttonbox.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "fftchart.h"
#include "qpushbutton.h"
#include "qvalidator.h"
#include "./ffft/FFTReal.h"

FFTOptionsDialog::FFTOptionsDialog()
{
    m_dc = new QCheckBox("Plot direct component (f[0])");

    QLabel *samplingLabel = new QLabel("Sampling (ms):");
    m_samplingInput = new QLineEdit(QString("%1").arg(FFT_SAMPLING));
    m_samplingInput->setValidator(new QIntValidator());

    QLabel *samplesLabel = new QLabel("Samples (2^n):");
    m_samplesInput = new QLineEdit(QString("%1").arg(SAMPLING_WINDOW));
    m_samplesInput->setValidator(new QIntValidator());

    m_showGrid = new QCheckBox("Show chart grid");

    QGridLayout *table = new QGridLayout();
    table->addWidget(m_dc, 0, 0, 1, 2);
    table->addWidget(samplingLabel, 1, 0);
    table->addWidget(m_samplingInput, 1, 1);
    table->addWidget(samplesLabel, 2, 0);
    table->addWidget(m_samplesInput, 2, 1);
    table->addWidget(m_showGrid, 3, 0);

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

void FFTOptionsDialog::accept()
{
    auto window = m_samplesInput->text().toLong();
    if(!ffft::FFTReal_is_pow2(window))
    {
        g_statusBar->showMessage("The window is not a power of 2", 3000);
        return;
    }

    QDialog::accept();

    auto dc = m_dc->isChecked();
    auto sampling = m_samplingInput->text().toLong();
    auto grid = m_showGrid->isChecked();

    g_settings->saveFFTOptions(sampling, window, dc, grid);

    emit optionsChanged(dc, sampling, window);
    emit gridSet(grid);
}

void FFTOptionsDialog::loadSettings()
{
    auto options = g_settings->getFFTOptions();
    if(options.count() > 0)
    {
        m_samplingInput->setText(QString("%1").arg(options[0].toLongLong()));
        m_samplesInput->setText(QString("%1").arg(options[1].toLongLong()));
        m_dc->setChecked(options[2].toBool());
        m_showGrid->setChecked(options[3].toBool());

        emit optionsChanged(m_dc->isChecked(), m_samplingInput->text().toLong(), m_samplesInput->text().toLong());
        emit gridSet(m_showGrid->isChecked());
    }
}
