#include "appsettings.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QStyle>
#include <QToolTip>
#include <QFont>
#include <QSettings>        // <-- ДОБАВИТЬ
#include <QVBoxLayout>      // <-- ДОБАВИТЬ
#include <QHBoxLayout>      // <-- ДОБАВИТЬ
#include <QGridLayout>      // <-- ДОБАВИТЬ
#include <QGroupBox>        // <-- ДОБАВИТЬ
#include <QLabel>           // <-- ДОБАВИТЬ
#include <QLineEdit>        // <-- ДОБАВИТЬ
#include <QPushButton>      // <-- ДОБАВИТЬ
#include <QSpinBox>         // <-- ДОБАВИТЬ
#include <QCheckBox>        // <-- ДОБАВИТЬ
#include <QTabWidget>       // <-- ДОБАВИТЬ
#include <QFileDialog>      // <-- ДОБАВИТЬ
#include <QMessageBox>      // <-- ДОБАВИТЬ
#include <QFrame>           // <-- ДОБАВИТЬ

const QString AppSettings::SETTINGS_FILE = "app_settings.conf";

// ==================== AppSettings ====================

AppSettings& AppSettings::instance()
{
    static AppSettings instance;
    return instance;
}

AppSettings::AppSettings()
    : QObject(nullptr)
    , m_prometheusHost("192.168.0.121")
    , m_testDuration(300)
    , m_localArchivePath("")
    , m_remoteTestDir("/opt/jmeter_tests")
    , m_generateReport(true)
    , m_resultsLocalDir("./jmeter_results")
    , m_resultsRemoteDir("/tmp/jmeter_results")
    , m_collectCpuMetrics(true)
    , m_collectMemoryMetrics(true)
    , m_jmeterHost("")  // Инициализация JMeter хоста
    , m_ansiblePlaybookPath("./ansible.yml")
    , m_defaultSshUser("astra1")
{
    loadFromFile();
}

AppSettings::~AppSettings()
{
    saveToFile();
}

// Prometheus
QString AppSettings::getPrometheusHost() const
{
    return m_prometheusHost;
}

void AppSettings::setPrometheusHost(const QString& host)
{
    if (m_prometheusHost != host) {
        m_prometheusHost = host;
        saveToFile();
        emit prometheusHostChanged(host);
        qDebug() << "Prometheus host changed to:" << host;
    }
}

QString AppSettings::getFullPrometheusUrl() const
{
    return QString("http://%1:%2").arg(m_prometheusHost).arg(PROMETHEUS_PORT);
}

// JMeter
int AppSettings::getTestDuration() const
{
    return m_testDuration;
}

void AppSettings::setTestDuration(int seconds)
{
    if (m_testDuration != seconds) {
        m_testDuration = seconds;
        saveToFile();
        emit jmeterSettingsChanged();
        qDebug() << "Test duration changed to:" << seconds << "seconds";
    }
}

QString AppSettings::getLocalArchivePath() const
{
    return m_localArchivePath;
}

void AppSettings::setLocalArchivePath(const QString& path)
{
    if (m_localArchivePath != path) {
        m_localArchivePath = path;
        saveToFile();
        emit jmeterSettingsChanged();
        qDebug() << "Local archive path changed to:" << path;
    }
}

QString AppSettings::getRemoteTestDir() const
{
    return m_remoteTestDir;
}

void AppSettings::setRemoteTestDir(const QString& path)
{
    if (m_remoteTestDir != path) {
        m_remoteTestDir = path;
        saveToFile();
        emit jmeterSettingsChanged();
    }
}

bool AppSettings::isGenerateReportEnabled() const
{
    return m_generateReport;
}

void AppSettings::setGenerateReportEnabled(bool enabled)
{
    if (m_generateReport != enabled) {
        m_generateReport = enabled;
        saveToFile();
        emit jmeterSettingsChanged();
    }
}

QString AppSettings::getResultsLocalDir() const
{
    return m_resultsLocalDir;
}

void AppSettings::setResultsLocalDir(const QString& path)
{
    if (m_resultsLocalDir != path) {
        m_resultsLocalDir = path;
        saveToFile();
        emit jmeterSettingsChanged();
    }
}

QString AppSettings::getResultsRemoteDir() const
{
    return m_resultsRemoteDir;
}

