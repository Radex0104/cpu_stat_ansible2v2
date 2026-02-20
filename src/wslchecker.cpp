#include "wslchecker.h"
#include <QTimer>
#include <QMessageBox>
#include <QPushButton>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QClipboard>
#include <QGuiApplication>
#include <QApplication>

// Конструктор
WSLChecker::WSLChecker(QObject *parent) : QObject(parent)
{
    m_installProcess = new QProcess(this);
    m_ansibleProcess = new QProcess(this);
    m_distroProcess = new QProcess(this);
    m_versionCheckProcess = new QProcess(this); // Процесс для проверки версии
    m_ansibleInstallProcess = new QProcess(this);
    
    connect(m_installProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onInstallProcessFinished(int, QProcess::ExitStatus)));
    
    // Подключаем слоты для процесса установки Ansible
    connect(m_ansibleInstallProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onAnsibleInstallFinished(int, QProcess::ExitStatus)));
    
    connect(m_ansibleInstallProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(onAnsibleInstallOutput()));
    
    connect(m_ansibleInstallProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(onAnsibleInstallError()));
    
    connect(m_distroProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onDistroInstallFinished(int, QProcess::ExitStatus)));
    
    connect(m_distroProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(onDistroOutput()));
    
    connect(m_distroProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(onDistroError()));
    
    // Подключаем слот для проверки версии
    connect(m_versionCheckProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onVersionCheckFinished(int, QProcess::ExitStatus)));
    
    qDebug() << "WSLChecker создан";
}

// Деструктор
WSLChecker::~WSLChecker()
{
    // Завершаем все запущенные процессы при удалении
    if (m_installProcess && m_installProcess->state() != QProcess::NotRunning) {
        m_installProcess->terminate();
        m_installProcess->waitForFinished(1000);
    }
    if (m_ansibleProcess && m_ansibleProcess->state() != QProcess::NotRunning) {
        m_ansibleProcess->terminate();
        m_ansibleProcess->waitForFinished(1000);
    }
    if (m_distroProcess && m_distroProcess->state() != QProcess::NotRunning) {
        m_distroProcess->terminate();
        m_distroProcess->waitForFinished(1000);
    }
    if (m_versionCheckProcess && m_versionCheckProcess->state() != QProcess::NotRunning) {
        m_versionCheckProcess->terminate();
        m_versionCheckProcess->waitForFinished(1000);
    }
    if (m_ansibleInstallProcess && m_ansibleInstallProcess->state() != QProcess::NotRunning) {
        m_ansibleInstallProcess->terminate();
        m_ansibleInstallProcess->waitForFinished(1000);
    }
    qDebug() << "WSLChecker уничтожен";
}

// Слот для вывода Ansible (обычный вывод)
void WSLChecker::onAnsibleOutput()
{
    qDebug() << "onAnsibleOutput вызван";
    
    if (m_ansibleDialog && m_ansibleOutput) {
        QByteArray output = m_ansibleProcess->readAllStandardOutput();
        QString text = QString::fromLocal8Bit(output);
        if (!text.isEmpty()) {
            m_ansibleOutput->append(text);
            
            // Прокручиваем вниз
            QTextCursor cursor = m_ansibleOutput->textCursor();
            cursor.movePosition(QTextCursor::End);
            m_ansibleOutput->setTextCursor(cursor);
        }
    }
}

// Слот для ошибок Ansible
void WSLChecker::onAnsibleError()
{
    qDebug() << "onAnsibleError вызван";
    
    if (m_ansibleDialog && m_ansibleOutput) {
        QByteArray error = m_ansibleProcess->readAllStandardError();
        QString text = QString::fromLocal8Bit(error);
        if (!text.isEmpty()) {
            m_ansibleOutput->append("<font color='red'>" + text + "</font>");
            
            // Прокручиваем вниз
            QTextCursor cursor = m_ansibleOutput->textCursor();
            cursor.movePosition(QTextCursor::End);
            m_ansibleOutput->setTextCursor(cursor);
        }
    }
}

