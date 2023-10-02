#include "sessionlogger.h"
#include "qcoreapplication.h"
#include "qthread.h"
#include <synchapi.h>

SessionLogger::SessionLogger()
    : m_dir(QCoreApplication::applicationDirPath() + "/acquisitions")
    , m_file(nullptr)
    , m_sampling(40)
    , m_duration(60)
    , m_sample(0)
    , m_sampleTS(std::numeric_limits<qint64>::max())
{
    if(!m_dir.exists())
        m_dir.mkpath(".");

    connect(&g_db, &MongoInterface::peakReceived, this, &SessionLogger::updatePeak);
}

SessionLogger::~SessionLogger()
{
    if(m_file)
        m_file->close();
}

void SessionLogger::start()
{
    QString fileName = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.log");
    m_file = new QFile(m_dir.absoluteFilePath(fileName));
    m_file->open(QIODevice::WriteOnly);

    QString start = QString("Start Time = %1\n").arg(QDateTime::currentMSecsSinceEpoch());
    m_file->write(start.toStdString().c_str());
    m_file->write(QString("Time interval = %1 ms\n\n").arg(m_sampling).toStdString().c_str());

    QString header("Time\t");
    for(auto sensor : m_peaks.keys())
    {
        QString name = QString("Ch%1Gr%2")
                .arg(sensor.first, 2, 10, QChar('0'))
                .arg(sensor.second, 2, 10, QChar('0'));
        header.append(name + "\t");
    }
    header.append("\n\t");
    for(auto sensor : m_peaks.keys())
        header.append("Wavelength (nm)\t");
    header.append("\n");
    m_file->write(header.toStdString().c_str());

    m_sampleTS = QDateTime::currentMSecsSinceEpoch();

    g_statusBar->updateLogStatus(true);
}

void SessionLogger::stop()
{
    m_sample = 0;
    m_sampleTS = std::numeric_limits<qint64>::max();
    if(m_file)
        m_file->close();
    g_statusBar->updateLogStatus(false);
}

void SessionLogger::changeSettings(long sampling, long duration, QList<QPair<int, int>> sensors)
{
    stop();
    m_sampling = sampling;
    m_duration = duration;
    m_peaks.clear();
    for(auto sensor : sensors)
    {
        m_peaks[sensor] = 0;
    }
}

void SessionLogger::writeRow()
{
    QString row(QString::number(m_sampling * m_sample++ / 1000.0, 'f', 6) + "\t");
    for(auto peak : m_samples)
        row.append(QString::number(peak, 'f', 4) + "\t");
    row.append("\n");
    m_file->write(row.toStdString().c_str());

    m_samples.clear();
    m_sampleTS += m_sampling;

    if(m_sampling * m_sample > m_duration*1000)
        stop();
}

void SessionLogger::addSample(QPair<int,int> sensor, qreal peak)
{
    if(m_samples.contains(sensor))
    {
        for(auto s : m_peaks.keys())
        {
            if(!m_samples.contains(s))
                m_samples[s] = m_peaks[s];
        }
        writeRow();
    }

    m_samples[sensor] = peak;

    if(m_samples.count() == m_peaks.count())
        writeRow();
}

void SessionLogger::updatePeak(int channel, int grating, qreal peak, qint64 timestamp)
{
    QPair<int, int> sensor = {channel, grating};
    if(!m_peaks.contains(sensor) || m_sampleTS == std::numeric_limits<qint64>::max())
        return;

    if((timestamp > m_sampleTS && !m_samples.contains(sensor)) || timestamp > m_sampleTS + m_sampling)
    {
        addSample(sensor, m_peaks[sensor]);
    }
    else if(timestamp == m_sampleTS || timestamp == m_sampleTS + m_sampling)
    {
        addSample(sensor, peak);
    }

    m_peaks[sensor] = peak;
}
