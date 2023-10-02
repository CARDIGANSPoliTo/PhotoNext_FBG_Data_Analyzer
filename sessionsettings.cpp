#include "sessionsettings.h"
#include "qcoreapplication.h"
#include "qvariant.h"

#define GRADIENT_RANGE 0.70

SessionSettings::SessionSettings()
    : m_file(QCoreApplication::applicationDirPath() + "/settings.json")
    , m_strViews({"peakTimeView", "measuresView", "fftView"})
{
    loadSettings();
}

SessionSettings::~SessionSettings()
{
    writeSettings();
}

void SessionSettings::loadSettings()
{
    try
    {
        m_file.open(QIODevice::ReadOnly);
        auto jsonStr = m_file.readAll().toStdString();
        m_settings = nlohmann::json::parse(jsonStr);
        m_file.close();
    }
    catch(...)
    {
        m_file.close();
        m_settings = nlohmann::json::parse("{}");
    }
}

void SessionSettings::writeSettings()
{
    m_file.open(QIODevice::WriteOnly);
    m_file.write(m_settings.dump().c_str());
    m_file.close();
}

void SessionSettings::saveDbInfo(QString host, int port, QString db)
{
    auto &connection = m_settings["connection"];
    connection["host"] = host.toStdString();
    connection["port"] = port;
    connection["database"] = db.toStdString();
}

void SessionSettings::saveDbAuth(QString username, QString password, QString authDb)
{
    auto &connection = m_settings["connection"];
    connection["username"] = username.toStdString();
    connection["password"] = password.toStdString();
    connection["authDatabase"] = authDb.toStdString();
}

void SessionSettings::saveDbConnection(bool direct, bool ssl)
{
    auto &connection = m_settings["connection"];
    connection["direct"] = direct;
    connection["ssl"] = ssl;
}

void SessionSettings::savePeakOptions(long sampling, long range, bool dynamic, bool yRange, qreal min, qreal max, bool grid)
{
    auto &options = m_settings["peakTimeView"]["options"];
    options["sampling"] = sampling;
    options["range"] = range;
    options["dynamicRange"] = dynamic;
    options["yStaticRange"] = yRange;
    options["yMin"] = min;
    options["yMax"] = max;
    options["showGrid"] = grid;
}

void SessionSettings::saveMeasuresOptions(long sampling, long range,
                                          bool sRange, QPair<qreal,qreal> sMinMax, bool tRange, QPair<qreal,qreal> tMinMax, bool grid)
{
    auto &options = m_settings["measuresView"]["options"];
    options["sampling"] = sampling;
    options["range"] = range;
    options["sStaticRange"] = sRange;
    options["sMin"] = sMinMax.first;
    options["sMax"] = sMinMax.second;
    options["tStaticRange"] = tRange;
    options["tMin"] = tMinMax.first;
    options["tMax"] = tMinMax.second;
    options["showGrid"] = grid;
}

void SessionSettings::saveFFTOptions(long sampling, long window, bool dc, bool grid)
{
    auto &options = m_settings["fftView"]["options"];
    options["sampling"] = sampling;
    options["window"] = window;
    options["plotDC"] = dc;
    options["showGrid"] = grid;
}

void SessionSettings::saveSensorColor(int channel, int grating, QColor color)
{
    auto &colors = m_settings["colors"];
    auto strCh = QString("%1").arg(channel).toStdString();
    auto strGr = QString("%1").arg(grating).toStdString();
    //colors[strCh][strGr] = {color.red(), color.green(), color.blue()};
    colors[strCh] = {color.red(), color.green(), color.blue()};

    float step = m_gratings[channel] > 1 ? GRADIENT_RANGE / (m_gratings[channel] - 1) : 0;
    for(int gr=0; gr<m_gratings[channel]; gr++)
    {
        float p = 1 - step * gr;
        int r = color.red() * p + 255 * (1 - p);
        int g = color.green() * p + 255 * (1 - p);
        int b = color.blue() * p + 255 * (1 - p);
        emit sensorColorChanged(channel, gr, QColor(r, g, b), color);
    }
}