// Слот для вывода процесса установки Ansible
void WSLChecker::onAnsibleInstallOutput()
{
    if (m_installDialog && m_installOutput) {
        QByteArray output = m_ansibleInstallProcess->readAllStandardOutput();
        QString text = QString::fromLocal8Bit(output);
        m_installOutput->append(text);
        
        QTextCursor cursor = m_installOutput->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_installOutput->setTextCursor(cursor);
        
        // Проверяем, не запрашивается ли пароль sudo
        if (text.contains("password", Qt::CaseInsensitive) && 
            (text.contains("sudo") || text.contains("password for"))) {
            m_installOutput->append("\n⚠️ ТРЕБУЕТСЯ ПАРОЛЬ SUDO - Введите ваш пароль выше\n");
        }
    }
}

// Слот для ошибок процесса установки Ansible
void WSLChecker::onAnsibleInstallError()
{
    if (m_installDialog && m_installOutput) {
        QByteArray error = m_ansibleInstallProcess->readAllStandardError();
        QString text = QString::fromLocal8Bit(error);
        m_installOutput->append("<font color='red'>" + text + "</font>");
    }
}

// Слот завершения проверки версии Ansible
void WSLChecker::onVersionCheckFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    
    QString output = m_versionCheckProcess->readAllStandardOutput().trimmed();
    QString error = m_versionCheckProcess->readAllStandardError().trimmed();
    
    qDebug() << "Результат проверки версии:" << output;
    if (!error.isEmpty()) {
        qDebug() << "Ошибка проверки версии:" << error;
    }
    
    if (output.isEmpty() || output == "VERSION_ERROR" || output.contains("not found")) {
        m_lastInfo.ansibleInstalled = false;
        m_lastInfo.ansibleVersion = QString();
        qDebug() << "Ansible не установлен или не удалось получить версию";
        
        // Предлагаем установить Ansible
        QTimer::singleShot(500, this, &WSLChecker::offerAnsibleInstallation);
    } else {
        m_lastInfo.ansibleInstalled = true;
        m_lastInfo.ansibleVersion = output;
        qDebug() << "Обнаружена версия Ansible:" << output;
    }
    
    emit ansibleInfoUpdated(m_lastInfo.ansibleInstalled, m_lastInfo.ansibleVersion);
}

// Предложение установить Ansible
void WSLChecker::offerAnsibleInstallation()
{
    qDebug() << "Предложение установки Ansible...";
    
    if (!m_lastInfo.isInstalled || !m_lastInfo.hasDistributions) {
        qDebug() << "Невозможно установить Ansible: WSL не настроен правильно";
        return;
    }
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle("Ansible не найден");
    msgBox.setText("Ansible не установлен в вашем дистрибутиве WSL.");
    msgBox.setInformativeText(
        "Хотите установить Ansible сейчас?\n\n"
        "Будут выполнены следующие команды в вашем дистрибутиве WSL:\n"
        "• sudo apt update\n"
        "• sudo apt install software-properties-common\n"
        "• sudo apt-add-repository ppa:ansible/ansible\n"
        "• sudo apt install ansible\n\n"
        "Примечание: Во время установки может потребоваться ввод пароля sudo."
    );
    
    QPushButton *installButton = msgBox.addButton("Установить Ansible", QMessageBox::AcceptRole);
    QPushButton *laterButton = msgBox.addButton("Позже", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == installButton) {
        qDebug() << "Пользователь выбрал установку Ansible";
        installAnsibleInWSL();
    } else {
        qDebug() << "Пользователь отложил установку Ansible";
    }
}

