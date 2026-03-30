#include "windowgraphics.h"
#include "graphmanager.h"
#include "prometheus.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

WindowGraphics::WindowGraphics(QWidget *parent)
    : QWidget(parent)
    , progressManager(new ProgressManager(this))
    , graphManager(nullptr)
    , m_chartView(nullptr)
    , m_graphLayout(nullptr)
{
    setupUI();
    setAcceptDrops(true);
    
    graphManager = new GraphManager(this);
    
    connect(graphManager, &GraphManager::graphDataUpdated, 
            this, &WindowGraphics::onGraphDataUpdated);
    connect(graphManager, &GraphManager::errorOccurred,
            this, &WindowGraphics::appendOutput);
    
    progressManager->setProgressBar(progressBar);
}

WindowGraphics::~WindowGraphics()
{
    // Сначала отключаем сигналы
    if (graphManager) {
        disconnect(graphManager, nullptr, this, nullptr);
    }
}

void WindowGraphics::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    dragDropLabel = new QLabel("Перетащите архив сюда");
    dragDropLabel->setAlignment(Qt::AlignCenter);
    dragDropLabel->setStyleSheet("QLabel { border: 2px dashed #aaa; padding: 20px; background-color: #f8f8f8; }");
    dragDropLabel->setMinimumHeight(100);
    mainLayout->addWidget(dragDropLabel);

    filePathLabel = new QLabel("Архив не выбран");
    filePathLabel->setStyleSheet("QLabel { color: #666; font-size: 10pt; }");
    mainLayout->addWidget(filePathLabel);

    QGroupBox *hostsGroup = new QGroupBox("Настройка хостов");
    QVBoxLayout *hostsLayout = new QVBoxLayout(hostsGroup);

    QHBoxLayout *hostsControlLayout = new QHBoxLayout();
    newHostEdit = new QLineEdit();
    doggySign = new QLabel("@");
    sshUserEdit = new QLineEdit();
    sshPasswordEdit = new QLineEdit();
    statusBar = new QStatusBar();
    
    sshUserEdit->setPlaceholderText("Имя пользователя на сервере");
    sshUserEdit->setText("astra1");
    newHostEdit->setPlaceholderText("Введите IP или домен");
    newHostEdit->setText("192.168.0.");
    sshPasswordEdit->setPlaceholderText("Введите пароль для хоста");
    sshPasswordEdit->setEchoMode(QLineEdit::Password);
    
    addHostButton = new QPushButton("Добавить");
    removeHostButton = new QPushButton("Удалить");

    hostsControlLayout->addWidget(sshUserEdit);
    hostsControlLayout->addWidget(doggySign);
    hostsControlLayout->addWidget(newHostEdit);
    hostsControlLayout->addWidget(sshPasswordEdit);
    hostsControlLayout->addWidget(addHostButton);
    hostsControlLayout->addWidget(removeHostButton);

    hostsListWidget = new QListWidget();
    hostsLayout->addLayout(hostsControlLayout);
    hostsLayout->addWidget(hostsListWidget);
    mainLayout->addWidget(hostsGroup);

    // Группа для графиков
    QGroupBox *graphGroup = new QGroupBox("Графики времени ответа");
    m_graphLayout = new QVBoxLayout(graphGroup);
    
    // Создаем QChartView для отображения графиков
    m_chartView = new QChartView(this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setRubberBand(QChartView::RectangleRubberBand);
    m_chartView->setMinimumHeight(300);
    m_graphLayout->addWidget(m_chartView);
    
    mainLayout->addWidget(graphGroup);

    QGroupBox *progressGroup = new QGroupBox("Прогресс выполнения");
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);
    
    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setVisible(false);
    progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 1px solid #bbb;"
        "    border-radius: 5px;"
        "    text-align: center;"
        "    height: 25px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #4CAF50;"
        "    border-radius: 5px;"
        "}"
    );
    
    progressLayout->addWidget(progressBar);
    mainLayout->addWidget(progressGroup);

    playButton = new QPushButton("Play");
    playButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    padding: 10px;"
        "    font-size: 14pt;"
        "    border: none;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #cccccc;"
        "}"
    );
    mainLayout->addWidget(playButton);

    QGroupBox *outputGroup = new QGroupBox("Вывод Ansible");
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);

    outputTextEdit = new QTextEdit();
    outputTextEdit->setReadOnly(true);
    outputTextEdit->setMinimumHeight(200);
    outputTextEdit->setFontFamily("Courier New");

    outputLayout->addWidget(outputTextEdit);
    mainLayout->addWidget(outputGroup);
    mainLayout->addWidget(statusBar);
}

