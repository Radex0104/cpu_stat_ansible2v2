#ifndef GRAPHMANAGER_H
#define GRAPHMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QMap>
#include <QVector>
#include <QString>
#include <QPointer>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegendMarker>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QEventLoop>
#include <QUrlQuery>
#include <QDateTime>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <QPointer>
#include "yaxisautoupdater.h"

struct MetricData {
    QString label;
    double minVal;
    double maxVal;
    double meanVal;
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["label"] = label;
        obj["min_val"] = minVal;
        obj["max_val"] = maxVal;
        obj["mean_val"] = meanVal;
        return obj;
    }
};

class GraphManager : public QObject
{
    Q_OBJECT
    
public:
    explicit GraphManager(QObject *parent = nullptr);
    
    void buildPrometheusChart(QtCharts::QChartView* chartView, const QString& query);
    void forceUpdateYAxis();
    
    QMap<QtCharts::QChartView*, QPointer<YAxisAutoUpdater>> m_updaters;
    
private:
    QJsonObject requestPrometheus(const QString& prometheusUrl, 
                                   const QString& dateStart, 
                                   const QString& dateEnd,
                                   const QString& query);
};

class LegendManager {
private:
    QChart* m_chart;
    QDateTimeAxis* m_axisX;
    QValueAxis* m_axisY;
    bool m_allVisible;
    QAreaSeries* m_soloSeries;
    QMap<QAreaSeries*, QColor> m_originalColors;
    QMap<QAreaSeries*, QPen> m_originalPens;
    QMap<QAreaSeries*, QBrush> m_originalBrushes;
    QMap<QAreaSeries*, bool> m_hadFill;
    
public:
    LegendManager(QChart* chart, QDateTimeAxis* axisX, QValueAxis* axisY) 
        : m_chart(chart), m_axisX(axisX), m_axisY(axisY), 
          m_allVisible(true), m_soloSeries(nullptr) 
    {
        if (!m_chart || !m_chart->legend()) return;
        
        m_chart->legend()->setVisible(true);
        m_chart->legend()->setAlignment(Qt::AlignRight);
        
        // Сохраняем оригинальные стили
        for (QLegendMarker* marker : m_chart->legend()->markers()) {
            QAreaSeries* series = qobject_cast<QAreaSeries*>(marker->series());
            if (!series) continue;
            
            m_originalColors[series] = series->color();
            m_originalPens[series] = series->pen();
            m_originalBrushes[series] = series->brush();
            m_hadFill[series] = (series->brush().style() != Qt::NoBrush);
            
            marker->setVisible(true);
            
            QObject::connect(marker, &QLegendMarker::clicked, [this, series]() {
                handleClick(series);
            });
        }
    }
    
