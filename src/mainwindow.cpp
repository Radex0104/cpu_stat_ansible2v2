#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QTimer>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentArchivePath(QString())
    , pythonProcess(nullptr)
{
    ui->setupUi(this);

    // Создаем графическую оболочку
    graphics = new WindowGraphics(this);
    setCentralWidget(graphics);
    checker = new WSLChecker(this);
    configManager = new ConfigManager(this);
    ansibleRunner = new AnsibleRunner(this);
    ansibleRunner->setProgressManager(graphics->getProgressManager());

    loadSavedConfiguration();
    setupConnections();

    setAcceptDrops(true);
    setWindowTitle("AnsibleDeploymentManager");
    resize(600, 500);

    playbookPath = QCoreApplication::applicationDirPath() + "/../ansible.yml";
    playbookPath = QDir::cleanPath(playbookPath);
    
    // Путь к Python скрипту
    pythonScriptPath = QCoreApplication::applicationDirPath() + "/../src/cicd.py";
    // pythonScriptPathWsl = "/mnt/c/Users/Admin/Desktop/practic/cpu_stat_ansible2v2/src/cicd.py";
    pythonScriptPath = QDir::cleanPath(pythonScriptPath);
    
    // // qDebug() << "Playbook path:" << playbookPath;
    // // qDebug() << "Python script path:" << pythonScriptPath;
    
    ansibleRunner->setPlaybookPath(playbookPath);
    AppSettings& settings = AppSettings::instance();
    ansibleRunner->setJmeterHost(settings.getJmeterHost());
    ansibleRunner->setJmeterArchiveSrc(settings.getLocalArchivePath());
    ansibleRunner->setJmeterRemoteTestDir(settings.getRemoteTestDir());
    ansibleRunner->setJmeterResultsRemoteDir(settings.getResultsRemoteDir());
    ansibleRunner->setJmeterResultsLocalDir(settings.getResultsLocalDir());
    ansibleRunner->setJmeterTestDuration(settings.getTestDuration());
    graphics->setupGraphTabs();
    QPushButton* showGraphsBtn = graphics->getShowGraphsButton();
        if (showGraphsBtn && showGraphsBtn->text().contains("Показать")) {
            showGraphsBtn->click();
        }
    connect(ansibleRunner, &AnsibleRunner::outputReceived, this, &MainWindow::onAnsibleOutput);
    connect(ansibleRunner, &AnsibleRunner::finished, this, &MainWindow::onAnsibleFinished);
    connect(ansibleRunner, &AnsibleRunner::errorOccurred, this, &MainWindow::onAnsibleError);
    connect(checker, SIGNAL(wslSetupFinished(bool)),
            this, SLOT(onWslSetupFinished(bool)));
}

MainWindow::~MainWindow()
{
    if (pythonProcess && pythonProcess->state() != QProcess::NotRunning) {
        pythonProcess->terminate();
        pythonProcess->waitForFinished(3000);
    }
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    
    static bool checked = false;
    if (!checked) {
        checked = true;
        QTimer::singleShot(100, this, &MainWindow::checkWSLAndShowStatus);
    }
}

