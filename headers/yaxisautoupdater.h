#ifndef YAXISAUTOUPDATER_H
#define YAXISAUTOUPDATER_H

#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QDateTime>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>

QT_CHARTS_USE_NAMESPACE

class YAxisAutoUpdater : public QObject {
    Q_OBJECT
    
private:
    QPointer<QChartView> m_chartView;
    QPointer<QValueAxis> m_axisY;
    QVector<QAreaSeries*> m_series;
    QPointer<QDateTimeAxis> m_axisX;
    
public:
    YAxisAutoUpdater(QChartView* chartView, QDateTimeAxis* axisX, QValueAxis* axisY, 
                     const QVector<QAreaSeries*>& series, QObject* parent = nullptr)
        : QObject(parent), m_chartView(chartView), m_axisY(axisY), m_series(series), m_axisX(axisX)
    {
        
        // Подключаемся к сигналу изменения диапазона оси X
        if (m_axisX) {
            connect(m_axisX, &QDateTimeAxis::rangeChanged, 
                    this, &YAxisAutoUpdater::onXRangeChanged);
        }
        
        // Подключаемся к сигналу rubberBand
        if (m_chartView) {
            connect(m_chartView, &QChartView::rubberBandChanged, 
                    this, &YAxisAutoUpdater::onRubberBandChanged);
        }
    }
    
public slots:
    void onXRangeChanged(const QDateTime& min, const QDateTime& max) {
        updateYAxis();
    }
    
    void onRubberBandChanged(const QRectF& rect, const QPointF& from, const QPointF& to) {
        updateYAxis();
    }
    
    void updateYAxis() {
        if (!m_axisY || !m_axisX || m_series.isEmpty() || m_chartView.isNull()) {
            return;
        }
        
        QDateTime minDateTime = m_axisX->min();
        QDateTime maxDateTime = m_axisX->max();
        
        if (!minDateTime.isValid() || !maxDateTime.isValid()) {
            return;
        }
        
        qint64 minTimeSec = minDateTime.toSecsSinceEpoch();
        qint64 maxTimeSec = maxDateTime.toSecsSinceEpoch();
        
        
        double maxValue = 0;
        int pointsChecked = 0;
        
        for (QAreaSeries* areaSeries : m_series) {
            if (!areaSeries) continue;
            
            QLineSeries* upperSeries = areaSeries->upperSeries();
            if (!upperSeries) continue;
            
            const QList<QPointF>& points = upperSeries->points();
            
            for (const QPointF& point : points) {
                qint64 pointTimeSec = static_cast<qint64>(point.x() / 1000);
                
                if (pointTimeSec >= minTimeSec && pointTimeSec <= maxTimeSec) {
                    pointsChecked++;
                    if (point.y() > maxValue) {
                        maxValue = point.y();
                    }
                }
            }
        }
        
        
        if (maxValue > 1) {
            double newMax = maxValue * 1.1;
            
            m_axisY->setRange(0, newMax);
        } else if (maxValue < 1) {
            m_axisY->setRange(0, maxValue * 1.1);
            m_axisY->setLabelFormat("%.2f");
        }
    }
};

#endif // YAXISAUTOUPDATER_H