// Установка Ansible в WSL
void WSLChecker::installAnsibleInWSL()
{
    qDebug() << "Открытие WSL с инструкциями по установке Ansible...";
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle("Установка Ansible в WSL");
    msgBox.setText("Следуйте этим шагам для установки Ansible:");
    msgBox.setInformativeText(
        "1. Терминал WSL откроется\n"
        "2. Скопируйте и вставьте эти команды по одной:\n\n"
        "sudo apt update\n"
        "sudo apt install -y software-properties-common\n"
        "sudo apt-add-repository --yes --update ppa:ansible/ansible\n"
        "sudo apt install -y ansible\n"
        "ansible --version\n\n"
        "Вам будет предложено ввести пароль sudo."
    );
    
    QPushButton *openWslButton = msgBox.addButton("Открыть WSL", QMessageBox::AcceptRole);
    QPushButton *closeButton = msgBox.addButton("Закрыть", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == openWslButton) {
        // СПОСОБ 1: Запуск через cmd.exe (самый надежный)
        QStringList args;
        args << "/c" << "start" << "wsl.exe" << "bash";
        
        bool started = QProcess::startDetached("cmd.exe", args);
        
        if (!started) {
            qDebug() << "Способ 1 не сработал, пробуем способ 2...";
            
            // СПОСОБ 2: Прямой запуск wsl.exe
            started = QProcess::startDetached("wsl.exe", QStringList());
        }
        
        if (!started) {
            qDebug() << "Способ 2 не сработал, пробуем способ 3...";
            
            // СПОСОБ 3: Запуск через полный путь
            QString wslPath = "C:\\Windows\\System32\\wsl.exe";
            started = QProcess::startDetached(wslPath, QStringList());
        }
        
        if (!started) {
            qDebug() << "Способ 3 не сработал, пробуем способ 4...";
            
            // СПОСОБ 4: Создаем временный batch-файл
            QString tempDir = QDir::tempPath();
            QString batchPath = tempDir + "/open_wsl.bat";
            
            QFile batchFile(batchPath);
            if (batchFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&batchFile);
                out << "@echo off\n";
                out << "start wsl.exe\n";
                out << "exit\n";
                batchFile.close();
                
                started = QProcess::startDetached("cmd.exe", QStringList() << "/c" << batchPath);
            }
        }
        
        if (started) {
            qDebug() << "WSL успешно запущен";
            
            QMessageBox msgBox(parent);
            msgBox.setWindowTitle("Команды для установки Ansible");
            msgBox.setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
            msgBox.setText("Скопируйте по очереди эти команды и нажмите ПКМ в терминале WSL:");
            
            QString commands = 
                "sudo apt update\n"
                "sudo apt install -y software-properties-common\n"
                "sudo apt-add-repository --yes --update ppa:ansible/ansible\n"
                "sudo apt install -y ansible\n"
                "ansible --version";
            
            msgBox.setInformativeText(commands);
            msgBox.setDetailedText(commands); // Создает текстовое поле с командами
            
            //QPushButton *copyButton = msgBox.addButton("Копировать в буфер", QMessageBox::ActionRole);
            QPushButton *closeButton = msgBox.addButton("Закрыть", QMessageBox::AcceptRole);
            
            msgBox.exec();
            
        //     if (msgBox.clickedButton() == copyButton) {
        //         QApplication::clipboard()->setText(commands);
        //         QMessageBox::information(parent, "Скопировано", "Команды скопированы в буфер обмена!");
        //     }
        // } else {
        //     qDebug() << "Все способы запуска WSL не сработали";
            
        //     QMessageBox::warning(parent, "Ошибка", 
        //         "Не удалось автоматически открыть WSL.\n\n"
        //         "Пожалуйста, откройте WSL вручную:\n"
        //         "• Меню Пуск → введите 'wsl' и нажмите Enter\n"
        //         "• Или нажмите Win+R, введите 'wsl' и нажмите Enter\n\n"
        //         "Затем выполните команды, показанные ранее.");
        }
    }
}

// Слот завершения установки Ansible
void WSLChecker::onAnsibleInstallFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    
    qDebug() << "Установка Ansible завершена с кодом:" << exitCode;
    
    if (m_installDialog) {
        if (exitCode == 0) {
            m_installOutput->append("\n✅ Ansible успешно установлен!");
            m_installOutput->append("\nТеперь вы можете использовать команду 'ansible-playbook' в WSL.");
            
            QMessageBox::information(m_installDialog, "Успех", 
                "Ansible был успешно установлен в WSL!");
            
            // Обновляем информацию об Ansible
            m_lastInfo.ansibleInstalled = true;
            checkAnsibleVersionAsync(); // Проверяем версию
            
        } else {
            QString error = m_ansibleInstallProcess->readAllStandardError();
            m_installOutput->append("\n❌ Установка не удалась с ошибкой:");
            m_installOutput->append(error);
            
            // Анализируем ошибку
            if (error.contains("sudo: a password is required") || 
                error.contains("no tty present")) {
                m_installOutput->append(
                    "\n⚠️ Эта ошибка обычно означает, что sudo требует пароль.\n"
                    "Сначала выполните 'sudo -v' в WSL для аутентификации, затем повторите попытку."
                );
                
                QMessageBox::warning(m_installDialog, "Ошибка прав доступа", 
                    "Установка не удалась из-за требований пароля sudo.\n\n"
                    "Пожалуйста, откройте терминал WSL и выполните 'sudo -v' сначала,\n"
                    "затем повторите установку Ansible.");
            } 
            else if (error.contains("Could not get lock") || error.contains("Unable to lock")) {
                m_installOutput->append(
                    "\n⚠️ Другой менеджер пакетов уже запущен.\n"
                    "Пожалуйста, закройте другие менеджеры пакетов и повторите попытку."
                );
                
                QMessageBox::warning(m_installDialog, "Ошибка блокировки", 
                    "Другой менеджер пакетов уже запущен.\n"
                    "Пожалуйста, закройте все другие менеджеры пакетов и повторите попытку.");
            }
            else {
                QMessageBox::warning(m_installDialog, "Ошибка установки", 
                    "Не удалось установить Ansible в WSL.\n\n" + error);
            }
        }
        
        m_installCloseButton->setEnabled(true);
        m_installProgressBar->setRange(0, 100);
        m_installProgressBar->setValue(100);
    }
    
    emit ansibleInstallFinished(exitCode == 0);
}

