#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QClipboard>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include "graphmanager.h"

QT_CHARTS_USE_NAMESPACE

class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    TestWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setupUI();
        setupConnections();
        
        setWindowTitle("Prometheus Data Test - GUI");
        resize(1200, 800);
    }
    
    // Метод для загрузки файла по пути
    void loadFile(const QString& filePath)
    {
        if (!filePath.isEmpty()) {
            loadAndProcessData(filePath);
        }
    }

public slots:
    void onLoadFile()
    {
        QString filePath = QFileDialog::getOpenFileName(
            this,
            "Выберите JSON файл с данными Prometheus",
            QString(),
            "JSON Files (*.json);;All Files (*)"
        );
        
        if (filePath.isEmpty()) {
            return;
        }
        
        loadAndProcessData(filePath);
    }
    
    void onProcessClipboard()
    {
        QClipboard *clipboard = QApplication::clipboard();
        if (!clipboard) {
            appendLog("⚠️ Не удалось получить доступ к буферу обмена");
            return;
        }
        
        QString jsonData = clipboard->text();
        if (jsonData.isEmpty()) {
            appendLog("⚠️ Буфер обмена пуст");
            return;
        }
        
        appendLog("📋 Данные загружены из буфера обмена");
        processJsonData(jsonData.toUtf8());
    }
    
    void onExportGraphs()
    {
        if (!m_graphManager || !m_graphManager->hasGraphs()) {
            appendLog("⚠️ Нет данных для экспорта");
            return;
        }
        
        QString filePath = QFileDialog::getSaveFileName(
            this,
            "Сохранить графики",
            QString(),
            "JSON Files (*.json)"
        );
        
        if (filePath.isEmpty()) {
            return;
        }
        
        if (m_graphManager->exportToJson(filePath)) {
            appendLog("✅ Графики экспортированы в: " + filePath);
        } else {
            appendLog("❌ Ошибка экспорта графиков");
        }
    }
    
    void onSaveImage()
    {
        if (!m_chartView) {
            appendLog("⚠️ Графический виджет не инициализирован");
            return;
        }
        
        QString filePath = QFileDialog::getSaveFileName(
            this,
            "Сохранить график как изображение",
            QString(),
            "PNG Images (*.png);;JPEG Images (*.jpg);;All Files (*)"
        );
        
        if (filePath.isEmpty()) {
            return;
        }
        
        QPixmap pixmap = m_chartView->grab();
        pixmap.save(filePath);
        appendLog("✅ График сохранен как: " + filePath);
    }
    
    void onClearGraphs()
    {
        if (m_graphManager) {
            m_graphManager->clearAllGraphs();
        }
        if (m_chartView) {
            m_chartView->setChart(new QChart());
        }
        appendLog("🗑️ Графики очищены");
    }

