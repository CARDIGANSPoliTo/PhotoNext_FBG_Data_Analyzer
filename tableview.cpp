#include "tableview.h"
#include "qevent.h"
#include "qheaderview.h"
#include "qlabel.h"
#include <QVBoxLayout>

TableView::TableView(QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    m_table = new QTableWidget(16, 4);
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
        "QTableWidget::item{"
            "padding-left:5px;"
            "padding-right:5px;"
        "}"
    );

    // Labels
    QStringList list;
    for(int i=0; i<4; i++)
        list.append("Channel " + QString::number(i));
    m_table->setHorizontalHeaderLabels(list);
    list.clear();
    for(int i=0; i<16; i++)
        list.append(QString("Grating %1").arg(i, 2, 10, QChar('0')));
    m_table->setVerticalHeaderLabels(list);

    // Main layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(m_table);
    this->setLayout(layout);

    connect(&g_db, &MongoInterface::peakReceived, this, &TableView::updatePeak);
}

void TableView::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    event->accept();
}

void TableView::updatePeak(int channel, int grating, qreal peak)
{
    QLabel *label = (QLabel*)m_table->cellWidget(grating, channel);

    if(label == nullptr)
    {
        m_table->setCellWidget(grating, channel, new QLabel(QString("%1").arg(peak)));
    }
    else
    {
        label->setText(QString("%1").arg(peak));
    }
}

