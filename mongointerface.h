#ifndef MONGOINTERFACE_H
#define MONGOINTERFACE_H

#include "qdatetime.h"
#include "qmutex.h"
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <string>
#include <QObject>

class MongoInterface : public QObject
{
    Q_OBJECT
public:
    MongoInterface();
    ~MongoInterface();

    enum status { disconnected, connecting, connected };

private:
    typedef struct {
        int channel, grating;
        qreal peak;
        qint64 timestamp;
    } peak_t;
    static const mongocxx::instance instance;

    mongocxx::uri m_uri;
    std::string m_db;
    std::string m_collection;
    mongocxx::client m_statusClient;
    mongocxx::client m_streamClient;
    status m_dbStatus;
    QThread *m_statusTh;
    QThread *m_streamTh;
    QMutex m_statusLock;
    QDateTime m_lastLatencyCheck;
    QVector<QPair<int,int>> m_sensors;

    void statusThread();
    void streamThread();
    void changeStatus(status status);
    void fetchConfig();

public slots:
    void connect(QString uri, QString db);
    void disconnect();

signals:
    void statusChanged(MongoInterface::status status);
    void peakReceived(int channel, int grating, qreal peak, qint64 timestamp);
    void connectionFailed();
    void configReceived(int channel, int grating);
    void latencyComputed(qint64 latency);
    void connectionClosed();
};

#endif // MONGOINTERFACE_H