void AppSettings::setResultsRemoteDir(const QString& path)
{
    if (m_resultsRemoteDir != path) {
        m_resultsRemoteDir = path;
        saveToFile();
        emit jmeterSettingsChanged();
    }
}

bool AppSettings::isCollectCpuMetrics() const
{
    return m_collectCpuMetrics;
}

void AppSettings::setCollectCpuMetrics(bool enabled)
{
    if (m_collectCpuMetrics != enabled) {
        m_collectCpuMetrics = enabled;
        saveToFile();
        emit jmeterSettingsChanged();
    }
}

bool AppSettings::isCollectMemoryMetrics() const
{
    return m_collectMemoryMetrics;
}

void AppSettings::setCollectMemoryMetrics(bool enabled)
{
    if (m_collectMemoryMetrics != enabled) {
        m_collectMemoryMetrics = enabled;
        saveToFile();
        emit jmeterSettingsChanged();
    }
}

// JMeter host - ТОЛЬКО ОДНА РЕАЛИЗАЦИЯ
QString AppSettings::getJmeterHost() const
{
    return m_jmeterHost;
}

void AppSettings::setJmeterHost(const QString& host)
{
    if (m_jmeterHost != host) {
        m_jmeterHost = host;
        saveToFile();
        emit jmeterSettingsChanged();
        qDebug() << "JMeter host changed to:" << host;
    }
}

// Ansible
QString AppSettings::getAnsiblePlaybookPath() const
{
    return m_ansiblePlaybookPath;
}

void AppSettings::setAnsiblePlaybookPath(const QString& path)
{
    if (m_ansiblePlaybookPath != path) {
        m_ansiblePlaybookPath = path;
        saveToFile();
        emit ansibleSettingsChanged();
    }
}

QString AppSettings::getDefaultSshUser() const
{
    return m_defaultSshUser;
}

void AppSettings::setDefaultSshUser(const QString& user)
{
    if (m_defaultSshUser != user) {
        m_defaultSshUser = user;
        saveToFile();
        emit ansibleSettingsChanged();
    }
}

void AppSettings::loadFromFile()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configPath);
    QString filePath = configPath + "/" + SETTINGS_FILE;
    
    QSettings settings(filePath, QSettings::IniFormat);
    
    // Prometheus
    m_prometheusHost = settings.value("prometheus/host", m_prometheusHost).toString();
    
    // JMeter
    m_testDuration = settings.value("jmeter/duration_seconds", m_testDuration).toInt();
    m_localArchivePath = settings.value("jmeter/local_archive_path", m_localArchivePath).toString();
    m_remoteTestDir = settings.value("jmeter/remote_test_dir", m_remoteTestDir).toString();
    m_generateReport = settings.value("jmeter/generate_report", m_generateReport).toBool();
    m_resultsLocalDir = settings.value("jmeter/results_local_dir", m_resultsLocalDir).toString();
    m_resultsRemoteDir = settings.value("jmeter/results_remote_dir", m_resultsRemoteDir).toString();
    m_collectCpuMetrics = settings.value("jmeter/collect_cpu", m_collectCpuMetrics).toBool();
    m_collectMemoryMetrics = settings.value("jmeter/collect_memory", m_collectMemoryMetrics).toBool();
    m_jmeterHost = settings.value("jmeter/jmeter_host", m_jmeterHost).toString();  // Загрузка JMeter хоста
    
    // Ansible
    m_ansiblePlaybookPath = settings.value("ansible/playbook_path", m_ansiblePlaybookPath).toString();
    m_defaultSshUser = settings.value("ansible/default_ssh_user", m_defaultSshUser).toString();
    qDebug() << "Loaded local_archive_path:" << m_localArchivePath;  // <-- ДОБАВИТЬ
    qDebug() << "Loaded jmeter_host:" << m_jmeterHost;  // <-- ДОБАВИТЬ
    qDebug() << "Loaded test_duration:" << m_testDuration;  // <-- ДОБ
    qDebug() << "Settings loaded from:" << filePath;
}

