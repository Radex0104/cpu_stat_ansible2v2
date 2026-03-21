// graphwidget.h
#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QVector>
#include "graphmanager.h"  // Для получения GraphData

QT_CHARTS_USE_NAMESPACE

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();
    
    void plotGraphs(const QVector<GraphData>& graphs);
    void clear();
    void setBackgroundColor(const QColor& color);
    void setGridVisible(bool visible);
    void setLegendVisible(bool visible);
    void setXAxisLabel(const QString& label);
    void setYAxisLabel(const QString& label);
    void setDateTimeFormat(const QString& format);
    void autoRescale();
    void resetZoom();
    void setYAxisRange(double min, double max);
    void saveAsImage(const QString& filePath, int width = 0, int height = 0);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupChart();
    QColor generateColor(int index);
    
private:
    QChartView *m_chartView;
    QChart *m_chart;
    QDateTimeAxis *m_axisX;
    QValueAxis *m_axisY;
    bool m_gridVisible;
    bool m_legendVisible;
    QString m_xAxisLabel;
    QString m_yAxisLabel;
    QString m_dateTimeFormat;
    QVector<QColor> m_colorPalette;
};

#endif // GRAPHWIDGET_H