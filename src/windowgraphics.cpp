#include "windowgraphics.h"
#include "graphmanager.h"
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

#include <QCheckBox>

QT_CHARTS_USE_NAMESPACE

WindowGraphics::WindowGraphics(QWidget *parent)
    : QWidget(parent)
    , progressManager(new ProgressManager(this))
    , graphManager(new GraphManager(this))
    , m_graphManager(new GraphManager(this))
    , m_chartView(nullptr)
    , m_graphLayout(nullptr)
    , m_tabWidget(nullptr)
    , m_graphsVisible(false)
{
    setupUI();
    setAcceptDrops(true);
    progressManager->setProgressBar(progressBar);
}

WindowGraphics::~WindowGraphics()
{
    // Сначала отключаем сигналы
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

    // Кнопка показа/скрытия графиков
    QHBoxLayout* graphControlLayout = new QHBoxLayout();
    showGraphsButton = new QPushButton("▼ Показать графики");
    showGraphsButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2196F3;"
        "    color: white;"
        "    padding: 8px;"
        "    font-size: 12pt;"
        "    border: none;"
        "    border-radius: 5px;"
        "    text-align: left;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1976D2;"
        "}"
    );

    connect(showGraphsButton, &QPushButton::clicked, this, &WindowGraphics::onShowGraphsClicked);
    graphControlLayout->addWidget(showGraphsButton);
    
    // Кнопка обновления всех графиков (изначально скрыта)
    refreshAllGraphsButton = new QPushButton("Обновить все графики");
    refreshAllGraphsButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    padding: 8px;"
        "    font-size: 12pt;"
        "    border: none;"
        "    border-radius: 5px;"
        "    margin-left: 10px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
    );
    refreshAllGraphsButton->setVisible(false);
    connect(refreshAllGraphsButton, &QPushButton::clicked, this, &WindowGraphics::onRefreshAllGraphsClicked);
    graphControlLayout->addWidget(refreshAllGraphsButton);
    
    graphControlLayout->addStretch();
    mainLayout->addLayout(graphControlLayout);

    // Виджет с вкладками для графиков
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setMinimumHeight(400);
    m_tabWidget->setVisible(false);
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "    border: 1px solid #ccc;"
        "    background: white;"
        "}"
        "QTabBar::tab {"
        "    padding: 8px 16px;"
        "    margin-right: 2px;"
        "    background: #f0f0f0;"
        "}"
        "QTabBar::tab:selected {"
        "    background: #2196F3;"
        "    color: white;"
        "}"
    );
    mainLayout->addWidget(m_tabWidget);  // <-- ВОТ ЭТО БЫЛО ПРОПУЩЕНО!

    // Создаем вкладки с графиками
    setupGraphTabs();
    
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

void WindowGraphics::setupGraphTabs()
{
    // 1. Общий RPS (одна линия)
    addGraphTab("RPS (Total)", 
        "sum(rate(jmeter_requests_total[30s]))");
    
    // 2. RPS по каждому запросу (много линий)
    addGraphTab("RPS by Endpoint", 
        "sum(rate(jmeter_requests_total[30s])) by (label)");
    
    // 3. Среднее время ответа по каждому запросу
    addGraphTab("Avg Response Time", 
        "avg(jmeter_rt_summary) by (label)");
}

void WindowGraphics::addGraphTab(const QString& title, const QString& prometheusQuery)
{
    QWidget* tabWidget = new QWidget();
    QVBoxLayout* tabLayout = new QVBoxLayout(tabWidget);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    
    // Используем ZoomableChartView (с поддержкой резинки и колесика)
    ZoomableChartView* chartView = new ZoomableChartView();
    chartView->setRenderHint(QPainter::Antialiasing);
    
    // Сохраняем query
    chartView->setProperty("query", prometheusQuery);
    
    tabLayout->addWidget(chartView);
    m_tabWidget->addTab(tabWidget, title);
}

