#include "prometheus.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>

PrometheusProcessor::PrometheusProcessor(QObject *parent)
    : QObject(parent)
{
}

bool PrometheusProcessor::processJsonData(const QByteArray& jsonData, GraphManager* graphManager)
{
    if (!graphManager) {
        emit errorOccurred("GraphManager не инициализирован");
        return false;
    }
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        QString error = QString("Ошибка парсинга JSON: %1").arg(parseError.errorString());
        emit errorOccurred(error);
        return false;
    }
    
    if (doc.isNull() || doc.isEmpty()) {
        emit errorOccurred("Получены пустые данные");
        return false;
    }
    
    if (!doc.isObject()) {
        emit errorOccurred("Некорректный формат JSON");
        return false;
    }
    
    return parsePrometheusData(doc.object(), graphManager);
}

bool PrometheusProcessor::processJsonData(const QString& jsonString, GraphManager* graphManager)
{
    return processJsonData(jsonString.toUtf8(), graphManager);
}

bool PrometheusProcessor::parsePrometheusData(const QJsonObject& root, GraphManager* graphManager)
{
    // Очищаем старые данные
    graphManager->clearAllGraphs();
    
    // Проверяем статус ответа
    if (!root.contains("status") || root["status"].toString() != "success") {
        QString errorMsg = root["error"].toString("Неизвестная ошибка Prometheus");
        emit errorOccurred(QString("Ошибка Prometheus: %1").arg(errorMsg));
        return false;
    }
    
    // Получаем данные
    QJsonObject dataObj = root["data"].toObject();
    if (!dataObj.contains("result")) {
        emit errorOccurred("Отсутствуют данные result в ответе");
        return false;
    }
    
    QJsonArray results = dataObj["result"].toArray();
    
    int processedCount = 0;
    
    // Обрабатываем каждую метрику
    for (const QJsonValue& resultValue : results) {
        QJsonObject result = resultValue.toObject();
        
        // Получаем метки
        QJsonObject metric = result["metric"].toObject();
        QString label = extractLabel(metric);
        QString code = extractCode(metric);
        
        // Фильтруем ненужные метрики
        if (shouldSkipMetric(label)) {
            continue;
        }
        
        // Получаем значения
        QJsonArray values = result["values"].toArray();
        
        QVector<double> timestamps;
        QVector<double> metricValues;
        extractValues(values, timestamps, metricValues);
        
        if (!timestamps.isEmpty()) {
            graphManager->addGraphData(label, code, timestamps, metricValues);
            processedCount++;
        }
    }
    
    if (processedCount == 0) {
        emit errorOccurred("Нет данных для отображения после фильтрации");
        return false;
    }
    
    emit processingFinished(processedCount);
    return true;
}

QString PrometheusProcessor::extractLabel(const QJsonObject& metric)
{
    // Пробуем получить label из разных полей
    if (metric.contains("label")) {
        return metric["label"].toString();
    }
    if (metric.contains("__name__")) {
        return metric["__name__"].toString();
    }
    if (metric.contains("quantile")) {
        return QString("quantile_%1").arg(metric["quantile"].toString());
    }
    return "unknown";
}

QString PrometheusProcessor::extractCode(const QJsonObject& metric)
{
    // Пробуем получить code
    if (metric.contains("code")) {
        return metric["code"].toString();
    }
    return "";
}

void PrometheusProcessor::extractValues(const QJsonArray& values, 
                                        QVector<double>& timestamps, 
                                        QVector<double>& metricValues)
{
    timestamps.clear();
    metricValues.clear();
    
    for (const QJsonValue& pointValue : values) {
        if (!pointValue.isArray()) continue;
        
        QJsonArray point = pointValue.toArray();
        if (point.size() >= 2) {
            double timestamp = point[0].toDouble();
            double value = point[1].toString().toDouble();
            
            timestamps.append(timestamp);
            metricValues.append(value);
        }
    }
}

bool PrometheusProcessor::shouldSkipMetric(const QString& label)
{
    // Пропускаем метрики, содержащие эти подстроки
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