void AppSettings::saveToFile() const
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configPath);
    QString filePath = configPath + "/" + SETTINGS_FILE;
    
    QSettings settings(filePath, QSettings::IniFormat);
    
    // Prometheus
    settings.setValue("prometheus/host", m_prometheusHost);
    
    // JMeter
    settings.setValue("jmeter/duration_seconds", m_testDuration);
    settings.setValue("jmeter/local_archive_path", m_localArchivePath);
    settings.setValue("jmeter/remote_test_dir", m_remoteTestDir);
    settings.setValue("jmeter/generate_report", m_generateReport);
    settings.setValue("jmeter/results_local_dir", m_resultsLocalDir);
    settings.setValue("jmeter/results_remote_dir", m_resultsRemoteDir);
    settings.setValue("jmeter/collect_cpu", m_collectCpuMetrics);
    settings.setValue("jmeter/collect_memory", m_collectMemoryMetrics);
    settings.setValue("jmeter/jmeter_host", m_jmeterHost);  // Сохранение JMeter хоста
    
    // Ansible
    settings.setValue("ansible/playbook_path", m_ansiblePlaybookPath);
    settings.setValue("ansible/default_ssh_user", m_defaultSshUser);
    
    settings.sync();
    qDebug() << "Settings saved to:" << filePath;
}