void WindowGraphics::onShowGraphsClicked()
{
    m_graphsVisible = !m_graphsVisible;
    
    if (m_graphsVisible) {
        m_tabWidget->setVisible(true);
        showGraphsButton->setText("▲ Скрыть графики");
        refreshAllGraphsButton->setVisible(true);
        
        // Используем m_graphManager
        for (int i = 0; i < m_tabWidget->count(); ++i) {
            QWidget* tab = m_tabWidget->widget(i);
            QChartView* chartView = tab->findChild<QChartView*>();
            
            if (chartView && !chartView->chart() && m_graphManager) {
                QString query = chartView->property("query").toString();
                m_graphManager->buildPrometheusChart(chartView, query);
            }
        }
    } else {
        m_tabWidget->setVisible(false);
        showGraphsButton->setText("▼ Показать графики");
        refreshAllGraphsButton->setVisible(false);
    }
}

void WindowGraphics::onRefreshAllGraphsClicked()
{
    qDebug() << "Refreshing all graphs...";
    
    if (!m_graphManager) {
        qDebug() << "GraphManager is null!";
        return;
    }
    
    // Обновляем оси Y
    m_graphManager->forceUpdateYAxis();
    
    // Перестраиваем графики
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        QWidget* tab = m_tabWidget->widget(i);
        QChartView* chartView = tab->findChild<QChartView*>();
        
        if (chartView) {
            QString query = chartView->property("query").toString();
            m_graphManager->buildPrometheusChart(chartView, query);
        }
    }
    
    appendOutput("📊 Графики обновлены");
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
        createHostListItem(hostInfo, true);
    }
}

void WindowGraphics::removeHostFromList(int row)
{
    if (hostsListWidget && row >= 0 && row < hostsListWidget->count()) {
        delete hostsListWidget->takeItem(row);
    }
}

QListWidgetItem* WindowGraphics::createHostListItem(const QString& hostInfo, bool checked)
{
    QListWidgetItem* item = new QListWidgetItem(hostsListWidget);
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(5, 2, 5, 2);
    layout->setSpacing(10);
    
    QCheckBox* checkBox = new QCheckBox();
    checkBox->setChecked(checked);
    checkBox->setProperty("hostIndex", hostsListWidget->count());
    
    QLabel* label = new QLabel(hostInfo);
    label->setStyleSheet("QLabel { font-size: 10pt; }");
    
    layout->addWidget(checkBox);
    layout->addWidget(label);
    layout->addStretch();
    
    widget->setLayout(layout);
    item->setSizeHint(widget->sizeHint());
    
    hostsListWidget->addItem(item);
    hostsListWidget->setItemWidget(item, widget);
    
    return item;
}

QList<int> WindowGraphics::getSelectedHostIndices() const
{
    QList<int> selectedIndices;
    for (int i = 0; i < hostsListWidget->count(); ++i) {
        if (isHostChecked(i)) {
            selectedIndices.append(i);
        }
    }
    return selectedIndices;
}

WindowGraphics::HostListItem WindowGraphics::getHostListItem(int index) const
{
    HostListItem result;
    result.isChecked = false;
    result.displayText = "";
    
    if (index >= 0 && index < hostsListWidget->count()) {
        QListWidgetItem* item = hostsListWidget->item(index);
        QWidget* widget = hostsListWidget->itemWidget(item);
        if (widget) {
            QCheckBox* checkBox = widget->findChild<QCheckBox*>();
            QLabel* label = widget->findChild<QLabel*>();
            if (checkBox) result.isChecked = checkBox->isChecked();
            if (label) result.displayText = label->text();
        }
    }
    return result;
}

void WindowGraphics::setHostChecked(int index, bool checked)
{
    if (index >= 0 && index < hostsListWidget->count()) {
        QListWidgetItem* item = hostsListWidget->item(index);
        QWidget* widget = hostsListWidget->itemWidget(item);
        if (widget) {
            QCheckBox* checkBox = widget->findChild<QCheckBox*>();
            if (checkBox) checkBox->setChecked(checked);
        }
    }
}

bool WindowGraphics::isHostChecked(int index) const
{
    if (index >= 0 && index < hostsListWidget->count()) {
        QListWidgetItem* item = hostsListWidget->item(index);
        QWidget* widget = hostsListWidget->itemWidget(item);
        if (widget) {
            QCheckBox* checkBox = widget->findChild<QCheckBox*>();
            if (checkBox) return checkBox->isChecked();
        }
    }
    return false;
}