void MainWindow::setupConnections()
{
    connect(graphics->getAddHostButton(), &QPushButton::clicked, this, &MainWindow::onAddHostClicked);
    connect(graphics->getRemoveHostButton(), &QPushButton::clicked, this, &MainWindow::removeHost);
    connect(graphics->getPlayButton(), &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
}

void MainWindow::loadSavedConfiguration()
{
    QString defaultUser;
    configManager->loadConfiguration(hostsConfig, defaultUser);

    for (int i = 0; i < hostsConfig.size(); ++i) {
        const auto& host = hostsConfig[i];
        QString displayText = host.address + " (" + host.sshUser + "@" + host.address + ")";
        graphics->addHostToList(displayText);
        // По умолчанию все хосты включены (чекбокс отмечен)
    }
}

void MainWindow::checkWSLAndShowStatus()
{
    // Выполняем синхронную проверку
    WSLChecker::WSLInfo info = checker->checkWSL();
    
    if (info.isInstalled) {
        if (info.hasDistributions) {
            QString status = "WSL готов: " + info.distributions.join(", ");
            graphics->appendStatusBar(status);
        } else {
            graphics->appendStatusBar("WSL установлен, но нет дистрибутивов");
            QTimer::singleShot(500, checker, &WSLChecker::showWslSetupDialog);
        }
    } else {
        graphics->appendStatusBar("WSL не установлен: " + info.errorMessage);
        QTimer::singleShot(500, checker, &WSLChecker::showWslSetupDialog);
    }
}

void MainWindow::onWslSetupFinished(bool success)
{
    if (success) {
        QMessageBox::information(this, "Установка завершена", 
            "Установка WSL завершена. После перезагрузки компьютера программа автоматически проверит наличие WSL.");
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::onWslCheckCompleted(const WSLChecker::WSLInfo &info)
{
    // // qDebug() << info.isInstalled;
    // // qDebug() << "WSL is installed:" << (info.isInstalled ? "yes" : "no");
    
    if (info.isInstalled) {
        if (info.hasDistributions) {
            QString status = "WSL ready: " + info.distributions.join(", ");
            graphics->appendStatusBar(status);
            // // qDebug() << "distr:" << info.distributions;
        } else {
            graphics->appendStatusBar("WSL is installed, but no distr");
        }
    } else {
        graphics->appendStatusBar("WSL isn't installed");
    }
}

void MainWindow::onWslCheckError(const QString &error)
{
    graphics->appendStatusBar("Ошибка проверки WSL");
}

void MainWindow::setArchivePath(const QString& path)
{
    currentArchivePath = path;
    QFileInfo fileInfo(path);
    
    if (!path.isEmpty()) {
        graphics->updateFilePathLabel("Архив загружен: " + fileInfo.fileName(), true);
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();

        if (!urlList.isEmpty()) {
            QString filePath = urlList.first().toLocalFile();
            QFileInfo fileInfo(filePath);
            
            // // qDebug() << "=== DROP EVENT ===";
            // // qDebug() << "File path:" << filePath;
            // // qDebug() << "Is file:" << fileInfo.isFile();
            // // qDebug() << "Is dir:" << fileInfo.isDir();
            // // qDebug() << "Suffix:" << fileInfo.suffix();
            // // qDebug() << "Complete suffix:" << fileInfo.completeSuffix();
            
            emit fileDropped(fileInfo);
            
            if (fileInfo.isFile()) {
                QString suffix = fileInfo.suffix().toLower();
                QString completeSuffix = fileInfo.completeSuffix().toLower();
                
                // // qDebug() << "Checking suffixes - suffix:" << suffix << "completeSuffix:" << completeSuffix;
                
                if (suffix == "gz" || suffix == "tgz" || suffix == "tar" || 
                    suffix == "zip" || suffix == "bz2" || completeSuffix == "tar.bz2") {
                    
                    // // qDebug() << "Archive file detected";
                    
                    QString archivePath = filePath;
                    currentArchivePath = archivePath;
                    setArchivePath(filePath);
                    graphics->updateFilePathLabel("Выбран архив: " + fileInfo.fileName(), true);
                    
                    if (ansibleRunner->updateArchivePathInPlaybook(playbookPath, archivePath)) {
                        graphics->appendOutput("📦 Архив добавлен: " + fileInfo.fileName());
                    }
                } else {
                    // // qDebug() << "File is not an archive";
                }
            }
            else if (fileInfo.isDir()) {
                // // qDebug() << "Directory detected";
                
                QString foundArchivePath;
                
                if (ansibleRunner->filesFinder(filePath, &foundArchivePath)) {
                    // // qDebug() << "filesFinder returned:" << foundArchivePath;
                    
                    if (!foundArchivePath.isEmpty()) {
                        currentArchivePath = foundArchivePath;
                        
                        graphics->updateFilePathLabel(
                            "Выбрана папка: " + fileInfo.fileName() + "\nНайден архив: " + QFileInfo(foundArchivePath).fileName(), 
                            true
                        );
                        
                        if (ansibleRunner->updateArchivePathInPlaybook(playbookPath, foundArchivePath)) {
                            graphics->appendOutput("📦 Архив найден в папке: " + QFileInfo(foundArchivePath).fileName());
                        }
                    } else {
                        // // qDebug() << "No archive found in directory";
                        graphics->updateFilePathLabel("В папке не найдено архивов", false);
                    }
                } else {
                    // // qDebug() << "filesFinder failed";
                }
            }
            
            // // qDebug() << "currentArchivePath after drop:" << currentArchivePath;
        }
    }
}

// Запуск Python скрипта для получения данных из Prometheus
void MainWindow::fetchPrometheusData()
{
    if (!QFile::exists(pythonScriptPath)) {
        graphics->appendOutput("⚠️ Python скрипт не найден: " + pythonScriptPath);
        return;
    }
    
    graphics->appendOutput("📊 Запрос данных из Prometheus...");
    
    // Запускаем Python процесс
    pythonProcess = new QProcess(this);
    
    QStringList arguments;
    arguments << pythonScriptPath;
    
    
    connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onPythonFinished);
    
    pythonProcess->start("python3", arguments);
    
    // Таймаут 30 секунд
    QTimer::singleShot(30000, this, [this]() {
        if (pythonProcess && pythonProcess->state() != QProcess::NotRunning) {
            pythonProcess->terminate();
            graphics->appendOutput("⚠️ Таймаут получения данных из Prometheus");
        }
    });
}

void MainWindow::onPythonFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0) {
        graphics->appendOutput("✅ Данные из Prometheus успешно получены");
    } else {
        graphics->appendOutput("⚠️ Ошибка выполнения Python скрипта (код: " + QString::number(exitCode) + ")");
    }
    
    if (pythonProcess) {
        pythonProcess->deleteLater();
        pythonProcess = nullptr;
    }
}