// ==================== SettingsDialog ====================

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , m_jmeterHostEdit(nullptr)  // Инициализация nullptr
{
    setupUI();
    loadCurrentSettings();
    setWindowTitle("Настройки приложения");
    setMinimumWidth(600);
    setMinimumHeight(500);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    m_tabWidget = new QTabWidget();
    
    // ========== Вкладка Prometheus ==========
    QWidget* prometheusTab = new QWidget();
    QVBoxLayout* prometheusLayout = new QVBoxLayout(prometheusTab);
    
    QGroupBox* prometheusGroup = new QGroupBox("Подключение к Prometheus");
    QGridLayout* prometheusGrid = new QGridLayout(prometheusGroup);
    
    prometheusGrid->addWidget(new QLabel("IP-адрес:"), 0, 0);
    m_prometheusIpEdit = new QLineEdit();
    m_prometheusIpEdit->setPlaceholderText("192.168.0.121");
    prometheusGrid->addWidget(m_prometheusIpEdit, 0, 1);
    
    prometheusGrid->addWidget(new QLabel("Порт:"), 1, 0);
    QLabel* portLabel = new QLabel("9090 (фиксированный)");
    portLabel->setStyleSheet("color: #666;");
    prometheusGrid->addWidget(portLabel, 1, 1);
    
    prometheusGrid->addWidget(new QLabel("Текущий URL:"), 2, 0);
    m_currentUrlLabel = new QLabel();
    m_currentUrlLabel->setStyleSheet("font-family: monospace; color: #2196F3;");
    m_currentUrlLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    prometheusGrid->addWidget(m_currentUrlLabel, 2, 1);
    
    m_testPrometheusButton = new QPushButton("Проверить подключение");
    m_testPrometheusButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; }");
    prometheusGrid->addWidget(m_testPrometheusButton, 3, 0, 1, 2);
    
    m_prometheusStatusLabel = new QLabel();
    m_prometheusStatusLabel->setVisible(false);
    prometheusGrid->addWidget(m_prometheusStatusLabel, 4, 0, 1, 2);
    
    prometheusLayout->addWidget(prometheusGroup);
    prometheusLayout->addStretch();
    
    // ========== Вкладка JMeter ==========
    QWidget* jmeterTab = new QWidget();
    QVBoxLayout* jmeterLayout = new QVBoxLayout(jmeterTab);
    
    QGroupBox* jmeterGroup = new QGroupBox("Настройки JMeter теста");
    QGridLayout* jmeterGrid = new QGridLayout(jmeterGroup);
    
    int row = 0;
    jmeterGrid->addWidget(new QLabel("Длительность теста (сек):"), row, 0);
    m_durationSpinBox = new QSpinBox();
    m_durationSpinBox->setRange(10, 36000);
    m_durationSpinBox->setSuffix(" сек");
    jmeterGrid->addWidget(m_durationSpinBox, row, 1);
    row++;
    
    jmeterGrid->addWidget(new QLabel("Архив с тестом (tar.gz/zip):"), row, 0);
    QHBoxLayout* archiveLayout = new QHBoxLayout();
    m_archivePathEdit = new QLineEdit();
    m_browseArchiveButton = new QPushButton("Обзор...");
    archiveLayout->addWidget(m_archivePathEdit);
    archiveLayout->addWidget(m_browseArchiveButton);
    jmeterGrid->addLayout(archiveLayout, row, 1);
    row++;
    
    jmeterGrid->addWidget(new QLabel("Директория распаковки на сервере:"), row, 0);
    m_remoteTestDirEdit = new QLineEdit();
    m_remoteTestDirEdit->setPlaceholderText("/opt/jmeter_tests");
    jmeterGrid->addWidget(m_remoteTestDirEdit, row, 1);
    row++;
    
    m_generateReportCheckBox = new QCheckBox("Генерировать HTML отчет");
    m_generateReportCheckBox->setChecked(true);
    jmeterGrid->addWidget(m_generateReportCheckBox, row, 0, 1, 2);
    row++;
    
    jmeterGrid->addWidget(new QLabel("Локальная директория результатов:"), row, 0);
    QHBoxLayout* resultsLayout = new QHBoxLayout();
    m_resultsLocalDirEdit = new QLineEdit();
    m_browseResultsButton = new QPushButton("Обзор...");
    resultsLayout->addWidget(m_resultsLocalDirEdit);
    resultsLayout->addWidget(m_browseResultsButton);
    jmeterGrid->addLayout(resultsLayout, row, 1);
    row++;
    
    jmeterGrid->addWidget(new QLabel("Удаленная директория результатов:"), row, 0);
    m_resultsRemoteDirEdit = new QLineEdit();
    m_resultsRemoteDirEdit->setPlaceholderText("/tmp/jmeter_results");
    jmeterGrid->addWidget(m_resultsRemoteDirEdit, row, 1);
    row++;
    
    m_collectCpuCheckBox = new QCheckBox("Собирать CPU метрики");
    m_collectMemoryCheckBox = new QCheckBox("Собирать Memory метрики");
    QHBoxLayout* metricsLayout = new QHBoxLayout();
    metricsLayout->addWidget(m_collectCpuCheckBox);
    metricsLayout->addWidget(m_collectMemoryCheckBox);
    jmeterGrid->addLayout(metricsLayout, row, 0, 1, 2);
    row++;
    
    // Добавляем поле для JMeter хоста
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    jmeterGrid->addWidget(line, row, 0, 1, 2);
    row++;
    
    QLabel* jmeterHostLabel = new QLabel("JMeter нагрузочный хост (IP):");
    jmeterHostLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    jmeterGrid->addWidget(jmeterHostLabel, row, 0, 1, 2);
    row++;
    
    QHBoxLayout* jmeterHostLayout = new QHBoxLayout();
    m_jmeterHostEdit = new QLineEdit();
    m_jmeterHostEdit->setPlaceholderText("192.168.0.100 (хост с установленным JMeter)");
    m_jmeterHostEdit->setToolTip("IP адрес сервера, на котором установлен JMeter для нагрузочного тестирования");
    jmeterHostLayout->addWidget(m_jmeterHostEdit);
    jmeterGrid->addLayout(jmeterHostLayout, row, 0, 1, 2);
    row++;
    
    QLabel* jmeterHostHint = new QLabel("ℹ️ На этом хосте должен быть установлен JMeter. Тесты будут запускаться с него на все остальные хосты.");
    jmeterHostHint->setStyleSheet("color: #666; font-size: 9pt;");
    jmeterGrid->addWidget(jmeterHostHint, row, 0, 1, 2);
    
    jmeterLayout->addWidget(jmeterGroup);
    jmeterLayout->addStretch();
    
    // ========== Вкладка Ansible ==========
    QWidget* ansibleTab = new QWidget();
    QVBoxLayout* ansibleLayout = new QVBoxLayout(ansibleTab);
    
    QGroupBox* ansibleGroup = new QGroupBox("Настройки Ansible");
    QGridLayout* ansibleGrid = new QGridLayout(ansibleGroup);
    
    ansibleGrid->addWidget(new QLabel("Путь к playbook:"), 0, 0);
    QHBoxLayout* playbookLayout = new QHBoxLayout();
    m_ansiblePlaybookEdit = new QLineEdit();
    m_browseAnsibleButton = new QPushButton("Обзор...");
    playbookLayout->addWidget(m_ansiblePlaybookEdit);
    playbookLayout->addWidget(m_browseAnsibleButton);
    ansibleGrid->addLayout(playbookLayout, 0, 1);
    
    ansibleGrid->addWidget(new QLabel("SSH пользователь по умолчанию:"), 1, 0);
    m_defaultSshUserEdit = new QLineEdit();
    ansibleGrid->addWidget(m_defaultSshUserEdit, 1, 1);
    
    ansibleLayout->addWidget(ansibleGroup);
    ansibleLayout->addStretch();
    
    m_tabWidget->addTab(prometheusTab, "Prometheus");
    m_tabWidget->addTab(jmeterTab, "JMeter");
    m_tabWidget->addTab(ansibleTab, "Ansible");
    
    mainLayout->addWidget(m_tabWidget);
    
    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_defaultsButton = new QPushButton("По умолчанию");
    m_saveButton = new QPushButton("Сохранить");
    m_cancelButton = new QPushButton("Отмена");
    
    m_saveButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px 20px; }");
    m_cancelButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px 20px; }");
    
    buttonLayout->addWidget(m_defaultsButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);
    
    // Подключаем сигналы
    connect(m_saveButton, &QPushButton::clicked, this, &SettingsDialog::onSaveClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);
    connect(m_testPrometheusButton, &QPushButton::clicked, this, &SettingsDialog::onTestPrometheusConnection);
    connect(m_browseArchiveButton, &QPushButton::clicked, this, &SettingsDialog::onSelectArchive);
    connect(m_browseResultsButton, &QPushButton::clicked, this, &SettingsDialog::onSelectResultsLocalDir);
    connect(m_browseAnsibleButton, &QPushButton::clicked, this, &SettingsDialog::onSelectAnsiblePlaybook);
    connect(m_defaultsButton, &QPushButton::clicked, this, &SettingsDialog::onRestoreDefaults);
}

