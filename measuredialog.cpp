#include "measuredialog.h"
#include "qdialogbuttonbox.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "qlineedit.h"

MeasureDialog::MeasureDialog(QVector<QPair<int, int>> sensors)
{
    QVector<QPair<QString, QPair<int, int>>> sensorList;
    for(auto sensor : sensors)
    {
        QString name = QString("Ch%1Gr%2")
                .arg(sensor.first, 2, 10, QChar('0'))
                .arg(sensor.second, 2, 10, QChar('0'));
        sensorList.append({name, sensor});
    }

    QLabel *fbgLabel = new QLabel("Sensor:");
    m_fbgCombo = new QComboBox();

    QLabel *k1Label = new QLabel("Kt: ");
    m_k1Input = new QLineEdit();
    m_k1Input->setValidator(new QDoubleValidator());
    QLabel *t0Label = new QLabel("T0: ");
    m_t0Input = new QLineEdit();
    m_t0Input->setValidator(new QDoubleValidator());

    QLabel *k2Label = new QLabel("Ke: ");
    m_k2Input = new QLineEdit();
    m_k2Input->setValidator(new QDoubleValidator());
    k2Label->setVisible(false);
    m_k2Input->setVisible(false);
    QLabel *s0Label = new QLabel("S0: ");
    m_s0Input = new QLineEdit();
    m_s0Input->setValidator(new QDoubleValidator());
    s0Label->setVisible(false);
    m_s0Input->setVisible(false);

    QLabel *tmpLabel = new QLabel("T Sensor:");
    m_tmpCombo = new QComboBox();
    m_tmpCombo->addItem("N/A");
    tmpLabel->setVisible(false);
    m_tmpCombo->setVisible(false);

    QLabel *typeLabel = new QLabel("Type:");
    m_typeCombo = new QComboBox();
    m_typeCombo->addItems({"Temperature", "Strain"});
    connect(m_typeCombo, &QComboBox::currentIndexChanged, [=](int type){
        switch(type)
        {
        case type::temperature:
            k1Label->setVisible(true);
            m_k1Input->setVisible(true);
            t0Label->setVisible(true);
            m_t0Input->setVisible(true);

            k2Label->setVisible(false);
            m_k2Input->setVisible(false);
            s0Label->setVisible(false);
            m_s0Input->setVisible(false);

            tmpLabel->setVisible(false);
            m_tmpCombo->setVisible(false);

            break;
        case type::strain:
            k1Label->setVisible(false);
            m_k1Input->setVisible(false);
            t0Label->setVisible(false);
            m_t0Input->setVisible(false);

            k2Label->setVisible(true);
            m_k2Input->setVisible(true);
            s0Label->setVisible(true);
            m_s0Input->setVisible(true);

            tmpLabel->setVisible(true);
            m_tmpCombo->setVisible(true);

            break;
        }
    });

    for(auto sensor : sensorList)
    {
        m_fbgCombo->addItem(sensor.first, QVariant::fromValue(sensor.second));
        m_tmpCombo->addItem(sensor.first, QVariant::fromValue(sensor.second));
    }

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QGridLayout *table = new QGridLayout();
    table->addWidget(fbgLabel, 0, 0);
    table->addWidget(m_fbgCombo, 0, 1);
    table->addWidget(typeLabel, 1, 0);
    table->addWidget(m_typeCombo, 1, 1);
    table->addWidget(k1Label, 2, 0);
    table->addWidget(m_k1Input, 2, 1);
    table->addWidget(m_t0Input, 3, 1);
    table->addWidget(t0Label, 3, 0);
    table->addWidget(k2Label, 4, 0);
    table->addWidget(m_k2Input, 4, 1);
    table->addWidget(s0Label, 5, 0);
    table->addWidget(m_s0Input, 5, 1);
    table->addWidget(tmpLabel, 6, 0);
    table->addWidget(m_tmpCombo, 6, 1);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(table);
    layout->addSpacing(15);
    layout->addWidget(buttons);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(layout);
}

void MeasureDialog::accept()
{
    try
    {
        if(m_fbgCombo->currentText() != "")
        {
            auto sensor = m_fbgCombo->currentData().value<QPair<int, int>>();
            QPair<int, int> tSensor = {-1, -1};
            switch(m_typeCombo->currentIndex())
            {
            case type::temperature:
                if(m_k1Input->text() != "" && m_t0Input->text() != "")
                    QDialog::accept();
                else throw std::exception{};

                emit temperatureAdded(sensor.first, sensor.second, m_k1Input->text().toDouble(), m_t0Input->text().toDouble());
                break;
            case type::strain:
                if(m_k2Input->text() != "" && m_s0Input->text() != "")
                    QDialog::accept();
                else throw std::exception{};

                if(m_tmpCombo->currentText() != "N/A")
                    tSensor = m_tmpCombo->currentData().value<QPair<int, int>>();

                emit strainAdded(sensor.first, sensor.second, m_k2Input->text().toDouble(), m_s0Input->text().toDouble(), tSensor);
                break;
            }
        }
        else throw std::exception{};
    }
    catch(...)
    {
        g_statusBar->showMssingParamMessage();
    }
}