void MainWindow::onPlayButtonClicked()
{
    if (currentArchivePath.isEmpty()) {
        showMessage("Не выбран архив для установки", true);
        return;
    }
    AppSettings& settings = AppSettings::instance();
    ansibleRunner->setJmeterTestDuration(settings.getTestDuration());

    // Получаем только выбранные хосты
    QList<int> selectedIndices = graphics->getSelectedHostIndices();
    
    if (selectedIndices.isEmpty()) {
        showMessage("Не выбран ни один хост для выполнения (отметьте чекбоксы)", true);
        return;
    }

    // Собираем только выбранные хосты
    QList<HostConfig> selectedHosts;
    for (int idx : selectedIndices) {
        if (idx < hostsConfig.size()) {
            selectedHosts.append(hostsConfig[idx]);
        }
    }

    QFile playbookFile(playbookPath);
    if (!playbookFile.exists()) {
        showMessage("Файл ansible.yml не найден в папке проекта!", true);
        return;
    }

    graphics->clearOutput();
    graphics->appendOutput(QString("🚀 Запуск на %1 из %2 хостов").arg(selectedHosts.size()).arg(hostsConfig.size()));
    ansibleRunner->setHosts(selectedHosts);
    ansibleRunner->executePlaybook();
}

void MainWindow::onAnsibleOutput(const QString& text)
{
    graphics->appendOutput(text);
}

void MainWindow::onAddHostClicked()
{
    if (!graphics->getNewHostEdit()->text().isEmpty()) {
        if (graphics->getSshUserEdit()->text().isEmpty()) {
            showMessage("Укажите SSH пользователя для подключения к серверу", true);
            return;
        }

        // Проверяем, что пароль введен
        if (graphics->getSshPasswordEdit()->text().isEmpty()) {
            showMessage("Укажите пароль SSH для подключения к серверу", true);
            return;
        }

        HostConfig host;
        host.address = graphics->getNewHostEdit()->text();
        host.sshUser = graphics->getSshUserEdit()->text();
        host.sshPass = graphics->getSshPasswordEdit()->text(); // Сохраняем пароль

        QString displayText;
        displayText = QString("%1 (%2@%1)")
            .arg(host.address)
            .arg(host.sshUser);

        graphics->addHostToList(displayText);
        hostsConfig.append(host);

        graphics->getNewHostEdit()->clear();
        graphics->getSshUserEdit()->clear();
        graphics->getSshPasswordEdit()->clear();
        
        QString currentUser = graphics->getSshUserEdit()->text();
        configManager->saveConfiguration(hostsConfig, currentUser);

        // Добавляем сообщение в вывод
        graphics->appendOutput("✅ Хост добавлен: " + host.address + " (пользователь: " + host.sshUser + ")");
    } else {
        showMessage("Введите адрес хоста (IP или домен)", true);
    }
}

void MainWindow::removeHost()
{
    int row = graphics->getHostsListWidget()->currentRow();
    if (row >= 0) {
        QString removedHost = hostsConfig[row].address;
        graphics->removeHostFromList(row);
        hostsConfig.removeAt(row);

        // Сохраняем конфигурацию
        QString currentUser = graphics->getSshUserEdit()->text();
        configManager->saveConfiguration(hostsConfig, currentUser);

        graphics->appendOutput("✅ Хост удален: " + removedHost);
    } else {
        showMessage("Выберите хост для удаления", true);
    }
}

void MainWindow::onAnsibleFinished(bool success, int exitCode)
{
    if (success) {
        graphics->appendOutput("✅ Ansible выполнен успешно");
        graphics->appendOutput("📊 Загрузка графиков...");
        
        // Правильный поиск кнопки
        QPushButton* showGraphsBtn = graphics->getShowGraphsButton();
        if (showGraphsBtn && showGraphsBtn->text().contains("Показать")) {
            showGraphsBtn->click();
        }
        
        graphics->appendOutput("✅ Графики загружены");
    } else {
        graphics->appendOutput("❌ Ошибка выполнения Ansible (код: " + QString::number(exitCode) + ")");
    }
}

void MainWindow::onAnsibleError(const QString& message)
{
    showMessage(message, true);
}

void MainWindow::showMessage(const QString &message, bool isError)
{
    if (isError) {
        QMessageBox::critical(this, "Ошибка", message);
    } else {
        QMessageBox::information(this, "Информация", message);
    }
}