void WindowGraphics::onGraphDataReceived(const QString& jsonData)
{
    qDebug() << "Received data from Prometheus";
    
    if (!graphManager) {
        appendOutput("⚠️ GraphManager не инициализирован");
        return;
    }
    
    // Создаем обработчик Prometheus
    PrometheusProcessor processor;
    
    // Подключаем сигналы для отладки
    connect(&processor, &PrometheusProcessor::errorOccurred, this, [this](const QString& error) {
        appendOutput("⚠️ " + error);
    });
    
    connect(&processor, &PrometheusProcessor::processingFinished, this, [this](int count) {
        appendOutput(QString("✅ Обработано %1 метрик").arg(count));
    });
    
    // Обрабатываем данные
    bool success = processor.processJsonData(jsonData, graphManager);
    
    if (!success) {
        appendOutput("⚠️ Ошибка обработки данных Prometheus");
    }
}

void WindowGraphics::plotGraphs(const QVector<GraphSeries>& series)
{
    if (!m_chartView) {
        qDebug() << "m_chartView is null";
        return;
    }
    
    if (series.isEmpty()) {
        appendOutput("⚠️ Нет данных для отображения");
        return;
    }
    
    // Создаем новый chart
    QChart *chart = new QChart();
    chart->setTitle("");
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
    
    // Добавляем серии
    for (const GraphSeries& s : series) {
        QLineSeries *lineSeries = new QLineSeries();
        lineSeries->setName(s.name);
        
        for (const QPointF& point : s.points) {
            lineSeries->append(static_cast<qint64>(point.x()), point.y());
        }
        
        QPen pen(s.color);
        pen.setWidth(2);
        lineSeries->setPen(pen);
        lineSeries->setPointsVisible(true);
        
        chart->addSeries(lineSeries);
        lineSeries->attachAxis(axisX);
        lineSeries->attachAxis(axisY);
    }
    
    // Настройка легенды
    chart->legend()->setVisible(true);
    chart->legend()->setBackgroundVisible(true);
    chart->legend()->setBrush(QBrush(QColor(255, 255, 255, 200)));
    chart->legend()->setBorderColor(Qt::gray);
    
    // Обновляем chart view
    m_chartView->setChart(chart);
}

void WindowGraphics::onGraphDataUpdated()
{
    if (!graphManager) {
        return;
    }
    
    // Получаем данные для отображения
    QVector<GraphSeries> series = graphManager->getGraphSeries();
    
    if (series.isEmpty()) {
        appendOutput("⚠️ Нет данных для отображения");
        if (m_chartView) {
            m_chartView->setChart(new QChart());
        }
        return;
    }
    
    // Отрисовываем графики
    plotGraphs(series);
    
    appendOutput(QString("✅ Загружено %1 графиков").arg(series.size()));
}

void WindowGraphics::processGraphData(const QString& jsonData)
{
    onGraphDataReceived(jsonData);
}

void WindowGraphics::clearGraphs()
{
    if (graphManager) {
        graphManager->clearAllGraphs();
    }
    
    // Очищаем chart
    if (m_chartView) {
        m_chartView->setChart(new QChart());
    }
    
    appendOutput("🗑️ Графики очищены");
}

void WindowGraphics::exportGraphs(const QString& filePath)
{
    if (graphManager && graphManager->exportToJson(filePath)) {
        appendOutput(QString("💾 Графики экспортированы в %1").arg(filePath));
    } else {
        appendOutput(QString("⚠️ Ошибка экспорта графиков в %1").arg(filePath));
    }
}

void WindowGraphics::importGraphs(const QString& filePath)
{
    if (graphManager && graphManager->importFromJson(filePath)) {
        appendOutput(QString("📂 Графики импортированы из %1").arg(filePath));
    } else {
        appendOutput(QString("⚠️ Ошибка импорта графиков из %1").arg(filePath));
    }
}

void WindowGraphics::setGraphOpacity(double opacity)
{
    if (graphManager) {
        graphManager->setGlobalOpacity(opacity);
        appendOutput(QString("🎨 Прозрачность графиков установлена: %1%").arg(opacity * 100));
    }
}

void WindowGraphics::updatePlayButtonState()
{
    if (playButton) {
        playButton->setEnabled(hostsListWidget && hostsListWidget->count() > 0);
    }
}

void WindowGraphics::updateFilePathLabel(const QString& text, bool success)
{
    if (filePathLabel) {
        filePathLabel->setText(text);
        filePathLabel->setStyleSheet(success ?
            "QLabel { color: green; font-size: 10pt; }" :
            "QLabel { color: red; font-size: 10pt; }");
    }
}

void WindowGraphics::appendOutput(const QString& text)
{
    if (outputTextEdit) {
        outputTextEdit->append(text);
    }
}

void WindowGraphics::appendStatusBar(const QString& text)
{
    if (statusBar) {
        statusBar->showMessage(text);
    }
}

void WindowGraphics::clearOutput()
{
    if (outputTextEdit) {
        outputTextEdit->clear();
    }
}

void WindowGraphics::addHostToList(const QString& hostInfo)
{
    if (hostsListWidget) {
        hostsListWidget->addItem(hostInfo);
    }
}

void WindowGraphics::removeHostFromList(int row)
{
    if (hostsListWidget && row >= 0 && row < hostsListWidget->count()) {
        delete hostsListWidget->takeItem(row);
    }
}

