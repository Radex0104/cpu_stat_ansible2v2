#include "graphmanager.h"
#include <QDebug>
#include <QFile>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QPointF>
#include <numeric>
#include <algorithm>

// Статическая инициализация палитры цветов
const QVector<QColor> GraphManager::s_defaultPalette = {
    QColor(255, 107, 107),   // Красный
    QColor(78, 205, 196),    // Бирюзовый
    QColor(69, 183, 209),    // Голубой
    QColor(150, 206, 180),   // Мятный
    QColor(255, 234, 167),   // Желтый
    QColor(221, 160, 221),   // Фиолетовый
    QColor(152, 216, 200),   // Морская волна
    QColor(247, 176, 94),    // Оранжевый
    QColor(108, 91, 123),    // Темно-фиолетовый
    QColor(249, 213, 110),   // Золотой
    QColor(184, 59, 94),     // Бордовый
    QColor(44, 120, 108)     // Темно-зеленый
};

GraphManager::GraphManager(QObject *parent)
    : QObject(parent)
    , m_currentColorIndex(0)
    , m_globalOpacity(0.7)
{
    resetColorPalette();
}

GraphManager::~GraphManager()
{
    clearAllGraphs();
}

void GraphManager::clearAllGraphs()
{
    m_graphs.clear();
    m_currentColorIndex = 0;
    emit graphDataCleared();
    emit graphDataUpdated();
}

void GraphManager::addGraphData(const GraphData& data)
{
    GraphData newData = data;
    
    if (!newData.color.isValid()) {
        newData.color = generateColor();
    }
    
    if (newData.opacity <= 0 || newData.opacity > 1) {
        newData.opacity = m_globalOpacity;
    }
    
    calculateStatistics(newData);
    
    m_graphs.append(newData);
    emit graphDataAdded(newData);
    emit graphDataUpdated();
}

void GraphManager::addGraphData(const QString& label, const QString& code,
                               const QVector<double>& timestamps,
                               const QVector<double>& values,
                               const QColor& color, double opacity)
{
    GraphData data;
    data.label = label;
    data.code = code;
    data.timestamps = timestamps;
    data.values = values;
    data.color = color;
    data.opacity = opacity;
    
    addGraphData(data);
}

QVector<GraphData> GraphManager::getAllGraphs() const
{
    return m_graphs;
}

int GraphManager::getGraphCount() const
{
    return m_graphs.size();
}

bool GraphManager::hasGraphs() const
{
    return !m_graphs.isEmpty();
}

QVector<MetricStats> GraphManager::getStatistics() const
{
    QVector<MetricStats> stats;
    
    for (const GraphData& graph : m_graphs) {
        stats.append(MetricStats(
            graph.label,
            graph.code,
            graph.minValue,
            graph.maxValue,
            graph.meanValue,
            graph.values.size()
        ));
    }
    
    return stats;
}

bool GraphManager::processPrometheusData(const QJsonDocument& jsonData)
{
    if (jsonData.isNull() || jsonData.isEmpty()) {
        emit errorOccurred("Получены пустые данные");
        return false;
    }
    
    if (!jsonData.isObject()) {
        emit errorOccurred("Некорректный формат JSON");
        return false;
    }
    
    return parsePrometheusData(jsonData.object());
}

bool GraphManager::processPrometheusData(const QString& jsonString)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (doc.isNull()) {
        emit errorOccurred("Ошибка парсинга JSON строки");
        return false;
    }
    
    return processPrometheusData(doc);
}

bool GraphManager::parsePrometheusData(const QJsonObject& root)
{
    clearAllGraphs();
    
    if (!root.contains("status") || root["status"].toString() != "success") {
        QString errorMsg = root["error"].toString("Неизвестная ошибка Prometheus");
        emit errorOccurred(QString("Ошибка Prometheus: %1").arg(errorMsg));
        return false;
    }
    
    QJsonObject dataObj = root["data"].toObject();
    if (!dataObj.contains("result")) {
        emit errorOccurred("Отсутствуют данные result в ответе");
        return false;
    }
    
    QJsonArray results = dataObj["result"].toArray();
    int processedCount = 0;
    
    for (const QJsonValue& resultValue : results) {
        QJsonObject result = resultValue.toObject();
        
        QJsonObject metric = result["metric"].toObject();
        QString label = metric["label"].toString("unknown");
        QString code = metric["code"].toString("");
        
        if (shouldSkipMetric(label)) {
            continue;
        }
        
        QJsonArray values = result["values"].toArray();
        
        QVector<double> timestamps;
        QVector<double> metricValues;
        
        for (const QJsonValue& pointValue : values) {
            QJsonArray point = pointValue.toArray();
            if (point.size() >= 2) {
                double timestamp = point[0].toDouble();
                double value = point[1].toString().toDouble();
                
                timestamps.append(timestamp);
                metricValues.append(value);
            }
        }
        
        if (!timestamps.isEmpty()) {
            addGraphData(label, code, timestamps, metricValues);
            processedCount++;
        }
    }
    
    if (processedCount == 0) {
        emit errorOccurred("Нет данных для отображения после фильтрации");
        return false;
    }
    
    emit graphDataUpdated();
    return true;
}

