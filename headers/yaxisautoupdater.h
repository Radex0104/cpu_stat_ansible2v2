#ifndef YAXISAUTOUPDATER_H
#define YAXISAUTOUPDATER_H

#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QDebug>
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
        qDebug() << "YAxisAutoUpdater created";
        
        // Подключаемся к сигналу изменения диапазона оси X
        if (m_axisX) {
            connect(m_axisX, &QDateTimeAxis::rangeChanged, 
                    this, &YAxisAutoUpdater::onXRangeChanged);
            qDebug() << "Connected to axisX rangeChanged signal";
        }
        
        // Подключаемся к сигналу rubberBand
        if (m_chartView) {
            connect(m_chartView, &QChartView::rubberBandChanged, 
                    this, &YAxisAutoUpdater::onRubberBandChanged);
            qDebug() << "Connected to rubberBandChanged signal";
        }
    }
    
public slots:
    void onXRangeChanged(const QDateTime& min, const QDateTime& max) {
        qDebug() << "X range changed:" << min.toString("hh:mm:ss") << "to" << max.toString("hh:mm:ss");
        updateYAxis();
    }
    
    void onRubberBandChanged(const QRectF& rect, const QPointF& from, const QPointF& to) {
        qDebug() << "Rubber band changed";
        updateYAxis();
    }
    
    void updateYAxis() {
        if (!m_axisY || !m_axisX || m_series.isEmpty() || m_chartView.isNull()) {
            qDebug() << "YAxisAutoUpdater: missing components";
            return;
        }
        
        QDateTime minDateTime = m_axisX->min();
        QDateTime maxDateTime = m_axisX->max();
        
        if (!minDateTime.isValid() || !maxDateTime.isValid()) {
            qDebug() << "Invalid datetime range";
            return;
        }
        
        qint64 minTimeSec = minDateTime.toSecsSinceEpoch();
        qint64 maxTimeSec = maxDateTime.toSecsSinceEpoch();
        
        qDebug() << "Updating Y axis for X range:" << minTimeSec << "to" << maxTimeSec;
        
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
        
        qDebug() << "Checked" << pointsChecked << "points, max value:" << maxValue;
        
        if (maxValue > 0) {
            double newMax = maxValue * 1.1;
            if (newMax < 10) newMax = 10;
            
            qDebug() << "Setting Y axis range to 0 -" << newMax;
            m_axisY->setRange(0, newMax);
        } else {
            qDebug() << "No data points found in current X range";
        }
    }
};

#endif // YAXISAUTOUPDATER_H