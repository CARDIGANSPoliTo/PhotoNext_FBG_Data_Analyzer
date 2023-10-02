#ifndef SESSIONSETTINGS_H
#define SESSIONSETTINGS_H

#include "json.hpp"
#include "qcoreapplication.h"
#include "qfile.h"
#include "qcolor.h"

class SessionSettings : public QObject
{
    Q_OBJECT
public:
    enum viewEnum { PeakTimeView, MeasuresView, FFTView };
    enum sensorEnum { Strain, Temperature };

    SessionSettings();
    ~SessionSettings();

    void saveDbInfo(QString host, int port, QString db);
    void saveDbAuth(QString username, QString password, QString authDb = "");
    void saveDbConnection(bool direct, bool ssl);
    void savePeakOptions(long sampling, long range, bool dynamic, bool yRange, qreal min, qreal max, bool grid);
    void saveMeasuresOptions(long sampling, long range, bool sRange, QPair<qreal,qreal> sMinMax, bool tRange, QPair<qreal,qreal> tMinMax, bool grid);
    void saveFFTOptions(long sampling, long window, bool dc, bool grid);
    void saveSensorColor(int channel, int grating, QColor color);
    void saveSensorVisibility(viewEnum view, int channel, int grating, bool state);
    void saveAcquisitionSettings(long sampling, long duration, QVector<QPair<int, int>> sensors);
    void saveStrainSensor(int channel, int grating, qreal k, qreal s0, qreal iPeak, QPair<int, int> tSensor = {-1,-1}, qreal tPeak = -1);
    void saveTempSensor(int channel, int grating, qreal k, qreal t0, qreal iPeak);

    QVector<QVariant> getDbInfo();
    QVector<QVariant> getDbAuth();
    QVector<QVariant> getDbConnection();
    QVector<QVariant> getPeakOptions();
    QVector<QVariant> getMeasuresOptions();
    QVector<QVariant> getFFTOptions();
    QColor getSensorColor(int channel, int grating);
    bool getSensorVisibility(viewEnum view, int channel, int grating);
    QVector<QVariant> getAcquisitionSettings();
    QVector<QVariant> getMeasureSensor(int channel, int grating);
    
    void deleteMeasureSensor(int channel, int grating);

private:
    nlohmann::json m_settings;
    QFile m_file;
    QVector<std::string> m_strViews;
    QMap<int,int> m_gratings;

    void loadSettings();
    void writeSettings();

signals:
    void sensorColorChanged(int channel, int grating, QColor color, QColor src);
};

#endif // SESSIONSETTINGS_H
