#include "ansiblerunner.h"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>

AnsibleRunner::AnsibleRunner(QObject *parent)
    : QObject(parent)
    , ansibleProcess(nullptr)
    , m_progressManager(nullptr)
    , m_currentTaskIndex(0)
{
    ansibleProcess = new QProcess(this);

    connect(ansibleProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &AnsibleRunner::onProcessFinished);
    connect(ansibleProcess, &QProcess::errorOccurred, this, &AnsibleRunner::onProcessErrorOccurred);
    connect(ansibleProcess, &QProcess::readyReadStandardOutput, this, &AnsibleRunner::readProcessOutput);
    connect(ansibleProcess, &QProcess::readyReadStandardError, this, &AnsibleRunner::readProcessOutput);

    inventoryPath = QCoreApplication::applicationDirPath() + "/inventory.ini";
    
    // Предопределенные задачи Ansible
    m_taskNames = QStringList() 
            << "Сбор информации о хостах"
            << "Подготовка окружения"
            << "Копирование скрипта на сервер"
            << "Установка прав на выполнение"
            << "Выполнение скрипта"
            << "Сбор результатов"
            << "Завершение";
}

AnsibleRunner::~AnsibleRunner()
{
    stop();
}

void AnsibleRunner::setProgressManager(ProgressManager* manager)
{
    m_progressManager = manager;
}

void AnsibleRunner::stop()
{
    if (ansibleProcess && ansibleProcess->state() == QProcess::Running) {
        ansibleProcess->terminate();
        ansibleProcess->waitForFinished(3000);
    }
}

void AnsibleRunner::setPlaybookPath(const QString& path)
{
    playbookPath = path;
}

void AnsibleRunner::setScriptPath(const QString& path)
{
    scriptPath = path;
}

void AnsibleRunner::setHosts(const QList<HostConfig>& hosts)
{
    hostsConfig = hosts;
}

void AnsibleRunner::createInventoryFile()
{
    QFile file(inventoryPath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << "[webservers]\n";

        for (int i = 0; i < hostsConfig.size(); ++i) {
            const HostConfig &host = hostsConfig[i];

            stream << host.address;
            stream << " ansible_user=" << host.sshUser;

            if (!host.sshPass.isEmpty()) {
                stream << " ansible_ssh_pass=" << host.sshPass;
                stream << " ansible_password=" << host.sshPass;
            }

            stream << " ansible_connection=ssh";
            stream << " ansible_port=22";
            stream << " ansible_ssh_extra_args='-o PubkeyAuthentication=no -o PasswordAuthentication=yes'";
            stream << "\n";
        }

        stream << "\n[webservers:vars]\n";
        stream << "ansible_ssh_common_args='-o StrictHostKeyChecking=no -o PubkeyAuthentication=no -o PasswordAuthentication=yes'\n";

        if (!hostsConfig.isEmpty() && !hostsConfig[0].sshPass.isEmpty()) {
            stream << "ansible_become_pass=" << hostsConfig[0].sshPass << "\n";
            stream << "ansible_sudo_pass=" << hostsConfig[0].sshPass << "\n";
        }

        file.close();
        emit outputReceived("📄 Inventory файл создан");
    } else {
        emit errorOccurred("Не удалось создать inventory файл");
    }
}

bool AnsibleRunner::updateScriptPathInPlaybook(const QString& playbookPath, const QString& scriptPath)
{
    if (scriptPath.isEmpty()) return false;

    QFile playbookFile(playbookPath);
    if (!playbookFile.exists()) {
        emit errorOccurred("Файл ansible.yml не найден в папке проекта!");
        return false;
    }

    if (!playbookFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred("Не удалось прочитать файл ansible.yml");
        return false;
    }

    QString content = QString::fromUtf8(playbookFile.readAll());
    playbookFile.close();

    QString wslScriptPath = convertToWslPath(scriptPath);

    QStringList lines = content.split("\n");
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].contains("script_src:")) {
            lines[i] = QString("    script_src: \"%1\"").arg(wslScriptPath);
            break;
        }
    }

    if (playbookFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        playbookFile.write(lines.join("\n").toUtf8());
        playbookFile.close();
        return true;
    }

    return false;
}

void AnsibleRunner::executePlaybook()
{
    createInventoryFile();

    emit outputReceived("🚀 Запуск Ansible playbook...");
    emit outputReceived("📋 Используется playbook: " + playbookPath);

    // Сброс индекса задачи
    m_currentTaskIndex = 0;
    
    // Запуск менеджера прогресса
    if (m_progressManager) {
        m_progressManager->startProgress(m_taskNames.size());
        m_progressManager->setStatusText("Подготовка к запуску...");
    }

    QStringList arguments;
    arguments << "-i" << convertToWslPath(inventoryPath);
    arguments << convertToWslPath(playbookPath);
    // arguments << "-v"; // Для более детального вывода

    emit outputReceived("\n⚡ Выполнение playbook...");
    emit outputReceived("Команда: ansible-playbook " + arguments.join(" "));

    QStringList wslArgs;
    wslArgs << "--" << "ansible-playbook" << arguments;
    ansibleProcess->start("wsl", wslArgs);
}