void SettingsDialog::loadCurrentSettings()
{
    AppSettings& settings = AppSettings::instance();
    
    // Prometheus
    m_prometheusIpEdit->setText(settings.getPrometheusHost());
    m_currentUrlLabel->setText(settings.getFullPrometheusUrl());
    
    // JMeter
    m_durationSpinBox->setValue(settings.getTestDuration());
    m_archivePathEdit->setText(settings.getLocalArchivePath());
    m_remoteTestDirEdit->setText(settings.getRemoteTestDir());
    m_generateReportCheckBox->setChecked(settings.isGenerateReportEnabled());
    m_resultsLocalDirEdit->setText(settings.getResultsLocalDir());
    m_resultsRemoteDirEdit->setText(settings.getResultsRemoteDir());
    m_collectCpuCheckBox->setChecked(settings.isCollectCpuMetrics());
    m_collectMemoryCheckBox->setChecked(settings.isCollectMemoryMetrics());
    m_jmeterHostEdit->setText(settings.getJmeterHost());  // Загрузка JMeter хоста
    
    // Ansible
    m_ansiblePlaybookEdit->setText(settings.getAnsiblePlaybookPath());
    m_defaultSshUserEdit->setText(settings.getDefaultSshUser());
}

void SettingsDialog::onSaveClicked()
{
    AppSettings& settings = AppSettings::instance();
    
    // Сохраняем Prometheus
    settings.setPrometheusHost(m_prometheusIpEdit->text().trimmed());
    
    // Сохраняем JMeter
    settings.setTestDuration(m_durationSpinBox->value());
    settings.setLocalArchivePath(m_archivePathEdit->text());
    settings.setRemoteTestDir(m_remoteTestDirEdit->text());
    settings.setGenerateReportEnabled(m_generateReportCheckBox->isChecked());
    settings.setResultsLocalDir(m_resultsLocalDirEdit->text());
    settings.setResultsRemoteDir(m_resultsRemoteDirEdit->text());
    settings.setCollectCpuMetrics(m_collectCpuCheckBox->isChecked());
    settings.setCollectMemoryMetrics(m_collectMemoryCheckBox->isChecked());
    settings.setJmeterHost(m_jmeterHostEdit->text().trimmed());  // Сохранение JMeter хоста
    
    // Сохраняем Ansible
    settings.setAnsiblePlaybookPath(m_ansiblePlaybookEdit->text());
    settings.setDefaultSshUser(m_defaultSshUserEdit->text());
    
    accept();
}

void SettingsDialog::onCancelClicked()
{
    reject();
}