QVector<GraphSeries> GraphManager::getGraphSeries() const
{
    QVector<GraphSeries> series;
    
    for (const GraphData& data : m_graphs) {
        GraphSeries s;
        s.name = QString("%1 (min: %2 ms, max: %3 ms, mean: %4 ms)")
            .arg(data.label)
            .arg(data.minValue, 0, 'f', 2)
            .arg(data.maxValue, 0, 'f', 2)
            .arg(data.meanValue, 0, 'f', 2);
        s.color = data.color;
        s.minValue = data.minValue;
        s.maxValue = data.maxValue;
        s.meanValue = data.meanValue;
        
        for (int i = 0; i < data.timestamps.size(); ++i) {
            s.points.append(QPointF(data.timestamps[i], data.values[i]));
        }
        
        series.append(s);
    }
    
    return series;
}

void GraphManager::calculateStatistics(GraphData& data)
{
    if (data.values.isEmpty()) {
        data.minValue = 0;
        data.maxValue = 0;
        data.meanValue = 0;
        return;
    }
    
    data.minValue = *std::min_element(data.values.begin(), data.values.end());
    data.maxValue = *std::max_element(data.values.begin(), data.values.end());
    
    double sum = std::accumulate(data.values.begin(), data.values.end(), 0.0);
    data.meanValue = sum / data.values.size();
}

bool GraphManager::shouldSkipMetric(const QString& label)
{
    QStringList skipPatterns = {
        "Transaction",
        "Debug",
        "complex",
        "unknown"
    };
    
    for (const QString& pattern : skipPatterns) {
        if (label.contains(pattern, Qt::CaseInsensitive)) {
            return true;
        }
    }
    
    return false;
}

void GraphManager::setGlobalOpacity(double opacity)
{
    m_globalOpacity = qBound(0.0, opacity, 1.0);
    
    for (GraphData& data : m_graphs) {
        data.opacity = m_globalOpacity;
    }
    
    emit graphDataUpdated();
}

void GraphManager::setColorPalette(const QVector<QColor>& palette)
{
    if (palette.isEmpty()) {
        resetColorPalette();
        return;
    }
    
    m_colorPalette = palette;
    m_currentColorIndex = 0;
    
    for (int i = 0; i < m_graphs.size(); ++i) {
        if (i < m_colorPalette.size()) {
            m_graphs[i].color = m_colorPalette[i];
        } else {
            m_graphs[i].color = generateColor();
        }
    }
    
    emit graphDataUpdated();
}

void GraphManager::resetColorPalette()
{
    m_colorPalette = s_defaultPalette;
    m_currentColorIndex = 0;
}

QColor GraphManager::generateColor()
{
    if (m_colorPalette.isEmpty()) {
        return QColor(
            QRandomGenerator::global()->bounded(256),
            QRandomGenerator::global()->bounded(256),
            QRandomGenerator::global()->bounded(256)
        );
    }
    
    QColor color = m_colorPalette[m_currentColorIndex % m_colorPalette.size()];
    m_currentColorIndex++;
    return color;
}

bool GraphManager::exportToJson(const QString& filePath) const
{
    QJsonArray graphsArray;
    
    for (const GraphData& data : m_graphs) {
        QJsonObject graphObj;
        graphObj["label"] = data.label;
        graphObj["code"] = data.code;
        graphObj["opacity"] = data.opacity;
        graphObj["minValue"] = data.minValue;
        graphObj["maxValue"] = data.maxValue;
        graphObj["meanValue"] = data.meanValue;
        
        QJsonObject colorObj;
        colorObj["red"] = data.color.red();
        colorObj["green"] = data.color.green();
        colorObj["blue"] = data.color.blue();
        graphObj["color"] = colorObj;
        
        QJsonArray timestampsArray;
        QJsonArray valuesArray;
        
        for (double ts : data.timestamps) {
            timestampsArray.append(ts);
        }
        for (double val : data.values) {
            valuesArray.append(val);
        }
        
        graphObj["timestamps"] = timestampsArray;
        graphObj["values"] = valuesArray;
        
        graphsArray.append(graphObj);
    }
    
    QJsonDocument doc(graphsArray);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Не удалось открыть файл для записи:" << filePath;
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    return true;
}

bool GraphManager::importFromJson(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred(QString("Не удалось открыть файл для чтения: %1").arg(filePath));
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        emit errorOccurred("Некорректный формат JSON файла");
        return false;
    }
    
    clearAllGraphs();
    
    QJsonArray graphsArray = doc.array();
    for (const QJsonValue& value : graphsArray) {
        QJsonObject graphObj = value.toObject();
        
        GraphData graphData;
        graphData.label = graphObj["label"].toString();
        graphData.code = graphObj["code"].toString();
        graphData.opacity = graphObj["opacity"].toDouble(0.7);
        graphData.minValue = graphObj["minValue"].toDouble(0);
        graphData.maxValue = graphObj["maxValue"].toDouble(0);
        graphData.meanValue = graphObj["meanValue"].toDouble(0);
        
        QJsonObject colorObj = graphObj["color"].toObject();
        graphData.color = QColor(
            colorObj["red"].toInt(),
            colorObj["green"].toInt(),
            colorObj["blue"].toInt()
        );
        
        QJsonArray timestampsArray = graphObj["timestamps"].toArray();
        QJsonArray valuesArray = graphObj["values"].toArray();
        
        for (const QJsonValue& ts : timestampsArray) {
            graphData.timestamps.append(ts.toDouble());
        }
        for (const QJsonValue& val : valuesArray) {
            graphData.values.append(val.toDouble());
        }
        
        if (!graphData.timestamps.isEmpty()) {
            m_graphs.append(graphData);
        }
    }
    
    emit graphDataUpdated();
    return true;
}

#include "moc_graphmanager.cpp"