void SessionSettings::saveSensorVisibility(viewEnum view, int channel, int grating, bool state)
{
    auto strCh = QString("%1").arg(channel).toStdString();
    auto strGr = QString("%1").arg(grating).toStdString();
    auto &sensors = m_settings[m_strViews[view]]["sensorsVisibility"];
    sensors[strCh][strGr] = state;
}

void SessionSettings::saveAcquisitionSettings(long sampling, long duration, QVector<QPair<int, int>> sensors)
{
    auto &acquisition = m_settings["acquisition"];
    acquisition["sampling"] = sampling;
    acquisition["duration"] = duration;
    acquisition["sensors"] = sensors;
}

void SessionSettings::saveStrainSensor(int channel, int grating, qreal k, qreal s0, qreal sPeak, QPair<int, int> tSensor, qreal tPeak)
{
    auto strCh = QString("%1").arg(channel).toStdString();
    auto strGr = QString("%1").arg(grating).toStdString();
    auto &sensor = m_settings[m_strViews[MeasuresView]]["strains"][strCh][strGr];
    sensor["k"] = k;
    sensor["initialStrain"] = s0;
    sensor["initialSPeak"] = sPeak;
    if(tSensor.first != -1)
    {
        sensor["tempSensor"] = tSensor;
        sensor["initialTPeak"] = tPeak;
    }
}

void SessionSettings::saveTempSensor(int channel, int grating, qreal k, qreal t0, qreal iPeak)
{
    auto strCh = QString("%1").arg(channel).toStdString();
    auto strGr = QString("%1").arg(grating).toStdString();
    auto &sensor = m_settings[m_strViews[MeasuresView]]["temperatures"][strCh][strGr];
    sensor["k"] = k;
    sensor["initialTemp"] = t0;
    sensor["initialPeak"] = iPeak;
}

QVector<QVariant> SessionSettings::getDbInfo()
{
    try
    {
        auto &connection = m_settings["connection"];
        auto host = QString(connection["host"].get<std::string>().c_str());
        auto port = connection["port"].get<int>();
        auto database = QString(connection["database"].get<std::string>().c_str());
        return { host, port, database };
    }
    catch(...)
    {
        return {};
    }
}

QVector<QVariant> SessionSettings::getDbAuth()
{
    try
    {
        auto &connection = m_settings["connection"];
        auto user = QString(connection["username"].get<std::string>().c_str());
        auto pass = QString(connection["password"].get<std::string>().c_str());
        auto db = QString(connection["authDatabase"].get<std::string>().c_str());
        return { user, pass, db };
    }
    catch(...)
    {
        return {};
    }
}

QVector<QVariant> SessionSettings::getDbConnection()
{
    try
    {
        auto &connection = m_settings["connection"];
        auto direct = connection["direct"].get<bool>();
        auto ssl = connection["ssl"].get<bool>();
        return { direct, ssl };
    }
    catch(...)
    {
        return {};
    }
}

QVector<QVariant> SessionSettings::getPeakOptions()
{
    try
    {
        auto &options = m_settings["peakTimeView"]["options"];
        auto sampling = options["sampling"].get<long>();
        auto range = options["range"].get<long>();
        auto dynamic = options["dynamicRange"].get<bool>();
        auto yStaticRange = options["yStaticRange"].get<bool>();
        auto yMin = options["yMin"].get<double>();
        auto yMax = options["yMax"].get<double>();
        auto grid = options["showGrid"].get<bool>();
        return { (qint64)sampling, (qint64)range, dynamic, yStaticRange, yMin, yMax, grid };
    }
    catch(...)
    {
        return {};
    }
}

QVector<QVariant> SessionSettings::getMeasuresOptions()
{
    try
    {
        auto &options = m_settings["measuresView"]["options"];
        auto sampling = options["sampling"].get<long>();
        auto range = options["range"].get<long>();
        auto sStaticRange = options["sStaticRange"].get<bool>();
        auto sMin = options["sMin"].get<double>();
        auto sMax = options["sMax"].get<double>();
        auto tStaticRange = options["tStaticRange"].get<bool>();
        auto tMin = options["tMin"].get<double>();
        auto tMax = options["tMax"].get<double>();
        auto grid = options["showGrid"].get<bool>();
        return { (qint64)sampling, (qint64)range, sStaticRange, sMin, sMax, tStaticRange, tMin, tMax, grid };
    }
    catch(...)
    {
        return {};
    }
}

