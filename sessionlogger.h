#ifndef SESSIONLOGGER_H
#define SESSIONLOGGER_H

#include "customstatusbar.h"
#include "mongointerface.h"
#include "qdir.h"

extern MongoInterface g_db;
extern CustomStatusBar *g_statusBar;

class SessionLogger : public QObject
{
    Q_OBJECT
public:
    SessionLogger();
    ~SessionLogger();

private:
    QDir m_dir;
    QFile *m_file;
    long m_sampling; //ms
    long m_duration; //s
    QMap<QPair<int,int>,qreal> m_peaks;
    QMap<QPair<int,int>,qreal> m_samples;
    int m_sample;
    qint64 m_sampleTS;

    void writeRow();
    void addSample(QPair<int,int> sensor, qreal peak);

public slots:
    void start();
    void stop();
    void changeSettings(long sampling, long duration, QList<QPair<int, int>> sensors);
    void updatePeak(int channel, int grating, qreal peak, qint64 timestamp);
};

#endif // SESSIONLOGGER_H