private:
    void setupUI()
    {
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        
        // Кнопки управления
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        
        m_loadButton = new QPushButton("📂 Загрузить JSON файл");
        m_clipboardButton = new QPushButton("📋 Вставить из буфера");
        m_exportButton = new QPushButton("💾 Экспортировать данные");
        m_saveImageButton = new QPushButton("📸 Сохранить график");
        m_clearButton = new QPushButton("🗑️ Очистить");
        
        buttonLayout->addWidget(m_loadButton);
        buttonLayout->addWidget(m_clipboardButton);
        buttonLayout->addWidget(m_exportButton);
        buttonLayout->addWidget(m_saveImageButton);
        buttonLayout->addWidget(m_clearButton);
        
        mainLayout->addLayout(buttonLayout);
        
        // График (QChartView)
        m_chartView = new QChartView(this);
        m_chartView->setRenderHint(QPainter::Antialiasing);
        m_chartView->setRubberBand(QChartView::RectangleRubberBand);
        m_chartView->setMinimumHeight(400);
        mainLayout->addWidget(m_chartView);
        
        // Лог вывода
        m_logText = new QTextEdit(this);
        m_logText->setReadOnly(true);
        m_logText->setMaximumHeight(200);
        m_logText->setFontFamily("Courier New");
        mainLayout->addWidget(m_logText);
        
        setCentralWidget(centralWidget);
        
        // Инициализируем менеджер графиков
        m_graphManager = new GraphManager(this);
        
        connect(m_graphManager, &GraphManager::graphDataUpdated, 
                this, &TestWindow::onGraphDataUpdated);
        connect(m_graphManager, &GraphManager::errorOccurred,
                this, &TestWindow::onError);
    }
    
    void setupConnections()
    {
        connect(m_loadButton, &QPushButton::clicked, this, &TestWindow::onLoadFile);
        connect(m_clipboardButton, &QPushButton::clicked, this, &TestWindow::onProcessClipboard);
        connect(m_exportButton, &QPushButton::clicked, this, &TestWindow::onExportGraphs);
        connect(m_saveImageButton, &QPushButton::clicked, this, &TestWindow::onSaveImage);
        connect(m_clearButton, &QPushButton::clicked, this, &TestWindow::onClearGraphs);
    }
    
    void loadAndProcessData(const QString& filePath)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            appendLog("❌ Не удалось открыть файл: " + filePath);
            return;
        }
        
        QByteArray jsonData = file.readAll();
        file.close();
        
        appendLog("📁 Загружен файл: " + QFileInfo(filePath).fileName());
        appendLog("📊 Размер: " + QString::number(jsonData.size()) + " байт");
        
        processJsonData(jsonData);
    }
    
    void processJsonData(const QByteArray& jsonData)
    {
        // Очищаем JSON от NaN
        QString jsonString = QString::fromUtf8(jsonData);
        
        // Заменяем NaN на null
        jsonString.replace(QRegularExpression("\\bNaN\\b"), "null");
        jsonString.replace(QRegularExpression("\\bnan\\b"), "null");
        
        // Заменяем Infinity на null
        jsonString.replace(QRegularExpression("\\bInfinity\\b"), "null");
        jsonString.replace(QRegularExpression("\\b-infinity\\b"), "null");
        
        QByteArray cleanedData = jsonString.toUtf8();
        
        // Парсим JSON
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(cleanedData, &parseError);
        
        if (parseError.error != QJsonParseError::NoError) {
            appendLog("❌ Ошибка парсинга JSON: " + parseError.errorString());
            return;
        }
        
        appendLog("✅ JSON валиден");
        
        // Обрабатываем данные через GraphManager
        bool success = m_graphManager->processPrometheusData(doc);
        
        if (success) {
            appendLog("✅ Данные успешно обработаны");
        } else {
            appendLog("❌ Ошибка обработки данных");
        }
    }
    
    void onGraphDataUpdated()
    {
        if (!m_graphManager || !m_chartView) {
            return;
        }
        
        QVector<GraphData> graphs = m_graphManager->getAllGraphs();
        
        if (graphs.isEmpty()) {
            appendLog("⚠️ Нет данных для отображения");
            return;
        }
        
        // Создаем новый chart
        QChart *chart = new QChart();
        chart->setTitle("Графики времени ответа");
        chart->setAnimationOptions(QChart::SeriesAnimations);
        chart->setTheme(QChart::ChartThemeLight);
        
        // Создаем оси
        QDateTimeAxis *axisX = new QDateTimeAxis();
        axisX->setFormat("hh:mm:ss");
        axisX->setTitleText("Время");
        axisX->setGridLineVisible(true);
        
        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("Время ответа (мс)");
        axisY->setGridLineVisible(true);
        axisY->setLabelFormat("%.2f");
        
        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);
        
        // Добавляем серии для каждого графика
        for (const GraphData& data : graphs) {
            if (data.timestamps.isEmpty() || data.values.isEmpty()) {
                continue;
            }
            
            QLineSeries *series = new QLineSeries();
            
            // Формируем имя с статистикой
            QString seriesName = QString("%1 (min: %2 ms, max: %3 ms, mean: %4 ms)")
                .arg(data.label)
                .arg(data.minValue, 0, 'f', 2)
                .arg(data.maxValue, 0, 'f', 2)
                .arg(data.meanValue, 0, 'f', 2);
            
            series->setName(seriesName);
            
            // Добавляем точки
            for (int i = 0; i < data.timestamps.size(); ++i) {
                // Конвертируем timestamp в миллисекунды для QDateTime
                qint64 timestampMs = static_cast<qint64>(data.timestamps[i] * 1000);
                series->append(timestampMs, data.values[i]);
            }
            
            // Настройка цвета
            QColor color = data.color.isValid() ? data.color : generateColor();
            QPen pen(color);
            pen.setWidth(2);
            series->setPen(pen);
            series->setPointsVisible(true);
            
            chart->addSeries(series);
            series->attachAxis(axisX);
            series->attachAxis(axisY);
        }
        
        // Настройка легенды
        chart->legend()->setVisible(true);
        chart->legend()->setBackgroundVisible(true);
        chart->legend()->setBrush(QBrush(QColor(255, 255, 255, 200)));
        chart->legend()->setBorderColor(Qt::gray);
        
        // Обновляем chart view
        m_chartView->setChart(chart);
        
        // Выводим статистику
        QVector<MetricStats> stats = m_graphManager->getStatistics();
        
        appendLog("\n📊 СТАТИСТИКА:");
        for (const MetricStats& stat : stats) {
            appendLog(QString("  • %1: min=%.2f ms, max=%.2f ms, mean=%.2f ms (точек: %2)")
                .arg(stat.label)
                .arg(stat.min)
                .arg(stat.max)
                .arg(stat.mean)
                .arg(stat.dataPointsCount));
        }
        
        appendLog(QString("✅ Загружено %1 графиков").arg(graphs.size()));
    }
    
    void onError(const QString& error)
    {
        appendLog("❌ Ошибка: " + error);
    }
    
    void appendLog(const QString& text)
    {
        if (m_logText) {
            m_logText->append(text);
        }
        qDebug() << text;
    }
    
    QColor generateColor()
    {
        static int colorIndex = 0;
        static QVector<QColor> palette = {
            QColor(255, 107, 107), QColor(78, 205, 196), QColor(69, 183, 209),
            QColor(150, 206, 180), QColor(255, 234, 167), QColor(221, 160, 221),
            QColor(152, 216, 200), QColor(247, 176, 94), QColor(108, 91, 123),
            QColor(249, 213, 110), QColor(184, 59, 94), QColor(44, 120, 108)
        };
        
        QColor color = palette[colorIndex % palette.size()];
        colorIndex++;
        return color;
    }
    
private:
    QPushButton *m_loadButton;
    QPushButton *m_clipboardButton;
    QPushButton *m_exportButton;
    QPushButton *m_saveImageButton;
    QPushButton *m_clearButton;
    QTextEdit *m_logText;
    QChartView *m_chartView;
    GraphManager *m_graphManager;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    TestWindow window;
    
    // Если передан аргумент командной строки, загружаем файл
    if (argc > 1) {
        QString filePath = argv[1];
        if (QFile::exists(filePath)) {
            // Используем QTimer для отложенной загрузки после отображения окна
            QTimer::singleShot(100, [&window, filePath]() {
                window.loadFile(filePath);
            });
        }
    }
    
    window.show();
    return app.exec();
}

#include "test_prometheus_gui.moc"