// Проверка наличия WSL и дистрибутивов
WSLChecker::WSLInfo WSLChecker::checkWSL()
{
    qDebug() << "Запуск проверки WSL с определением дистрибутивов...";
    
    WSLInfo info;
    
    // Проверяем наличие wsl.exe в системном пути
    QProcess whichProcess;
    whichProcess.start("where", QStringList() << "wsl.exe");
    
    if (whichProcess.waitForFinished(2000)) {
        QString whichOutput = whichProcess.readAllStandardOutput().trimmed();
        if (!whichOutput.isEmpty()) {
            qDebug() << "wsl.exe найден по адресу:" << whichOutput;
            info.isInstalled = true;
            
            // Пытаемся получить список установленных дистрибутивов
            QProcess listProcess;
            listProcess.start("wsl", QStringList() << "--list" << "--verbose");
            
            if (listProcess.waitForFinished(3000)) {
                QString output = QString::fromLocal8Bit(listProcess.readAllStandardOutput());
                qDebug() << "Сырой вывод wsl --list --verbose:" << output;
                
                // Разбираем вывод построчно
                QStringList lines = output.split('\n', QString::SkipEmptyParts);
                
                // Пропускаем заголовок (первую строку)
                for (int i = 1; i < lines.size(); ++i) {
                    QString line = lines[i].trimmed();
                    if (line.isEmpty()) continue;
                    
                    qDebug() << "Обработка строки:" << line;
                    
                    // Формат вывода: "  * Ubuntu-22.04    Running     2"
                    // Ищем название дистрибутива (обычно второй столбец)
                    QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                    
                    if (parts.size() >= 2) {
                        QString distroName;
                        
                        // Если первый символ '*', то имя во втором столбце
                        if (parts[0] == "*") {
                            distroName = parts[1];
                        } else {
                            distroName = parts[0];
                        }
                        
                        // Очищаем от лишних символов
                        distroName = distroName.trimmed();
                        distroName.remove('\r');
                        distroName.remove('\n');
                        
                        if (!distroName.isEmpty() && !distroName.startsWith("NAME")) {
                            info.distributions.append(distroName);
                            qDebug() << "Найден дистрибутив:" << distroName;
                        }
                    }
                }
                
                // Если не получилось через --verbose, пробуем --quiet
                if (info.distributions.isEmpty()) {
                    qDebug() << "Пробуем wsl --list --quiet как запасной вариант...";
                    
                    QProcess quietProcess;
                    quietProcess.start("wsl", QStringList() << "--list" << "--quiet");
                    
                    if (quietProcess.waitForFinished(3000)) {
                        QString quietOutput = QString::fromLocal8Bit(quietProcess.readAllStandardOutput());
                        QStringList quietLines = quietOutput.split('\n', QString::SkipEmptyParts);
                        
                        for (QString distro : quietLines) {
                            distro = distro.trimmed();
                            distro.remove('*');
                            distro.remove('\r');
                            distro.remove('\n');
                            
                            if (!distro.isEmpty()) {
                                info.distributions.append(distro);
                                qDebug() << "Найден дистрибутив (quiet):" << distro;
                            }
                        }
                    }
                }
                
                if (!info.distributions.isEmpty()) {
                    info.hasDistributions = true;
                    info.defaultDistribution = info.distributions.first();
                    
                    // Только теперь проверяем Ansible, так как есть дистрибутивы
                    qDebug() << "Дистрибутивы найдены, планируем проверку Ansible...";
                    QTimer::singleShot(100, this, &WSLChecker::checkAnsibleVersionAsync);
                    
                } else {
                    info.hasDistributions = false;
                    info.defaultDistribution = "Ubuntu";
                    info.ansibleInstalled = false;
                    info.ansibleVersion = QString();
                    qDebug() << "Дистрибутивы не найдены, используем Ubuntu по умолчанию";
                }
            } else {
                info.hasDistributions = false;
                info.defaultDistribution = "Ubuntu";
                info.ansibleInstalled = false;
                info.ansibleVersion = QString();
                qDebug() << "Не удалось получить список дистрибутивов, используем Ubuntu по умолчанию";
            }
            
            info.errorMessage = "";
        } else {
            qDebug() << "wsl.exe не найден в системном пути";
            info.isInstalled = false;
            info.hasDistributions = false;
            info.ansibleInstalled = false;
            info.ansibleVersion = QString();
            info.defaultDistribution = "";
            info.errorMessage = "WSL не установлен";
        }
    } else {
        qDebug() << "Не удалось проверить наличие wsl.exe (таймаут)";
        info.isInstalled = false;
        info.hasDistributions = false;
        info.ansibleInstalled = false;
        info.ansibleVersion = QString();
        info.defaultDistribution = "";
        info.errorMessage = "WSL не установлен";
    }
    
    m_lastInfo = info;
    
    qDebug() << "Проверка WSL завершена:";
    qDebug() << "  isInstalled:" << info.isInstalled;
    qDebug() << "  hasDistributions:" << info.hasDistributions;
    qDebug() << "  ansibleInstalled:" << info.ansibleInstalled;
    qDebug() << "  ansibleVersion:" << info.ansibleVersion;
    qDebug() << "  defaultDistribution:" << info.defaultDistribution;
    qDebug() << "  distributions:" << info.distributions;
    qDebug() << "  errorMessage:" << info.errorMessage;
    
    return info;
}

