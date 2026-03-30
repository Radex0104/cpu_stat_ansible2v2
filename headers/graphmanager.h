#ifndef GRAPHMANAGER_H
#define GRAPHMANAGER_H

#include <QObject>
#include <QVector>
#include <QColor>
#include <QPointF>
#include <QJsonDocument>
#include <QJsonObject>

// Структура для хранения данных графика
struct GraphData {
    QString label;
    QString code;
    QVector<double> timestamps;
    QVector<double> values;
    QColor color;
    double opacity;
    
    double minValue;
    double maxValue;
    double meanValue;
    
    GraphData() 
        : opacity(0.7)
        , minValue(0)
        , maxValue(0)
        , meanValue(0) 
    {}
};

// Структура для статистики метрики
struct MetricStats {
    QString label;
    QString code;
    double min;
    double max;
    double mean;
    int dataPointsCount;
    
    MetricStats() : min(0), max(0), mean(0), dataPointsCount(0) {}
    MetricStats(const QString& l, const QString& c, double mn, double mx, double m, int count)
        : label(l), code(c), min(mn), max(mx), mean(m), dataPointsCount(count) {}
};

// Структура для отображения графика (серии данных)
struct GraphSeries {
    QString name;
    QVector<QPointF> points;
    QColor color;
    double minValue;
    double maxValue;
    double meanValue;
    
    GraphSeries() : minValue(0), maxValue(0), meanValue(0) {}
};

class GraphManager : public QObject
{
    Q_OBJECT

public:
    explicit GraphManager(QObject *parent = nullptr);
    ~GraphManager();

    // Управление данными
    void clearAllGraphs();
    void addGraphData(const GraphData& data);
    void addGraphData(const QString& label, const QString& code,
                     const QVector<double>& timestamps,
                     const QVector<double>& values,
                     const QColor& color = QColor(), double opacity = -1);
    
    // Получение данных
    QVector<GraphData> getAllGraphs() const;
    QVector<GraphSeries> getGraphSeries() const;
    int getGraphCount() const;
    bool hasGraphs() const;
    
    // Обработка данных Prometheus
    bool processPrometheusData(const QJsonDocument& jsonData);
    bool processPrometheusData(const QString& jsonString);
    
    // Статистика
    QVector<MetricStats> getStatistics() const;
    
    // Настройки отображения
    void setGlobalOpacity(double opacity);
    void setColorPalette(const QVector<QColor>& palette);
    void resetColorPalette();
    
    // Экспорт/Импорт
    bool exportToJson(const QString& filePath) const;
    bool importFromJson(const QString& filePath);
    
signals:
    void graphDataUpdated();
    void graphDataAdded(const GraphData& data);
    void graphDataCleared();
    void errorOccurred(const QString& error);

private:
    void calculateStatistics(GraphData& data);
    QColor generateColor();
    bool shouldSkipMetric(const QString& label);
    bool parsePrometheusData(const QJsonObject& root);
    
    QVector<GraphData> m_graphs;
    QVector<QColor> m_colorPalette;
    int m_currentColorIndex;
    double m_globalOpacity;
    
    static const QVector<QColor> s_defaultPalette;
};

#endif // GRAPHMANAGER_H