QVector<QVariant> SessionSettings::getFFTOptions()
{
    try
    {
        auto &options = m_settings["fftView"]["options"];
        auto sampling = options["sampling"].get<long>();
        auto window = options["window"].get<long>();
        auto dc = options["plotDC"].get<bool>();
        auto grid = options["showGrid"].get<bool>();
        return { (qint64)sampling, (qint64)window, dc, grid };
    }
    catch(...)
    {
        return {};
    }
}

QColor SessionSettings::getSensorColor(int channel, int grating)
{
    if(!m_gratings.contains(channel))
        m_gratings[channel] = grating + 1;
    else if(grating >= m_gratings[channel])
        m_gratings[channel] = grating + 1;

    try
    {
        auto strCh = QString("%1").arg(channel).toStdString();
        auto strGr = QString("%1").arg(grating).toStdString();
        //auto &color = m_settings["colors"][strCh][strGr];
        auto &color = m_settings["colors"][strCh];

        QVector<int> rgb;
        for(int i=0;i<3;i++)
            rgb.append(color[i].get<int>());

        return QColor(rgb[0], rgb[1], rgb[2]);
    }
    catch(...)
    {
        return QColor::Invalid;
    }
}

bool SessionSettings::getSensorVisibility(viewEnum view, int channel, int grating)
{
    try
    {
        auto strCh = QString("%1").arg(channel).toStdString();
        auto strGr = QString("%1").arg(grating).toStdString();
        auto &sensors = m_settings[m_strViews[view]]["sensorsVisibility"];
        auto &sensor = sensors[strCh][strGr];
        return sensor.get<bool>();
    }
    catch(...)
    {
        return -1;
    }
}

QVector<QVariant> SessionSettings::getAcquisitionSettings()
{
    try
    {
        auto &acquisition = m_settings["acquisition"];
        auto sampling = acquisition["sampling"].get<long>();
        auto duration = acquisition["duration"].get<long>();
        auto sensors = acquisition["sensors"].get<QVector<QPair<int, int>>>();
        return { (quint64)sampling, (quint64)duration, QVariant::fromValue(sensors) };
    }
    catch(...)
    {
        return {};
    }
}

QVector<QVariant> SessionSettings::getMeasureSensor(int channel, int grating)
{
    auto strCh = QString("%1").arg(channel).toStdString();
    auto strGr = QString("%1").arg(grating).toStdString();

    auto &strains = m_settings[m_strViews[MeasuresView]]["strains"];
    if(strains.contains(strCh)){
        if(strains[strCh].contains(strGr))
        {
            auto &sensor = strains[strCh][strGr];
            auto k = sensor["k"].get<qreal>();
            auto s0 = sensor["initialStrain"].get<qreal>();
            auto sPeak = sensor["initialSPeak"].get<qreal>();
            if(sensor.contains("tempSensor"))
            {
                auto tSensor = sensor["tempSensor"].get<QPair<int,int>>();
                auto tPeak = sensor["initialTPeak"].get<qreal>();
                return { Strain, k, s0, sPeak, QVariant::fromValue(tSensor), tPeak };
            }
            else
                return { Strain, k, s0, sPeak };
        }
    }

    auto &temps = m_settings[m_strViews[MeasuresView]]["temperatures"];
    if(temps.contains(strCh)){
        if(temps[strCh].contains(strGr))
        {
            auto &sensor = temps[strCh][strGr];
            auto k = sensor["k"].get<qreal>();
            auto t0 = sensor["initialTemp"].get<qreal>();
            auto iPeak = sensor["initialPeak"].get<qreal>();
            return { Temperature, k, t0, iPeak };
        }
    }

    return {};
}

void SessionSettings::deleteMeasureSensor(int channel, int grating)
{
    auto strCh = QString("%1").arg(channel).toStdString();
    auto strGr = QString("%1").arg(grating).toStdString();

    try { m_settings[m_strViews[MeasuresView]]["strains"][strCh].erase(strGr); }
    catch(...){}

    try { m_settings[m_strViews[MeasuresView]]["temperatures"][strCh].erase(strGr); }
    catch(...){}
}