// Асинхронная проверка Ansible
void WSLChecker::checkAnsibleVersionAsync()
{
    qDebug() << "Запуск асинхронной проверки версии Ansible (WSL готов)...";
    
    // Дополнительная проверка с обновлением m_lastInfo
    if (!m_lastInfo.isInstalled || !m_lastInfo.hasDistributions) {
        qDebug() << "Невозможно проверить Ansible: WSL не настроен правильно";
        qDebug() << "  isInstalled:" << m_lastInfo.isInstalled;
        qDebug() << "  hasDistributions:" << m_lastInfo.hasDistributions;
        qDebug() << "  distributions:" << m_lastInfo.distributions;
        
        m_lastInfo.ansibleInstalled = false;
        m_lastInfo.ansibleVersion = QString();
        emit ansibleInfoUpdated(false, QString());
        return;
    }
    
    // Сначала проверим, можем ли мы вообще выполнить команду в WSL
    QProcess *testProcess = new QProcess(this);
    connect(testProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, testProcess](int, QProcess::ExitStatus) {
                QString output = testProcess->readAllStandardOutput().trimmed();
                qDebug() << "Результат теста WSL:" << output;
                
                if (output == "WSL_TEST_OK") {
                    // WSL работает, теперь проверим Ansible
                    checkAnsiblePresence();
                } else {
                    qDebug() << "Тест WSL не удался, невозможно проверить Ansible";
                    m_lastInfo.ansibleInstalled = false;
                    m_lastInfo.ansibleVersion = QString();
                    emit ansibleInfoUpdated(false, QString());
                }
                
                testProcess->deleteLater();
            });
    
    testProcess->start("wsl", QStringList() << "bash" << "-c" << "echo 'WSL_TEST_OK'");
}

