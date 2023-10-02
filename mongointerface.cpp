#include "mongointerface.h"
#include "qthread.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/database.hpp>
#include <synchapi.h>

#define LATENCY_FREQUENCY 1000

const mongocxx::instance MongoInterface::instance = mongocxx::instance{};

MongoInterface::MongoInterface()
    : m_dbStatus{status::disconnected}
    , m_lastLatencyCheck{QDateTime::fromMSecsSinceEpoch(0)}
    , m_collection{""}
{
    m_statusTh = QThread::create([this]{this->statusThread();});
    m_streamTh = QThread::create([this]{this->streamThread();});
}

void MongoInterface::statusThread()
{
    while(true)
    {
        m_statusLock.lock();
        if(m_dbStatus == status::disconnected)
        {
            m_statusLock.unlock();
            break;
        }

        status isConnected = status::connecting;
        try {
            m_statusClient.database(m_db).run_command(bsoncxx::from_json("{\"hello\": 1}"));
            isConnected = status::connected;
        } catch(...) {}

        if(m_dbStatus != isConnected)
        {
            changeStatus(isConnected);

            switch (m_dbStatus) {
            case status::connected:
                m_streamTh->start();
                break;
            case status::connecting:
                m_streamTh->wait();
                m_streamTh->deleteLater();
                m_streamTh = QThread::create([this]{this->streamThread();});
                break;
            }
        }
        m_statusLock.unlock();

        Sleep(1000);
    }
}

void MongoInterface::streamThread()
{
    while(m_dbStatus == status::connected)
    {
        try {
            mongocxx::change_stream stream = m_streamClient[m_db].watch();
            //QVector<peak_t> peaks;
            for(bsoncxx::document::view doc : stream)
            {
                if(m_dbStatus != status::connected)
                    return;

                std::string collection = doc["ns"]["coll"].get_utf8().value.to_string();
                if(collection != m_collection)
                    m_collection = collection;

                bsoncxx::document::element obj = doc["fullDocument"];

                std::string type = obj["type"].get_utf8().value.to_string();
                if(type == "peakData")
                {
                    int index = obj["index"].get_int32();
                    int channel = index / 16;
                    int grating = index % 16;
                    qint64 sourceTS = obj["timestamp"].get_int64() / 1000;

                    if(!m_sensors.contains({channel, grating}))
                        fetchConfig();

                    //peaks.append({channel, grating, obj["wavelength"].get_double(), sourceTS});
                    emit peakReceived(channel, grating, obj["wavelength"].get_double(), sourceTS);

                    QDateTime now = QDateTime::currentDateTime();
                    if(m_lastLatencyCheck.toMSecsSinceEpoch() == 0
                        || now.toMSecsSinceEpoch() - m_lastLatencyCheck.toMSecsSinceEpoch() >= LATENCY_FREQUENCY)
                    {
                        emit latencyComputed(now.toMSecsSinceEpoch() - sourceTS);

                        m_lastLatencyCheck = now;
                    }
                }
                else if(type == "config")
                {
                    int channel = obj["channel"].get_int32();
                    int grating = obj["grating"].get_int32();

                    if(!m_sensors.contains({channel, grating}))
                        m_sensors.append({channel, grating});

                    emit configReceived(channel, grating);
                }
            }

            // Peaks are received in reverse temporal order
            /* if(peaks.count() > 0)
                for(int i=peaks.count()-1; i>=0; i--)
                    emit peakReceived(peaks[i].channel, peaks[i].grating, peaks[i].peak, peaks[i].timestamp); */

        } catch (...) {
        }
    }
}

void MongoInterface::connect(QString uri, QString db)
{
    if(m_dbStatus != status::disconnected)
        disconnect();

    try {
        m_uri = mongocxx::uri{uri.toStdString()};
        m_db = db.toStdString();
        m_statusClient = mongocxx::client{m_uri};
        m_streamClient = mongocxx::client{m_uri};
        changeStatus(status::connecting);
        m_statusTh->start();
    } catch (...) {
        emit connectionFailed();
    }
}

void MongoInterface::changeStatus(status status)
{
    m_dbStatus = status;
    emit statusChanged(m_dbStatus);
}

void MongoInterface::fetchConfig()
{
    bsoncxx::document::view_or_value filter =
        bsoncxx::builder::stream::document{}
            << "type" << "config"
        << bsoncxx::builder::stream::finalize;
    mongocxx::cursor res = m_streamClient[m_db][m_collection].find(filter);

    for(auto doc : res)
    {
        int channel = doc["channel"].get_int32();
        int grating = doc["grating"].get_int32();

        if(!m_sensors.contains({channel, grating}))
            m_sensors.append({channel, grating});

        emit configReceived(channel, grating);
    }
}

void MongoInterface::disconnect()
{
    m_statusLock.lock();
    changeStatus(status::disconnected);
    m_statusLock.unlock();
    m_statusTh->wait();
    m_streamTh->wait();
    m_statusTh->deleteLater();
    m_streamTh->deleteLater();

    m_statusTh = QThread::create([this]{this->statusThread();});
    m_streamTh = QThread::create([this]{this->streamThread();});

    emit connectionClosed();
}

MongoInterface::~MongoInterface()
{
    m_statusTh->terminate();
    m_statusTh->wait();

    m_streamTh->terminate();
    m_streamTh->wait();
}


