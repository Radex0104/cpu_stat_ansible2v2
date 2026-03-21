// windowgraphics.cpp
#include "windowgraphics.h"
#include "graphmanager.h"
#include "graphwidget.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

WindowGraphics::WindowGraphics(QWidget *parent)
    : QWidget(parent)
    , progressManager(new ProgressManager(this))
    , graphManager(nullptr)
    , graphWidget(nullptr)
    , graphWindow(nullptr)
{
    setupUI();
    setupGraphWidget();
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
    
    // Сначала удаляем виджет с графиками
    if (graphWindow) {
        graphWindow->close();
        delete graphWindow;
        graphWindow = nullptr;
    }
    
    // graphManager и graphWidget удалятся автоматически как дети this
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

void WindowGraphics::setupGraphWidget()
{
    graphWindow = new QWidget();
    graphWindow->setWindowTitle("Графики времени ответа");
    graphWindow->resize(1200, 800);
    
    QVBoxLayout *graphLayout = new QVBoxLayout(graphWindow);
    
    graphWidget = new GraphWidget(graphWindow);
    graphLayout->addWidget(graphWidget);
    
    graphWidget->setXAxisLabel("Время");
    graphWidget->setYAxisLabel("Время ответа (мс)");
    graphWidget->setDateTimeFormat("hh:mm:ss");
    graphWidget->setGridVisible(true);
    graphWidget->setLegendVisible(true);
    
    graphWindow->hide();
}

void WindowGraphics::processGraphData(const QString& jsonData)
{
    if (!graphManager) {
        appendOutput("Ошибка: менеджер графиков не инициализирован");
        return;
    }
    
    if (!graphManager->processPrometheusData(jsonData)) {
        appendOutput("Ошибка обработки данных для графиков");
    }
}

void WindowGraphics::clearGraphs()
{
    if (graphManager) {
        graphManager->clearAllGraphs();
    }
    if (graphWidget) {
        graphWidget->clear();
    }
}

void WindowGraphics::exportGraphs(const QString& filePath)
{
    if (graphManager && graphManager->exportToJson(filePath)) {
        appendOutput(QString("Графики экспортированы в %1").arg(filePath));
    } else {
        appendOutput(QString("Ошибка экспорта графиков в %1").arg(filePath));
    }
}

void WindowGraphics::importGraphs(const QString& filePath)
{
    if (graphManager && graphManager->importFromJson(filePath)) {
        appendOutput(QString("Графики импортированы из %1").arg(filePath));
    } else {
        appendOutput(QString("Ошибка импорта графиков из %1").arg(filePath));
    }
}

void WindowGraphics::setGraphOpacity(double opacity)
{
    if (graphManager) {
        graphManager->setGlobalOpacity(opacity);
    }
}

void WindowGraphics::onGraphDataReceived(const QString& dataJson)
{
    processGraphData(dataJson);
}

void WindowGraphics::onGraphDataUpdated()
{
    if (!graphManager || !graphWidget) {
        return;
    }
    
    QVector<GraphData> graphs = graphManager->getAllGraphs();
    
    if (!graphs.isEmpty()) {
        graphWidget->plotGraphs(graphs);
        
        if (graphWindow && !graphWindow->isVisible()) {
            graphWindow->show();
        } else if (graphWindow) {
            graphWindow->raise();
            graphWindow->activateWindow();
        }
        
        appendOutput(QString("Построено %1 графиков").arg(graphs.size()));
    } else {
        if (graphWidget) {
            graphWidget->clear();
        }
        if (graphWindow && graphWindow->isVisible()) {
            graphWindow->hide();
        }
    }
}

void WindowGraphics::updatePlayButtonState()
{
    playButton->setEnabled(hostsListWidget->count() > 0);
}

void WindowGraphics::updateFilePathLabel(const QString& text, bool success)
{
    filePathLabel->setText(text);
    filePathLabel->setStyleSheet(success ?
        "QLabel { color: green; font-size: 10pt; }" :
        "QLabel { color: red; font-size: 10pt; }");
}

void WindowGraphics::appendOutput(const QString& text)
{
    outputTextEdit->append(text);
}

void WindowGraphics::appendStatusBar(const QString& text)
{
    statusBar->showMessage(text);
}

void WindowGraphics::clearOutput()
{
    outputTextEdit->clear();
}

void WindowGraphics::addHostToList(const QString& hostInfo)
{
    hostsListWidget->addItem(hostInfo);
}

void WindowGraphics::removeHostFromList(int row)
{
    delete hostsListWidget->takeItem(row);
}