void WSLChecker::checkAnsiblePresence()
{
    qDebug() << "Проверка наличия Ansible...";
    
    QProcess *checkProcess = new QProcess(this);
    connect(checkProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, checkProcess](int, QProcess::ExitStatus) {
                QString output = checkProcess->readAllStandardOutput().trimmed();
                QString error = checkProcess->readAllStandardError().trimmed();
                
                qDebug() << "Результат проверки наличия Ansible:" << output;
                if (!error.isEmpty()) {
                    qDebug() << "Ошибка проверки наличия Ansible:" << error;
                }
                
                if (output == "INSTALLED") {
                    qDebug() << "Ansible установлен, получаем версию...";
                    getAnsibleVersionAsync();
                } else {
                    qDebug() << "Ansible НЕ установлен";
                    m_lastInfo.ansibleInstalled = false;
                    m_lastInfo.ansibleVersion = QString();
                    emit ansibleInfoUpdated(false, QString());
                    
                    // Предлагаем установить Ansible
                    QTimer::singleShot(500, this, &WSLChecker::offerAnsibleInstallation);
                }
                
                checkProcess->deleteLater();
            });
    
    // Более надежная проверка наличия ansible
    QStringList args;
    args << "bash" << "-c" << 
        "if command -v ansible >/dev/null 2>&1; then "
        "    echo 'INSTALLED'; "
        "else "
        "    echo 'NOT_INSTALLED'; "
        "fi";
    
    checkProcess->start("wsl", args);
}

void WSLChecker::getAnsibleVersionAsync()
{
    qDebug() << "Получение версии Ansible...";
    
    QStringList args;
    args << "bash" << "-c" << "ansible --version 2>&1 | head -n 1 || echo 'VERSION_ERROR'";
    
    m_versionCheckProcess->start("wsl", args);
}

// Принудительное обновление информации об Ansible
void WSLChecker::refreshAnsibleInfo()
{
    qDebug() << "Обновление информации об Ansible...";
    
    if (m_lastInfo.isInstalled && m_lastInfo.hasDistributions) {
        checkAnsibleVersionAsync();
    } else {
        qDebug() << "Невозможно обновить информацию об Ansible: WSL не готов";
        m_lastInfo.ansibleInstalled = false;
        m_lastInfo.ansibleVersion = QString();
        emit ansibleInfoUpdated(false, QString());
    }
}

// Синхронная проверка наличия Ansible
bool WSLChecker::isAnsibleInstalled()
{
    qDebug() << "Синхронная проверка наличия Ansible...";
    
    if (!m_lastInfo.isInstalled || !m_lastInfo.hasDistributions) {
        qDebug() << "Невозможно проверить Ansible: WSL или дистрибутивы не доступны";
        return false;
    }
    
    QProcess checkProcess;
    QStringList args;
    args << "bash" << "-c" << "command -v ansible >/dev/null 2>&1 && echo 'INSTALLED' || echo 'NOT_INSTALLED'";
    
    checkProcess.start("wsl", args);
    
    if (!checkProcess.waitForFinished(5000)) {
        qDebug() << "Таймаут проверки Ansible";
        return false;
    }
    
    QString output = checkProcess.readAllStandardOutput().trimmed();
    bool installed = (output == "INSTALLED");
    
    qDebug() << "Ansible установлен (синхронная проверка):" << installed;
    return installed;
}

// Синхронное получение версии Ansible
QString WSLChecker::getAnsibleVersion()
{
    qDebug() << "Синхронное получение версии Ansible...";
    
    if (!m_lastInfo.isInstalled || !m_lastInfo.hasDistributions) {
        qDebug() << "Невозможно получить версию Ansible: WSL или дистрибутивы не доступны";
        return QString();
    }
    
    QProcess versionProcess;
    QStringList args;
    args << "bash" << "-c" << "ansible --version 2>/dev/null | head -n 1";
    
    versionProcess.start("wsl", args);
    
    if (!versionProcess.waitForFinished(5000)) {
        qDebug() << "Таймаут получения версии Ansible";
        return QString();
    }
    
    QString version = versionProcess.readAllStandardOutput().trimmed();
    
    if (!version.isEmpty()) {
        m_lastInfo.ansibleInstalled = true;
        m_lastInfo.ansibleVersion = version;
        qDebug() << "Версия Ansible (синхронно):" << version;
    } else {
        m_lastInfo.ansibleInstalled = false;
        m_lastInfo.ansibleVersion = QString();
    }
    
    return version;
}

