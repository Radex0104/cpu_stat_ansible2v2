#ifndef GRAPHMANAGER_H
#define GRAPHMANAGER_H

#include <QObject>
#include <QVector>
#include <QColor>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <memory>

// Структура для хранения данных графика
struct GraphData {
    QString label;           // Название метрики
    QString code;            // Код ответа
    QVector<double> timestamps;  // Временные метки (Unix timestamp)
    QVector<double> values;      // Значения метрики
    QColor color;            // Цвет графика
    double opacity;          // Прозрачность (0-1)
    
    GraphData() : opacity(0.7) {}
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
                     const QColor& color = QColor(),
                     double opacity = 0.7);
    
    // Получение данных
    QVector<GraphData> getAllGraphs() const;
    int getGraphCount() const;
    bool hasGraphs() const;
    
    // Обработка данных из Prometheus
    bool processPrometheusData(const QJsonDocument& jsonData);
    bool processPrometheusData(const QString& jsonString);
    
    // Настройка внешнего вида
    void setGlobalOpacity(double opacity);
    void setColorPalette(const QVector<QColor>& palette);
    void resetColorPalette();
    
    // Экспорт/импорт данных
    bool exportToJson(const QString& filePath) const;
    bool importFromJson(const QString& filePath);
    
signals:
    // Сигналы для уведомления об изменениях
    void graphDataAdded(const GraphData& data);
    void graphDataCleared();
    void graphDataUpdated();  // Общее обновление всех данных
    void errorOccurred(const QString& errorMessage);

private:
    // Генерация цвета для графика
    QColor generateColor();
    
    // Парсинг данных из JSON
    bool parsePrometheusData(const QJsonObject& root);
    
private:
    QVector<GraphData> m_graphs;           // Все графики
    QVector<QColor> m_colorPalette;        // Палитра цветов
    int m_currentColorIndex;               // Текущий индекс цвета
    double m_globalOpacity;                // Глобальная прозрачность
    static const QVector<QColor> DEFAULT_PALETTE;  // Стандартная палитра
};

#endif // GRAPHMANAGER_H