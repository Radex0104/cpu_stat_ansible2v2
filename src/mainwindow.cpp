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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Создаем графическую оболочку
    graphics = new WindowGraphics(this);
    setCentralWidget(graphics);

    configManager = new ConfigManager(this);
    ansibleRunner = new AnsibleRunner(this);

    loadSavedConfiguration();
    setupConnections();

    setAcceptDrops(true);
    setWindowTitle("CpuStatCheck");
    resize(600, 500);

    playbookPath = QCoreApplication::applicationDirPath() + "/../ansible.yml";
    playbookPath = QDir::cleanPath(playbookPath);
    
    qDebug() << "Playbook path:" << playbookPath;
    ansibleRunner->setPlaybookPath(playbookPath);

    connect(ansibleRunner, &AnsibleRunner::outputReceived, this, &MainWindow::onAnsibleOutput);
    connect(ansibleRunner, &AnsibleRunner::finished, this, &MainWindow::onAnsibleFinished);
    connect(ansibleRunner, &AnsibleRunner::errorOccurred, this, &MainWindow::onAnsibleError);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    // Ищем кнопки по property
    QPushButton* addButton = findChild<QPushButton*>("addHostButton");
    QPushButton* removeButton = findChild<QPushButton*>("removeHostButton");

    // Или можно добавить методы в WindowGraphics для доступа к кнопкам
    connect(graphics->getAddHostButton(), &QPushButton::clicked, this, &MainWindow::onAddHostClicked);
    connect(graphics->getRemoveHostButton(), &QPushButton::clicked, this, &MainWindow::removeHost);
    connect(graphics->getPlayButton(), &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
}

void MainWindow::loadSavedConfiguration()
{
    QString defaultUser;
    configManager->loadConfiguration(hostsConfig, defaultUser);

    for (const auto& host : hostsConfig) {
        graphics->addHostToList(host.address + " (" + host.sshUser + "@" + host.address + ")");
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
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

            if (fileInfo.isFile() && fileInfo.suffix() == "sh") {
                QString convertedPath;
                if (ansibleRunner->convertScriptToUnixFormat(filePath, convertedPath)) {
                    currentFilePath = convertedPath;
                    graphics->updateFilePathLabel("Выбран скрипт: " + fileInfo.fileName() + " (сконвертирован)", true);

                    if (ansibleRunner->updateScriptPathInPlaybook(playbookPath, currentFilePath)) {
                    }
                }
            } else if (fileInfo.isDir()) {
                QDir dir(filePath);
                QStringList scripts = dir.entryList(QStringList() << "*.sh", QDir::Files);

                if (!scripts.isEmpty()) {
                    QString scriptPath = dir.absoluteFilePath(scripts.first());
                    QString convertedPath;

                    if (ansibleRunner->convertScriptToUnixFormat(scriptPath, convertedPath)) {
                        currentFilePath = convertedPath;
                        graphics->updateFilePathLabel("Выбрана папка, найден скрипт: " + scripts.first() + " (сконвертирован)", true);

                        if (ansibleRunner->updateScriptPathInPlaybook(playbookPath, currentFilePath)) {
                        }
                    }
                } else {
                    graphics->updateFilePathLabel("В папке не найдено .sh файлов", false);
                }
            } else {
                graphics->updateFilePathLabel("Пожалуйста, выберите .sh файл", false);
            }
        }
    }
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

        // Формируем отображение с маскированным паролем или без пароля
        QString displayText;
        if (!host.sshPass.isEmpty()) {
            // Показываем только что пароль установлен, но не сам пароль
            displayText = QString("%1 (%2@%1) [пароль установлен]")
                .arg(host.address)
                .arg(host.sshUser);
        } else {
            displayText = QString("%1 (%2@%1) [без пароля]")
                .arg(host.address)
                .arg(host.sshUser);
        }

        graphics->addHostToList(displayText);
        hostsConfig.append(host);

        // Очищаем поля ввода, но оставляем пользователя и пароль, если нужно добавить еще один хост
        graphics->getNewHostEdit()->clear();
        // Не очищаем пользователя и пароль, чтобы можно было быстро добавить несколько хостов
        // graphics->getSshUserEdit()->clear();
        // graphics->getSshPasswordEdit()->clear();

        // Сохраняем конфигурацию
        // Важно: передаем текущего пользователя из поля, а не пустую строку
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

        // Сохраняем текущего пользователя и пароль из полей ввода
        QString currentUser = graphics->getSshUserEdit()->text();
        // Пароль не сохраняем отдельно, он уже в hostsConfig
        configManager->saveConfiguration(hostsConfig, currentUser);

        graphics->appendOutput("✅ Хост удален: " + removedHost);
    } else {
        showMessage("Выберите хост для удаления", true);
    }
}

void MainWindow::onPlayButtonClicked()
{
    if (currentFilePath.isEmpty()) {
        showMessage("Не выбран скрипт для выполнения", true);
        return;
    }

    if (hostsConfig.isEmpty()) {
        showMessage("Не добавлено ни одного хоста", true);
        return;
    }

    QFile playbookFile(playbookPath);
    if (!playbookFile.exists()) {
        showMessage("Файл ansible.yml не найден в папке проекта!", true);
        return;
    }

    graphics->clearOutput();
    ansibleRunner->setHosts(hostsConfig);
    ansibleRunner->setScriptPath(currentFilePath);
    ansibleRunner->executePlaybook();
}

void MainWindow::onAnsibleOutput(const QString& text)
{
    graphics->appendOutput(text);
}

void MainWindow::onAnsibleFinished(bool success, int exitCode)
{
    Q_UNUSED(success)
    Q_UNUSED(exitCode)
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