    void handleClick(QAreaSeries* clickedSeries) {
        if (!m_chart || !m_chart->legend()) return;
        
        if (m_allVisible) {
            // Режим: показываем только выбранную серию
            for (QLegendMarker* marker : m_chart->legend()->markers()) {
                QAreaSeries* s = qobject_cast<QAreaSeries*>(marker->series());
                if (s) {
                    bool isSelected = (s == clickedSeries);
                    s->setVisible(isSelected);
                    
                    if (isSelected) {
                        // Добавляем заливку для выбранного графика (сохраняем пока он один)
                        QColor fillColor = s->color();
                        fillColor.setAlpha(80); // 30% прозрачности
                        s->setBrush(QBrush(fillColor));
                        
                        // Делаем линию толще
                        QPen thickPen(s->color(), 2);
                        s->setPen(thickPen);
                        if (s->upperSeries()) {
                            s->upperSeries()->setPen(thickPen);
                        }
                    } else {
                        // Скрытые графики - без заливки
                        s->setBrush(QBrush(Qt::NoBrush));
                    }
                    marker->setVisible(true);
                }
            }
            m_allVisible = false;
            m_soloSeries = clickedSeries;
            recalcYAxisForCurrentRange();
        }
        else if (m_soloSeries == clickedSeries) {
            // Возвращаем все графики
            for (QLegendMarker* marker : m_chart->legend()->markers()) {
                QAreaSeries* s = qobject_cast<QAreaSeries*>(marker->series());
                if (s) {
                    s->setVisible(true);
                    
                    // Восстанавливаем оригинальные стили
                    if (m_originalColors.contains(s)) {
                        s->setColor(m_originalColors[s]);
                        s->setBorderColor(m_originalColors[s]);
                        s->setPen(m_originalPens[s]);
                        if (s->upperSeries()) {
                            s->upperSeries()->setPen(m_originalPens[s]);
                        }
                    }
                    
                    // Восстанавливаем оригинальную заливку
                    if (m_hadFill[s]) {
                        s->setBrush(m_originalBrushes[s]);
                    } else {
                        s->setBrush(QBrush(Qt::NoBrush));
                    }
                    
                    marker->setVisible(true);
                }
            }
            m_allVisible = true;
            m_soloSeries = nullptr;
            recalcYAxisForCurrentRange();
        }
        else {
            // Переключаемся на другой график (уже в режиме одного графика)
            for (QLegendMarker* marker : m_chart->legend()->markers()) {
                QAreaSeries* s = qobject_cast<QAreaSeries*>(marker->series());
                if (s) {
                    bool isSelected = (s == clickedSeries);
                    s->setVisible(isSelected);
                    
                    if (isSelected) {
                        // Добавляем заливку для нового выбранного графика
                        QColor fillColor = s->color();
                        fillColor.setAlpha(80);
                        s->setBrush(QBrush(fillColor));
                        
                        QPen thickPen(s->color(), 2);
                        s->setPen(thickPen);
                        if (s->upperSeries()) {
                            s->upperSeries()->setPen(thickPen);
                        }
                    } else {
                        // Другие графики скрыты и без заливки
                        s->setBrush(QBrush(Qt::NoBrush));
                    }
                    marker->setVisible(true);
                }
            }
            m_soloSeries = clickedSeries;
            recalcYAxisForCurrentRange();
        }
    }

    void resetAllSeries() {
        if (!m_chart) return;
        
        for (QLegendMarker* marker : m_chart->legend()->markers()) {
            QAreaSeries* s = qobject_cast<QAreaSeries*>(marker->series());
            if (s) {
                s->setVisible(true);
                
                if (m_originalColors.contains(s)) {
                    s->setColor(m_originalColors[s]);
                    s->setBorderColor(m_originalColors[s]);
                    s->setPen(m_originalPens[s]);
                    if (s->upperSeries()) {
                        s->upperSeries()->setPen(m_originalPens[s]);
                    }
                }
                
                if (m_hadFill[s]) {
                    s->setBrush(m_originalBrushes[s]);
                } else {
                    s->setBrush(QBrush(Qt::NoBrush));
                }
                
                marker->setVisible(true);
            }
        }
        m_allVisible = true;
        m_soloSeries = nullptr;
        recalcYAxisForCurrentRange();
    }
    
private:
    void recalcYAxisForCurrentRange() {
        if (!m_axisX || !m_axisY) return;
        
        QDateTime minDateTime = m_axisX->min();
        QDateTime maxDateTime = m_axisX->max();
        
        qint64 minTimeSec = minDateTime.toSecsSinceEpoch();
        qint64 maxTimeSec = maxDateTime.toSecsSinceEpoch();
        
        double maxValue = 0;
        
        for (QAbstractSeries* series : m_chart->series()) {
            QAreaSeries* areaSeries = qobject_cast<QAreaSeries*>(series);
            if (!areaSeries || !areaSeries->isVisible()) continue;
            
            QLineSeries* upperSeries = areaSeries->upperSeries();
            if (!upperSeries) continue;
            
            for (const QPointF& point : upperSeries->points()) {
                qint64 pointTimeSec = static_cast<qint64>(point.x() / 1000);
                if (pointTimeSec >= minTimeSec && pointTimeSec <= maxTimeSec) {
                    if (point.y() > maxValue) {
                        maxValue = point.y();
                    }
                }
            }
        }
        
        if (maxValue > 0) {
            m_axisY->setRange(0, maxValue * 1.1);
            qDebug() << "Y axis recalculated, new max:" << maxValue;
        }
    }
};
#endif // GRAPHMANAGER_H