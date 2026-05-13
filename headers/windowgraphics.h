#ifndef WINDOWGRAPHICS_H
#define WINDOWGRAPHICS_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QTextEdit>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QtCharts>
#include "ProgressManager.h"
#include "graphmanager.h"
#include "appsettings.h"
#include <QtCharts/QChartView>

// Предварительное объявление
QT_CHARTS_USE_NAMESPACE


// Класс для масштабирования только по оси X (колесиком и горизонтальным выделением)
class ZoomableChartView : public QChartView {
    Q_OBJECT
    
private:
    qreal m_zoomFactor;
    qint64 m_originalMin;
    qint64 m_originalMax;
    qint64 m_originalRange;
    double m_originalMaxValue;
    bool m_zoomEnabled;
    std::function<void()> m_onResetCallback; // Колбэк для сброса
    
public:
    explicit ZoomableChartView(QWidget* parent = nullptr) 
        : QChartView(parent), m_zoomFactor(1.2), m_zoomEnabled(true)
    {
        setRubberBand(QChartView::HorizontalRubberBand);
        setRenderHint(QPainter::Antialiasing);
        setCursor(Qt::CrossCursor);
    }
    
    void setOriginalRange(qint64 min, qint64 max) {
        m_originalMin = min;
        m_originalMax = max;
        m_originalRange = max - min;
    }
    
    void setOriginalMaxValue(double maxValue) {
        m_originalMaxValue = maxValue;
    }
    
    void setZoomEnabled(bool enabled) {
        m_zoomEnabled = enabled;
    }
    
    void setOnResetCallback(std::function<void()> callback) {
        m_onResetCallback = callback;
    }
    
protected:
    void wheelEvent(QWheelEvent* event) override {
        if (!m_zoomEnabled || !chart()) {
            QChartView::wheelEvent(event);
            return;
        }
        
        QList<QAbstractAxis*> axesX = chart()->axes(Qt::Horizontal);
        if (axesX.isEmpty()) return;
        
        QDateTimeAxis* axisX = qobject_cast<QDateTimeAxis*>(axesX.first());
        if (!axisX) return;
        
        QDateTime minX = axisX->min();
        QDateTime maxX = axisX->max();
        
        qint64 centerX = minX.toMSecsSinceEpoch() + (maxX.toMSecsSinceEpoch() - minX.toMSecsSinceEpoch()) / 2;
        
        qreal factor;
        if (event->angleDelta().y() > 0) {
            factor = 1.0 / m_zoomFactor;
        } else {
            factor = m_zoomFactor;
        }
        
        qint64 currentRange = maxX.toMSecsSinceEpoch() - minX.toMSecsSinceEpoch();
        qint64 newRange = static_cast<qint64>(currentRange * factor);
        
        if (newRange < 1000) newRange = 1000;
        if (m_originalRange > 0 && newRange > m_originalRange) newRange = m_originalRange;
        
        qint64 newMinX = centerX - newRange / 2;
        qint64 newMaxX = centerX + newRange / 2;
        
        axisX->setRange(QDateTime::fromMSecsSinceEpoch(newMinX), 
                        QDateTime::fromMSecsSinceEpoch(newMaxX));
        
        event->accept();
    }
    
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::RightButton) {
            // Сбрасываем масштаб
            resetZoom();
            // Вызываем колбэк для восстановления всех графиков
            if (m_onResetCallback) {
                m_onResetCallback();
            }
            event->accept();
        } else {
            QChartView::mousePressEvent(event);
        }
    }
    
    void resetZoom() {
        if (!chart()) return;
        
        QList<QAbstractAxis*> axesX = chart()->axes(Qt::Horizontal);
        if (!axesX.isEmpty()) {
            QDateTimeAxis* axisX = qobject_cast<QDateTimeAxis*>(axesX.first());
            if (axisX && m_originalRange > 0) {
                axisX->setRange(QDateTime::fromMSecsSinceEpoch(m_originalMin), 
                                QDateTime::fromMSecsSinceEpoch(m_originalMax));
            }
        }
        
        QList<QAbstractAxis*> axesY = chart()->axes(Qt::Vertical);
        if (!axesY.isEmpty()) {
            QValueAxis* axisY = qobject_cast<QValueAxis*>(axesY.first());
            if (axisY && m_originalMaxValue > 0) {
                axisY->setRange(0, m_originalMaxValue);
            }
        }
        
        update();
    }
};
class GraphManager;
class WindowGraphics : public QWidget
{
    Q_OBJECT

public:
    explicit WindowGraphics(QWidget *parent = nullptr);
    ~WindowGraphics();

    void updateFilePathLabel(const QString& text, bool success = true);
    void appendOutput(const QString& text);
    void appendStatusBar(const QString& text);
    void clearOutput();
    void addHostToList(const QString& hostInfo);
    void removeHostFromList(int row);
    // Получить список выбранных хостов (индексы)
    QList<int> getSelectedHostIndices() const;

    // Получить информацию о хосте с чекбоксом
    struct HostListItem {
        QString displayText;
        bool isChecked;
    };
    HostListItem getHostListItem(int index) const;
    void setHostChecked(int index, bool checked);
    bool isHostChecked(int index) const;
    
    void processGraphData(const QString& jsonData);
    void clearGraphs();
    void exportGraphs(const QString& filePath);
    void importGraphs(const QString& filePath);
    void setGraphOpacity(double opacity);
    QListWidgetItem* createHostListItem(const QString& hostInfo, bool checked);
    QPushButton* getShowGraphsButton() const { return showGraphsButton; }
    QLineEdit* getNewHostEdit() const { return newHostEdit; }
    QLineEdit* getSshUserEdit() const { return sshUserEdit; }
    QLineEdit* getSshPasswordEdit() const { return sshPasswordEdit; }
    QListWidget* getHostsListWidget() const { return hostsListWidget; }
    QPushButton* getAddHostButton() const { return addHostButton; }
    QPushButton* getRemoveHostButton() const { return removeHostButton; }
    QPushButton* getPlayButton() const { return playButton; }
    ProgressManager* getProgressManager() const { return progressManager; }
    void setupGraphTabs();

    void addHostGraphTab(const QString &host, const QString &title, const QString &prometheusQuery);

signals:
    void fileDropped(const QString& filePath);
    
private slots:
void onShowGraphsClicked();
void onRefreshAllGraphsClicked();

private:
    void setupUI();
    void updatePlayButtonState();
    QString injectInstanceFilter(const QString &query, const QString &instance);
    QTabWidget *getOrCreateHostTab(const QString &host); // Новая функция
    QMap<QString, QTabWidget*> m_hostInnerTabs;
    QLabel *dragDropLabel;
    QLabel *filePathLabel;
    QLineEdit *newHostEdit;
    QLineEdit *sshUserEdit;
    QLineEdit *sshPasswordEdit;
    QLabel *doggySign;
    QPushButton *addHostButton;
    QPushButton *removeHostButton;
    QListWidget *hostsListWidget;
    QProgressBar *progressBar;
    QPushButton *playButton;
    QTextEdit *outputTextEdit;
    QStatusBar *statusBar;
    
    GraphManager *graphManager;
    QVBoxLayout *m_graphLayout;
    ProgressManager *progressManager;

    QTabWidget* m_tabWidget;
    QPushButton* showGraphsButton;
    QPushButton* refreshAllGraphsButton;
    bool m_graphsVisible;
    GraphManager *m_graphManager;
    QChartView *m_chartView;
    SettingsButton* m_settingsButton;

    
    void addGraphTab(const QString& title, const QString& prometheusQuery);
};

#endif // WINDOWGRAPHICS_H