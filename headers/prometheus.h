#ifndef PROMETHEUS_H
#define PROMETHEUS_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVector>
#include "graphmanager.h"

class PrometheusProcessor : public QObject
{
    Q_OBJECT

public:
    explicit PrometheusProcessor(QObject *parent = nullptr);
    
    // Обработка JSON данных из Prometheus
    bool processJsonData(const QByteArray& jsonData, GraphManager* graphManager);
    bool processJsonData(const QString& jsonString, GraphManager* graphManager);
    
signals:
    void errorOccurred(const QString& error);
    void processingFinished(int graphCount);

private:
    bool parsePrometheusData(const QJsonObject& root, GraphManager* graphManager);
    bool shouldSkipMetric(const QString& label);
    
    // Методы для извлечения данных
    QString extractLabel(const QJsonObject& metric);
    QString extractCode(const QJsonObject& metric);
    void extractValues(const QJsonArray& values, QVector<double>& timestamps, QVector<double>& metricValues);
};

#endif // PROMETHEUS_H