void SettingsDialog::onTestPrometheusConnection()
{
    QString testHost = m_prometheusIpEdit->text().trimmed();
    if (testHost.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите IP-адрес!");
        return;
    }
    
    m_prometheusStatusLabel->setVisible(true);
    m_prometheusStatusLabel->setText("⏳ Проверка подключения...");
    m_prometheusStatusLabel->setStyleSheet("color: #FF9800;");
    m_testPrometheusButton->setEnabled(false);
    
    QString testUrl = QString("http://%1:%2/api/v1/query?query=up")
                          .arg(testHost)
                          .arg(AppSettings::PROMETHEUS_PORT);
    
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkRequest request(testUrl);
    
    QNetworkReply* reply = manager->get(request);
    
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(5000);
    loop.exec();
    
    if (!timer.isActive()) {
        m_prometheusStatusLabel->setText("❌ Таймаут подключения (5 сек)");
        m_prometheusStatusLabel->setStyleSheet("color: #f44336;");
    } else if (reply->error() != QNetworkReply::NoError) {
        m_prometheusStatusLabel->setText(QString("❌ Ошибка: %1").arg(reply->errorString()));
        m_prometheusStatusLabel->setStyleSheet("color: #f44336;");
    } else {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 200) {
            m_prometheusStatusLabel->setText("✅ Подключение успешно!");
            m_prometheusStatusLabel->setStyleSheet("color: #4CAF50;");
        } else {
            m_prometheusStatusLabel->setText(QString("❌ Код ошибки: %1").arg(statusCode));
            m_prometheusStatusLabel->setStyleSheet("color: #f44336;");
        }
    }
    
    reply->deleteLater();
    manager->deleteLater();
    m_testPrometheusButton->setEnabled(true);
}

void SettingsDialog::onSelectResultsLocalDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, 
        "Выберите директорию для результатов",
        m_resultsLocalDirEdit->text());
    
    if (!dir.isEmpty()) {
        m_resultsLocalDirEdit->setText(dir);
    }
}

void SettingsDialog::onSelectAnsiblePlaybook()
{
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Выберите Ansible playbook", 
        QString(), 
        "YAML Files (*.yml *.yaml);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        m_ansiblePlaybookEdit->setText(fileName);
    }
}

void SettingsDialog::onRestoreDefaults()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
        "Сброс настроек", 
        "Вы уверены, что хотите восстановить настройки по умолчанию?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_prometheusIpEdit->setText("");
        m_durationSpinBox->setValue(300);
        m_archivePathEdit->clear();
        m_remoteTestDirEdit->setText("/opt/jmeter_tests");
        m_generateReportCheckBox->setChecked(true);
        m_resultsLocalDirEdit->setText("./jmeter_results");
        m_resultsRemoteDirEdit->setText("/tmp/jmeter_results");
        m_collectCpuCheckBox->setChecked(true);
        m_collectMemoryCheckBox->setChecked(true);
        m_ansiblePlaybookEdit->setText("./ansible.yml");
        m_defaultSshUserEdit->setText("astra1");
        m_jmeterHostEdit->setText("");
        
        m_currentUrlLabel->setText(QString("http://%1:9090").arg("192.168.0.121"));
    }
}

void SettingsDialog::onSelectArchive()
{
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Выберите архив с JMeter тестом", 
        QString(), 
        "Archive Files (*.tar.gz *.tgz *.zip *.tar.bz2);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        m_archivePathEdit->setText(fileName);
    }
}

// ==================== SettingsButton ====================

SettingsButton::SettingsButton(QWidget* parent)
    : QPushButton(parent)
{
    setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    setText("⚙ Настройки");
    setToolTip("Открыть настройки приложения");
    setStyleSheet(
        "QPushButton {"
        "    background-color: #607D8B;"
        "    color: white;"
        "    padding: 8px 15px;"
        "    font-size: 12pt;"
        "    border: none;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #455A64;"
        "}"
    );
    
    connect(this, &QPushButton::clicked, this, &SettingsButton::onButtonClicked);
    connect(&AppSettings::instance(), &AppSettings::prometheusHostChanged, 
            this, &SettingsButton::onPrometheusSettingsChanged);
    connect(&AppSettings::instance(), &AppSettings::jmeterSettingsChanged, 
            this, &SettingsButton::onJmeterSettingsChanged);
}

void SettingsButton::onButtonClicked()
{
    SettingsDialog dialog(parentWidget());
    if (dialog.exec() == QDialog::Accepted) {
        emit settingsChanged();
    }
}

void SettingsButton::onPrometheusSettingsChanged()
{
    // Можно обновить внешний вид кнопки или уведомить
}

void SettingsButton::onJmeterSettingsChanged()
{
    // Можно обновить внешний вид кнопки или уведомить
}