bool AnsibleRunner::convertScriptToUnixFormat(const QString& filePath, QString& convertedPath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred("Не удалось открыть файл для конвертации");
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    data.replace("\r\n", "\n");
    data.replace("\r", "\n");
    data.replace('\r', '\n');

    QString content = QString::fromUtf8(data);
    content = content.trimmed();

    if (!content.startsWith("#!")) {
        content = "#!/bin/bash\n\n" + content;
    }

    if (!content.endsWith('\n')) {
        content += '\n';
    }

    QString tempFilePath = QDir::temp().absoluteFilePath("script_converted.sh");
    QFile tempFile(tempFilePath);
    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("Не удалось создать временный файл");
        return false;
    }

    QTextStream out(&tempFile);
    out.setCodec("UTF-8");
    out << content;
    tempFile.close();

    QStringList chmodArgs;
    chmodArgs << "--" << "chmod" << "+x" << convertToWslPath(tempFilePath);

    QProcess chmod;
    chmod.start("wsl", chmodArgs);
    chmod.waitForFinished(3000);

    convertedPath = tempFilePath;
    emit outputReceived("🔄 Скрипт сконвертирован в Unix-формат и подготовлен к выполнению");

    return true;
}

QString AnsibleRunner::convertToWslPath(const QString& windowsPath) const
{
    QString wslPath = windowsPath;
    wslPath.replace('\\', '/');

    if (wslPath.contains(':')) {
        QString driveLetter = wslPath.left(1).toLower();
        wslPath = wslPath.mid(2);
        wslPath = QString("/mnt/%1%2").arg(driveLetter, wslPath);
    }

    return wslPath;
}

void AnsibleRunner::parseProgressFromOutput(const QString& output)
{
    if (!m_progressManager) return;

    // Анализируем вывод Ansible для определения текущей задачи
    
    // TASK [Gathering Facts]
    if (output.contains("TASK [Gathering Facts]")) {
        m_currentTaskIndex = 0;
        emit taskStarted("Сбор информации о хостах");
        m_progressManager->setStatusText("Сбор информации о хостах...");
    }
    // TASK [copy script]
    else if (output.contains("TASK [copy script]") || output.contains("TASK [Копирование]")) {
        m_currentTaskIndex = 2;
        emit taskStarted("Копирование скрипта");
        m_progressManager->setStatusText("Копирование скрипта на сервер...");
    }
    // TASK [make executable]
    else if (output.contains("TASK [make executable]") || output.contains("chmod")) {
        m_currentTaskIndex = 3;
        emit taskStarted("Установка прав");
        m_progressManager->setStatusText("Установка прав на выполнение...");
    }
    // TASK [execute script]
    else if (output.contains("TASK [execute script]") || output.contains("TASK [Выполнение]")) {
        m_currentTaskIndex = 4;
        emit taskStarted("Выполнение скрипта");
        m_progressManager->setStatusText("Выполнение скрипта на сервере...");
    }
    // PLAY RECAP
    else if (output.contains("PLAY RECAP")) {
        m_currentTaskIndex = 6;
        emit taskStarted("Завершение");
        m_progressManager->setStatusText("Завершение выполнения...");
    }
    
    // Обновляем прогресс на основе индекса задачи
    if (m_currentTaskIndex < m_taskNames.size()) {
        m_progressManager->updateProgress(m_currentTaskIndex + 1, m_taskNames[m_currentTaskIndex]);
    }
    
    // Парсим прогресс по хостам
    QRegularExpression hostProgressRegex("(\\w+)\\s*:\\s*ok=(\\d+)\\s*changed=(\\d+)\\s*unreachable=(\\d+)\\s*failed=(\\d+)");
    QRegularExpressionMatch match = hostProgressRegex.match(output);
    if (match.hasMatch()) {
        QString host = match.captured(1);
        int ok = match.captured(2).toInt();
        int changed = match.captured(3).toInt();
        int unreachable = match.captured(4).toInt();
        int failed = match.captured(5).toInt();
        
        QString status = QString("Хост %1: OK=%2, Изменено=%3")
            .arg(host).arg(ok).arg(changed);
        
        if (failed > 0) {
            status += QString(", Ошибок=%1").arg(failed);
        }
        
        m_progressManager->setStatusText(status);
    }
}

void AnsibleRunner::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    bool success = (exitCode == 0 && status == QProcess::NormalExit);
    
    if (m_progressManager) {
        m_progressManager->stopProgress(success);

        if (!success) {
            m_progressManager->setErrorMode(true);
        }
    }
    
    if (success) {
        emit outputReceived("\n✅ Playbook успешно выполнен на всех хостах!");
        emit outputReceived("\n✨ Развертывание завершено!");
    } else {
        emit outputReceived("\n❌ Ошибка при выполнении playbook (код: " + QString::number(exitCode) + ")");
    }
    emit finished(success, exitCode);
}

void AnsibleRunner::onProcessErrorOccurred(QProcess::ProcessError error)
{
    QString errorMessage;
    switch (error) {
        case QProcess::FailedToStart:
            errorMessage = "Не удалось запустить Ansible. Проверьте установку WSL и Ansible.";
            break;
        case QProcess::Crashed:
            errorMessage = "Ansible аварийно завершился.";
            break;
        case QProcess::Timedout:
            errorMessage = "Превышено время ожидания.";
            break;
        case QProcess::WriteError:
            errorMessage = "Ошибка записи.";
            break;
        case QProcess::ReadError:
            errorMessage = "Ошибка чтения.";
            break;
        default:
            errorMessage = "Неизвестная ошибка.";
    }

    if (m_progressManager) {
        m_progressManager->stopProgress(false);
        m_progressManager->setErrorMode(true); 
    }
    
    emit errorOccurred(errorMessage);
}

void AnsibleRunner::readProcessOutput()
{
    QString output = ansibleProcess->readAllStandardOutput();
    QString error = ansibleProcess->readAllStandardError();

    if (!output.isEmpty()) {
        emit outputReceived(output);
        parseProgressFromOutput(output);
    }
    if (!error.isEmpty()) {
        emit outputReceived("<span style='color:red'>" + error + "</span>");
        parseProgressFromOutput(error);
    }
}