// Показать диалог настройки WSL
void WSLChecker::showWslSetupDialog()
{
    qDebug() << "Показ диалога настройки WSL";
    qDebug() << "  isInstalled:" << m_lastInfo.isInstalled;
    qDebug() << "  hasDistributions:" << m_lastInfo.hasDistributions;
    
    if (!m_lastInfo.isInstalled) {
        // Случай 1: WSL не установлен
        showWslInstallDialog();
    } else if (!m_lastInfo.hasDistributions) {
        // Случай 2: WSL установлен, но нет дистрибутивов
        showDistroInstallDialog();
    } else {
        // Случай 3: WSL установлен с дистрибутивами
        qDebug() << "WSL установлен. Дистрибутивы:" << m_lastInfo.distributions;
        qDebug() << "Ansible установлен:" << m_lastInfo.ansibleInstalled;
        
        // Если Ansible не установлен, предлагаем установить
        if (!m_lastInfo.ansibleInstalled) {
            QTimer::singleShot(100, this, &WSLChecker::offerAnsibleInstallation);
        }
    }
}

// Диалог установки дистрибутива Ubuntu
void WSLChecker::showDistroInstallDialog()
{
    qDebug() << "Показ диалога установки дистрибутива";
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle("Дистрибутив Linux не найден");
    msgBox.setText("WSL установлен, но дистрибутив Linux не найден.");
    msgBox.setInformativeText("Эта программа требует дистрибутив Linux. "
                              "Хотите установить Ubuntu (рекомендуется)?");
    
    QPushButton *installButton = msgBox.addButton("Установить Ubuntu", QMessageBox::AcceptRole);
    QPushButton *cancelButton = msgBox.addButton("Отмена", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == installButton) {
        qDebug() << "Пользователь выбрал установку Ubuntu";
        installUbuntu();
    } else {
        qDebug() << "Пользователь отменил установку дистрибутива";
    }
}

// Установка дистрибутива Ubuntu
void WSLChecker::installUbuntu()
{
    qDebug() << "Запуск установки Ubuntu";
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    // Создаем диалог для отображения прогресса установки
    QDialog *progressDialog = new QDialog(parent);
    progressDialog->setWindowTitle("Установка Ubuntu");
    progressDialog->setMinimumWidth(500);
    progressDialog->setMinimumHeight(300);
    
    QVBoxLayout *layout = new QVBoxLayout(progressDialog);
    
    QLabel *titleLabel = new QLabel("<b>Установка Ubuntu в WSL</b>", progressDialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    QLabel *infoLabel = new QLabel("Это может занять несколько минут. Пожалуйста, подождите...", progressDialog);
    infoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoLabel);
    
    QTextEdit *outputText = new QTextEdit(progressDialog);
    outputText->setReadOnly(true);
    outputText->setFontFamily("Courier New");
    outputText->setMaximumHeight(200);
    layout->addWidget(outputText);
    
    QProgressBar *progressBar = new QProgressBar(progressDialog);
    progressBar->setRange(0, 0); // Неопределенный прогресс
    layout->addWidget(progressBar);
    
    QPushButton *closeButton = new QPushButton("Закрыть", progressDialog);
    closeButton->setEnabled(false);
    layout->addWidget(closeButton);
    
    // Сохраняем указатели для использования в слотах
    m_distroDialog = progressDialog;
    m_distroOutput = outputText;
    m_distroCloseButton = closeButton;
    m_distroProgressBar = progressBar;
    
    // Подключаем кнопку закрытия
    connect(closeButton, &QPushButton::clicked, progressDialog, &QDialog::accept);
    
    progressDialog->show();
    
    // Устанавливаем дистрибутив Ubuntu
    QStringList args;
    args << "--install" << "-d" << "Ubuntu";
    
    m_distroProcess->start("wsl", args);
    qDebug() << "Процесс установки Ubuntu запущен";
}

