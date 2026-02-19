#include "ansiblerunner.h"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

AnsibleRunner::AnsibleRunner(QObject *parent)
    : QObject(parent)
    , ansibleProcess(nullptr)
{
    ansibleProcess = new QProcess(this);

    connect(ansibleProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &AnsibleRunner::onProcessFinished);
    connect(ansibleProcess, &QProcess::errorOccurred, this, &AnsibleRunner::onProcessErrorOccurred);
    connect(ansibleProcess, &QProcess::readyReadStandardOutput, this, &AnsibleRunner::readProcessOutput);
    connect(ansibleProcess, &QProcess::readyReadStandardError, this, &AnsibleRunner::readProcessOutput);

    inventoryPath = QCoreApplication::applicationDirPath() + "/inventory.ini";
}

AnsibleRunner::~AnsibleRunner()
{
    stop();
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

            // Базовые параметры
            stream << host.address;
            stream << " ansible_user=" << host.sshUser;

            // Добавляем пароль, если он есть
            if (!host.sshPass.isEmpty()) {
                // Для разных версий Ansible
                stream << " ansible_ssh_pass=" << host.sshPass;    // старый формат
                stream << " ansible_password=" << host.sshPass;    // новый формат
            }

            // Явно указываем метод подключения
            stream << " ansible_connection=ssh";
            stream << " ansible_port=22";
            stream << " ansible_ssh_extra_args='-o PubkeyAuthentication=no -o PasswordAuthentication=yes'";

            stream << "\n";
        }

        stream << "\n[webservers:vars]\n";
        stream << "ansible_ssh_common_args='-o StrictHostKeyChecking=no -o PubkeyAuthentication=no -o PasswordAuthentication=yes'\n";

        // Добавляем глобальные переменные для паролей
        if (!hostsConfig.isEmpty() && !hostsConfig[0].sshPass.isEmpty()) {
            stream << "ansible_become_pass=" << hostsConfig[0].sshPass << "\n";
            stream << "ansible_sudo_pass=" << hostsConfig[0].sshPass << "\n";
        }

        file.close();

        // Для отладки показываем созданный inventory
        emit outputReceived("📄 Inventory файл создан");
        QFile debugFile(inventoryPath);
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

    QStringList arguments;
    arguments << "-i" << convertToWslPath(inventoryPath);
    arguments << convertToWslPath(playbookPath);
    //arguments << "-v";

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
        emit errorOccurred("Не удалось открыть файл для конвертации"); // Изменено здесь
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
        emit errorOccurred("Не удалось создать временный файл"); // Изменено здесь
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

    QStringList previewLines = content.split('\n').mid(0, 5);

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

void AnsibleRunner::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    bool success = (exitCode == 0 && status == QProcess::NormalExit);
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

    emit errorOccurred(errorMessage); // Изменено здесь
}

void AnsibleRunner::readProcessOutput()
{
    QString output = ansibleProcess->readAllStandardOutput();
    QString error = ansibleProcess->readAllStandardError();

    if (!output.isEmpty()) {
        emit outputReceived(output);
    }
    if (!error.isEmpty()) {
        emit outputReceived("<span style='color:red'>" + error + "</span>");
    }
}
