#include "graphwidget.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>
#include <QRandomGenerator>

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent)
    , m_gridVisible(true)
    , m_legendVisible(true)
    , m_xAxisLabel("Время")
    , m_yAxisLabel("Время ответа (мс)")
    , m_dateTimeFormat("hh:mm:ss")
{
    setupChart();
    
    // Инициализация палитры цветов
    m_colorPalette = {
        QColor(255, 107, 107), QColor(78, 205, 196), QColor(69, 183, 209),
        QColor(150, 206, 180), QColor(255, 234, 167), QColor(221, 160, 221),
        QColor(152, 216, 200), QColor(247, 176, 94), QColor(108, 91, 123),
        QColor(249, 213, 110), QColor(184, 59, 94), QColor(44, 120, 108)
    };
}

GraphWidget::~GraphWidget()
{
    if (m_chartView) {
        delete m_chartView;
        m_chartView = nullptr;
    }
}

void GraphWidget::setupChart()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    m_chart = new QChart();
    m_chart->setTitle("");
    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    m_chart->setTheme(QChart::ChartThemeLight);
    
    // Создаем оси
    m_axisX = new QDateTimeAxis();
    m_axisX->setFormat(m_dateTimeFormat);
    m_axisX->setTitleText(m_xAxisLabel);
    m_axisX->setGridLineVisible(m_gridVisible);
    
    m_axisY = new QValueAxis();
    m_axisY->setTitleText(m_yAxisLabel);
    m_axisY->setGridLineVisible(m_gridVisible);
    m_axisY->setLabelFormat("%.2f");
    
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    
    // Настройка легенды
    m_chart->legend()->setVisible(m_legendVisible);
    m_chart->legend()->setBackgroundVisible(true);
    m_chart->legend()->setBrush(QBrush(QColor(255, 255, 255, 200)));
    m_chart->legend()->setBorderColor(Qt::gray);
    
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setRubberBand(QChartView::RectangleRubberBand);
    
    layout->addWidget(m_chartView);
}

QColor GraphWidget::generateColor(int index)
{
    if (index < m_colorPalette.size()) {
        return m_colorPalette[index];
    }
    return QColor(
        QRandomGenerator::global()->bounded(256),
        QRandomGenerator::global()->bounded(256),
        QRandomGenerator::global()->bounded(256)
    );
}

void GraphWidget::plotGraphs(const QVector<GraphData>& graphs)
{
    if (!m_chart || graphs.isEmpty()) {
        return;
    }
    
    // Очищаем старые серии
    QList<QAbstractSeries*> seriesList = m_chart->series();
    for (QAbstractSeries* series : seriesList) {
        if (series) {
            m_chart->removeSeries(series);
            // Не удаляем series здесь
        }
    }
    
    // Добавляем новые серии
    for (int i = 0; i < graphs.size(); ++i) {
        const GraphData& data = graphs[i];
        
        if (data.timestamps.isEmpty() || data.values.isEmpty()) {
            continue;
        }
        
        QLineSeries *series = new QLineSeries();
        series->setName(QString("%1 (code: %2)").arg(data.label).arg(data.code));
        
        // Добавляем точки
        for (int j = 0; j < data.timestamps.size(); ++j) {
            QDateTime timestamp = QDateTime::fromSecsSinceEpoch(
                static_cast<qint64>(data.timestamps[j])
            );
            series->append(timestamp.toMSecsSinceEpoch(), data.values[j]);
        }
        
        // Настройка цвета и прозрачности
        QColor color = data.color.isValid() ? data.color : generateColor(i);
        QPen pen(color);
        pen.setWidth(2);
        series->setPen(pen);
        
        series->setPointsVisible(true);
        series->setPointLabelsVisible(false);
        
        m_chart->addSeries(series);
        
        // Привязываем серию к осям
        series->attachAxis(m_axisX);
        series->attachAxis(m_axisY);
    }
    
    // Автомасштабирование
    autoRescale();
}

void GraphWidget::clear()
{
    if (!m_chart) {
        return;
    }
    
    QList<QAbstractSeries*> seriesList = m_chart->series();
    for (QAbstractSeries* series : seriesList) {
        if (series) {
            m_chart->removeSeries(series);
        }
    }
    
    if (m_chart) {
        m_chart->update();
    }
}
void GraphWidget::autoRescale()
{
    if (m_chart->series().isEmpty()) {
        return;
    }
    
    m_chart->axes(Qt::Horizontal).first()->setRange(
        QDateTime::fromMSecsSinceEpoch(0),
        QDateTime::currentDateTime()
    );
    
    m_chart->createDefaultAxes();
    
    // Настройка оси Y начинать с 0
    m_axisY->setMin(0);
    
    // Обновляем оси
    m_chart->update();
}

void GraphWidget::resetZoom()
{
    autoRescale();
}

void GraphWidget::setYAxisRange(double min, double max)
{
    m_axisY->setRange(min, max);
    m_chart->update();
}

void GraphWidget::setBackgroundColor(const QColor& color)
{
    m_chart->setBackgroundBrush(QBrush(color));
    m_chartView->setBackgroundBrush(QBrush(color));
    m_chart->update();
}

void GraphWidget::setGridVisible(bool visible)
{
    m_gridVisible = visible;
    m_axisX->setGridLineVisible(visible);
    m_axisY->setGridLineVisible(visible);
    m_chart->update();
}

void GraphWidget::setLegendVisible(bool visible)
{
    m_legendVisible = visible;
    m_chart->legend()->setVisible(visible);
    m_chart->update();
}

void GraphWidget::setXAxisLabel(const QString& label)
{
    m_xAxisLabel = label;
    m_axisX->setTitleText(label);
    m_chart->update();
}

void GraphWidget::setYAxisLabel(const QString& label)
{
    m_yAxisLabel = label;
    m_axisY->setTitleText(label);
    m_chart->update();
}

void GraphWidget::setDateTimeFormat(const QString& format)
{
    m_dateTimeFormat = format;
    m_axisX->setFormat(format);
    m_chart->update();
}

void GraphWidget::saveAsImage(const QString& filePath, int width, int height)
{
    QPixmap pixmap;
    if (width > 0 && height > 0) {
        pixmap = m_chartView->grab(QRect(0, 0, width, height));
    } else {
        pixmap = m_chartView->grab();
    }
    pixmap.save(filePath);
}

void GraphWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_chartView->resize(size());
}