// Слот завершения установки дистрибутива
void WSLChecker::onDistroInstallFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    
    qDebug() << "Установка дистрибутива завершена, код:" << exitCode;
    
    if (m_distroDialog) {
        if (exitCode == 0) {
            m_distroOutput->append("\n✅ Ubuntu успешно установлен!");
            m_distroOutput->append("\nДистрибутив установлен. Возможно, потребуется:");
            m_distroOutput->append("1. Запустить Ubuntu из меню Пуск для завершения настройки");
            m_distroOutput->append("2. Создать имя пользователя и пароль");
            
            QMessageBox::information(m_distroDialog, "Успех", 
                "Ubuntu успешно установлен!\n\n"
                "Пожалуйста, запустите 'Ubuntu' из меню Пуск для завершения начальной настройки,\n"
                "затем мы сможем установить Ansible.");
            
            // Обновляем информацию о WSL после установки
            checkWSL();
            
            // Спрашиваем об установке Ansible
            if (m_lastInfo.hasDistributions) {
                QTimer::singleShot(1000, this, [this]() {
                    offerAnsibleInstallation();
                });
            }
        } else {
            QString error = m_distroProcess->readAllStandardError();
            m_distroOutput->append("\n❌ Установка не удалась с ошибкой:");
            m_distroOutput->append(error);
            
            QMessageBox::warning(m_distroDialog, "Ошибка установки", 
                "Не удалось установить Ubuntu.\n\n" + error);
        }
        
        m_distroCloseButton->setEnabled(true);
        m_distroProgressBar->setRange(0, 100);
        m_distroProgressBar->setValue(100);
    }
    
    emit distroInstallFinished(exitCode == 0);
}

// Слот вывода установки дистрибутива
void WSLChecker::onDistroOutput()
{
    if (m_distroDialog && m_distroOutput) {
        QByteArray output = m_distroProcess->readAllStandardOutput();
        QString text = QString::fromLocal8Bit(output);
        m_distroOutput->append(text);
        
        // Прокручиваем вниз
        QTextCursor cursor = m_distroOutput->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_distroOutput->setTextCursor(cursor);
    }
}

// Слот ошибок установки дистрибутива
void WSLChecker::onDistroError()
{
    if (m_distroDialog && m_distroOutput) {
        QByteArray error = m_distroProcess->readAllStandardError();
        QString text = QString::fromLocal8Bit(error);
        m_distroOutput->append("<font color='red'>" + text + "</font>");
    }
}

// Диалог установки WSL
void WSLChecker::showWslInstallDialog()
{
    qDebug() << "Показ диалога установки WSL";
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle("WSL не найден");
    msgBox.setText("Эта программа требует Windows Subsystem for Linux (WSL).");
    msgBox.setInformativeText("WSL не найден на этом компьютере. Хотите установить WSL?\n\n"
                              "Это также позволит вам установить Ubuntu и Ansible позже.");
    
    QPushButton *installButton = msgBox.addButton("Установить WSL", QMessageBox::AcceptRole);
    QPushButton *cancelButton = msgBox.addButton("Отмена", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == installButton) {
        qDebug() << "Пользователь выбрал установку WSL";
        installWsl();
    } else {
        qDebug() << "Пользователь отменил установку WSL";
    }
}

// Установка WSL
void WSLChecker::installWsl()
{
    qDebug() << "Запуск установки WSL";
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox::information(parent, "Установка WSL", 
        "Запуск установки WSL. Это может занять несколько минут.\n"
        "После установки потребуется перезагрузка компьютера.\n\n"
        "После перезагрузки программа поможет установить Ubuntu и Ansible.");
    
    QStringList args;
    args << "/c" << "wsl" << "--install";
    
    m_installProcess->start("cmd.exe", args);
    qDebug() << "Процесс установки WSL запущен с аргументами:" << args;
}

// Слот завершения установки WSL
void WSLChecker::onInstallProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    
    qDebug() << "Процесс установки WSL завершен, код:" << exitCode;
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    if (exitCode == 0) {
        qDebug() << "Установка WSL успешна";
        
        QMessageBox::information(parent, "Установка WSL завершена", 
            "WSL успешно установлен! Пожалуйста, перезагрузите компьютер.\n\n"
            "После перезагрузки программа поможет установить Ubuntu и Ansible.");
        
        emit wslSetupFinished(true);
    } else {
        QString error = m_installProcess->readAllStandardError();
        qDebug() << "Установка WSL не удалась, ошибка:" << error;
        QMessageBox::warning(parent, "Ошибка установки WSL", 
            "Не удалось установить WSL.\n\n" + error);
        emit wslSetupFinished(false);
    }
}