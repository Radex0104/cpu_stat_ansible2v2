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
#include <QTextCodec>
#include "windows.h"
// Конструктор
WSLChecker::WSLChecker(QObject *parent) : QObject(parent)
{
    m_installProcess = new QProcess(this);
    m_ansibleProcess = new QProcess(this);
    m_distroProcess = new QProcess(this);
    m_versionCheckProcess = new QProcess(this); // Процесс для проверки версии
    m_ansibleInstallProcess = new QProcess(this);
    m_checkTimer = new QTimer(this);
    m_checkTimer->setInterval(3000); // Проверка каждые 3 секунды
    connect(m_checkTimer, &QTimer::timeout, this, &WSLChecker::periodicCheck);
    m_checkAttempts = 0;
    m_offerShown = false;
    
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
    
    // // qDebug() << "WSLChecker created";
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
    // // qDebug() << "WSLChecker destroyed";
}

// Слот для вывода Ansible (обычный вывод)
void WSLChecker::onAnsibleOutput()
{
    // // qDebug() << "onAnsibleOutput called";
    
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
    // // qDebug() << "onAnsibleError called";
    
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
    
    // // qDebug() << "Version:" << output;
    if (!error.isEmpty()) {
        // // qDebug() << "Error:" << error;
    }
    
    if (output.isEmpty() || output == "VERSION_ERROR" || output.contains("not found")) {
        m_lastInfo.ansibleInstalled = false;
        m_lastInfo.ansibleVersion = QString();
        
        // Предлагаем установить Ansible
        QTimer::singleShot(500, this, &WSLChecker::offerAnsibleInstallation);
    } else {
        m_lastInfo.ansibleInstalled = true;
        m_lastInfo.ansibleVersion = output;
    }
    
    emit ansibleInfoUpdated(m_lastInfo.ansibleInstalled, m_lastInfo.ansibleVersion);
}

// Предложение установить Ansible
void WSLChecker::offerAnsibleInstallation()
{
    // Если предложение уже было показано, не показываем повторно
    if (m_offerShown) {
        qDebug() << "Offer already shown, skipping";
        return;
    }
    
    // Обновляем информацию перед предложением
    m_lastInfo = checkWSL();
    
    if (!m_lastInfo.isInstalled || !m_lastInfo.hasDistributions) {
        // Нет дистрибутива - не предлагаем
        return;
    }
    
    // Проверяем Ansible
    QProcess checkProcess;
    checkProcess.start("wsl", QStringList() << "bash" << "-c" << "command -v ansible >/dev/null 2>&1 && echo 'INSTALLED' || echo 'NOT_INSTALLED'");
    
    if (!checkProcess.waitForFinished(5000)) {
        return;
    }
    
    QString output = checkProcess.readAllStandardOutput().trimmed();
    
    if (output == "INSTALLED") {
        // Ansible уже установлен
        m_lastInfo.ansibleInstalled = true;
        return;
    }
    
    // Устанавливаем флаг, что предложение будет показано
    m_offerShown = true;
    
    // Предлагаем установить Ansible
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle("Ansible не найден");
    msgBox.setText("Ansible не установлен в вашем дистрибутиве WSL.");
    msgBox.setInformativeText("Хотите установить Ansible сейчас?");
    
    QPushButton *installButton = msgBox.addButton("Установить Ansible", QMessageBox::AcceptRole);
    QPushButton *laterButton = msgBox.addButton("Позже", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == installButton) {
        installAnsibleInWSL();
    } else {
        // Пользователь отказался, флаг уже установлен, больше не предлагаем
        qDebug() << "User declined Ansible installation";
    }
}

// Установка Ansible в WSL
void WSLChecker::installAnsibleInWSL()
{
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle("Установка Ansible в WSL");
    msgBox.setText("Ansible будет установлен автоматически.\n\n"
                   "Пароль sudo: 1111");
    
    QPushButton *installButton = msgBox.addButton("Установить", QMessageBox::AcceptRole);
    QPushButton *closeButton = msgBox.addButton("Закрыть", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == installButton) {
        // Используем существующий m_ansibleInstallProcess
        if (!m_installDialog) {
            m_installDialog = new QDialog(parent);
            m_installDialog->setWindowTitle("Установка Ansible");
            m_installDialog->setMinimumWidth(600);
            m_installDialog->setMinimumHeight(400);
            
            QVBoxLayout *layout = new QVBoxLayout(m_installDialog);
            
            m_installOutput = new QTextEdit(m_installDialog);
            m_installOutput->setReadOnly(true);
            m_installOutput->setFontFamily("Courier New");
            layout->addWidget(m_installOutput);
            
            m_installProgressBar = new QProgressBar(m_installDialog);
            m_installProgressBar->setRange(0, 5);
            m_installProgressBar->setValue(0);
            layout->addWidget(m_installProgressBar);
            
            m_installCloseButton = new QPushButton("Закрыть", m_installDialog);
            m_installCloseButton->setEnabled(false);
            layout->addWidget(m_installCloseButton);
            
            connect(m_installCloseButton, &QPushButton::clicked, m_installDialog, &QDialog::accept);
        }
        
        m_installOutput->clear();
        m_installProgressBar->setValue(0);
        m_installCloseButton->setEnabled(false);
        m_installDialog->show();
        
        // Создаем скрипт для последовательной установки
        QString script = 
            "echo '=== 1. Обновление списка пакетов ==='\n"
            "echo '1111' | sudo -S apt update\n"
            "echo '=== 2. Установка дополнительных компонентов ==='\n"
            "echo '1111' | sudo -S apt install -y software-properties-common\n"
            "echo '=== 3. Добавление репозитория Ansible ==='\n"
            "echo '1111' | sudo -S add-apt-repository --yes --update ppa:ansible/ansible\n"
            "echo '=== 4. Установка Ansible ==='\n"
            "echo '1111' | sudo -S apt install -y ansible\n"
            "echo '=== 5. Проверка версии Ansible ==='\n"
            "ansible --version\n"
            "echo '=== Установка завершена ==='\n";
        
        // Настраиваем процесс на скрытый запуск
#ifdef _WIN32
        m_ansibleInstallProcess->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args) {
            args->flags |= CREATE_NO_WINDOW;
            args->startupInfo->dwFlags |= STARTF_USESHOWWINDOW;
            args->startupInfo->wShowWindow = SW_HIDE;
        });
#endif
        
        qDebug() << "Starting Ansible installation with password 1111";
        m_ansibleInstallProcess->start("wsl", QStringList() << "bash" << "-c" << script);
    }
}

// Слот завершения установки Ansible
void WSLChecker::onAnsibleInstallFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
        
    if (m_installDialog) {
        if (exitCode == 0) {
            m_installOutput->append("\n✅ Ansible успешно установлен!");
            m_installProgressBar->setValue(5);
            
            QMessageBox::information(m_installDialog, "Успех", 
                "Ansible был успешно установлен в WSL!");
            
            // Обновляем информацию об Ansible
            m_lastInfo.ansibleInstalled = true;
            checkAnsibleVersionAsync();
            
        } else {
            QString error = m_ansibleInstallProcess->readAllStandardError();
            m_installOutput->append("\n❌ Установка не удалась с ошибкой:");
            m_installOutput->append(error);
            
            QMessageBox::warning(m_installDialog, "Ошибка установки", 
                "Не удалось установить Ansible в WSL.\n\n" + error);
        }
        
        m_installCloseButton->setEnabled(true);
    }
    
    emit ansibleInstallFinished(exitCode == 0);
}
// Проверка наличия WSL и дистрибутивов
WSLChecker::WSLInfo WSLChecker::checkWSL()
{
    WSLInfo info;
    
    // Проверяем наличие wsl.exe в системном пути
    QProcess whichProcess;
    whichProcess.start("where", QStringList() << "wsl.exe");
    
    if (whichProcess.waitForFinished(2000)) {
        QString whichOutput = whichProcess.readAllStandardOutput().trimmed();
        if (!whichOutput.isEmpty()) {
            info.isInstalled = true;
            
            // Получаем список дистрибутивов через wsl -l -v
            QProcess listProcess;
            listProcess.start("wsl", QStringList() << "-l" << "-v");
            
            if (listProcess.waitForFinished(5000)) {
                QByteArray rawOutput = listProcess.readAllStandardOutput();
                
                // Конвертируем UTF-16LE в QString
                QString output;
                if (rawOutput.size() > 1) {
                    // Пробуем декодировать как UTF-16LE (Windows WSL вывод)
                    output = QString::fromUtf16(reinterpret_cast<const ushort*>(rawOutput.constData()), rawOutput.size() / 2);
                } else {
                    // Fallback на local8Bit
                    output = QString::fromLocal8Bit(rawOutput);
                }
                
                qDebug() << "WSL list output (decoded):" << output;
                
                // Проверяем наличие дистрибутивов в выводе
                // Ожидаемый формат:
                //   NAME      STATE           VERSION
                // * Ubuntu    Running         2
                if (output.contains("Ubuntu", Qt::CaseInsensitive) || 
                    output.contains("Debian", Qt::CaseInsensitive) ||
                    output.contains("Running", Qt::CaseInsensitive) ||
                    output.contains("Stopped", Qt::CaseInsensitive)) {
                    
                    info.hasDistributions = true;
                    
                    // Извлекаем имя дистрибутива
                    QStringList lines = output.split('\n');
                    for (const QString& line : lines) {
                        // Ищем строку с именем дистрибутива
                        if (line.contains("Ubuntu", Qt::CaseInsensitive)) {
                            info.distributions.append("Ubuntu");
                            info.defaultDistribution = "Ubuntu";
                            break;
                        } else if (line.contains("Debian", Qt::CaseInsensitive)) {
                            info.distributions.append("Debian");
                            info.defaultDistribution = "Debian";
                            break;
                        }
                    }
                    
                    // Если не нашли конкретное имя, но дистрибутив есть
                    if (info.distributions.isEmpty() && 
                        (output.contains("Running") || output.contains("Stopped"))) {
                        info.distributions.append("Linux");
                        info.defaultDistribution = "Linux";
                    }
                    
                    // Только если есть дистрибутивы, проверяем Ansible
                    QTimer::singleShot(100, this, &WSLChecker::checkAnsibleVersionAsync);
                } else {
                    info.hasDistributions = false;
                    info.defaultDistribution = "Ubuntu";
                    info.ansibleInstalled = false;
                    info.ansibleVersion = QString();
                    qDebug() << "No distributions found in output";
                }
            } else {
                qDebug() << "Failed to get distribution list";
                info.hasDistributions = false;
                info.defaultDistribution = "Ubuntu";
                info.ansibleInstalled = false;
                info.ansibleVersion = QString();
            }
            
            info.errorMessage = "";
        } else {
            info.isInstalled = false;
            info.hasDistributions = false;
            info.ansibleInstalled = false;
            info.ansibleVersion = QString();
            info.defaultDistribution = "";
        }
    } else {
        info.isInstalled = false;
        info.hasDistributions = false;
        info.ansibleInstalled = false;
        info.ansibleVersion = QString();
        info.defaultDistribution = "";
        info.errorMessage = "WSL не установлен";
    }
    
    m_lastInfo = info;
    
    qDebug() << "  isInstalled:" << info.isInstalled;
    qDebug() << "  hasDistributions:" << info.hasDistributions;
    qDebug() << "  distributions:" << info.distributions;
    qDebug() << "  defaultDistribution:" << info.defaultDistribution;
    
    return info;
}

// Асинхронная проверка Ansible
void WSLChecker::checkAnsibleVersionAsync()
{
    
    // Дополнительная проверка с обновлением m_lastInfo
    if (!m_lastInfo.isInstalled || !m_lastInfo.hasDistributions) {
        // // qDebug() << "  isInstalled:" << m_lastInfo.isInstalled;
        // // qDebug() << "  hasDistributions:" << m_lastInfo.hasDistributions;
        // // qDebug() << "  distributions:" << m_lastInfo.distributions;
        
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
                
                if (output == "WSL_TEST_OK") {
                    // WSL работает, теперь проверим Ansible
                    checkAnsiblePresence();
                } else {
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
    QProcess *checkProcess = new QProcess(this);
    connect(checkProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, checkProcess](int, QProcess::ExitStatus) {
                QString output = checkProcess->readAllStandardOutput().trimmed();
                QString error = checkProcess->readAllStandardError().trimmed();
                
                if (!error.isEmpty()) {
                }
                
                if (output == "INSTALLED") {
                    getAnsibleVersionAsync();
                } else {
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
    
    QStringList args;
    args << "bash" << "-c" << "ansible --version 2>&1 | head -n 1 || echo 'VERSION_ERROR'";
    
    m_versionCheckProcess->start("wsl", args);
}

// Принудительное обновление информации об Ansible
void WSLChecker::refreshAnsibleInfo()
{
    
    if (m_lastInfo.isInstalled && m_lastInfo.hasDistributions) {
        checkAnsibleVersionAsync();
    } else {
        m_lastInfo.ansibleInstalled = false;
        m_lastInfo.ansibleVersion = QString();
        emit ansibleInfoUpdated(false, QString());
    }
}

// Синхронная проверка наличия Ansible
bool WSLChecker::isAnsibleInstalled()
{
    
    if (!m_lastInfo.isInstalled || !m_lastInfo.hasDistributions) {
        return false;
    }
    
    QProcess checkProcess;
    QStringList args;
    args << "bash" << "-c" << "command -v ansible >/dev/null 2>&1 && echo 'INSTALLED' || echo 'NOT_INSTALLED'";
    
    checkProcess.start("wsl", args);
    
    if (!checkProcess.waitForFinished(5000)) {
        return false;
    }
    
    QString output = checkProcess.readAllStandardOutput().trimmed();
    bool installed = (output == "INSTALLED");
    
    return installed;
}

// Синхронное получение версии Ansible
QString WSLChecker::getAnsibleVersion()
{
    
    if (!m_lastInfo.isInstalled || !m_lastInfo.hasDistributions) {
        return QString();
    }
    
    QProcess versionProcess;
    QStringList args;
    args << "bash" << "-c" << "ansible --version 2>/dev/null | head -n 1";
    
    versionProcess.start("wsl", args);
    
    if (!versionProcess.waitForFinished(5000)) {
        return QString();
    }
    
    QString version = versionProcess.readAllStandardOutput().trimmed();
    
    if (!version.isEmpty()) {
        m_lastInfo.ansibleInstalled = true;
        m_lastInfo.ansibleVersion = version;
    } else {
        m_lastInfo.ansibleInstalled = false;
        m_lastInfo.ansibleVersion = QString();
    }
    
    return version;
}

// Показать диалог настройки WSL
void WSLChecker::showWslSetupDialog()
{
    // Обновляем информацию
    m_lastInfo = checkWSL();
    
    qDebug() << "showWslSetupDialog called";
    qDebug() << "  isInstalled:" << m_lastInfo.isInstalled;
    qDebug() << "  hasDistributions:" << m_lastInfo.hasDistributions;
    qDebug() << "  ansibleInstalled:" << m_lastInfo.ansibleInstalled;
    
    if (!m_lastInfo.isInstalled) {
        // Случай 1: WSL не установлен
        showWslInstallDialog();
    } else if (!m_lastInfo.hasDistributions) {
        // Случай 2: WSL установлен, но нет дистрибутивов
        showDistroInstallDialog();
    } else if (!m_lastInfo.ansibleInstalled && !m_offerShown) {
        // Случай 3: WSL и дистрибутив есть, но Ansible не установлен и предложение не показано
        offerAnsibleInstallation();
    } else {
        // Всё установлено или предложение уже показано
        qDebug() << "WSL, distro and Ansible are all installed or offer already shown";
    }
}

// Диалог установки дистрибутива Ubuntu
void WSLChecker::showDistroInstallDialog()
{
    
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
        installUbuntu();
    } else {
    }
}

// Установка дистрибутива Ubuntu
void WSLChecker::installUbuntu()
{
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle("Установка Ubuntu");
    msgBox.setText("Установка Ubuntu будет запущена.\n\n"
                   "После установки в отдельном окне введите:\n"
                   "• Имя пользователя: ubuntu\n"
                   "• Пароль: 1111\n"
                   "• Подтверждение пароля: 1111\n\n"
                   "Установка будет автоматически обнаружена.");
    
    QPushButton *installButton = msgBox.addButton("Установить", QMessageBox::AcceptRole);
    QPushButton *cancelButton = msgBox.addButton("Отмена", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == installButton) {
        // Запускаем установку
        m_distroProcess->start("wsl", QStringList() << "--install" << "-d" << "Ubuntu");
        
        // Показываем диалог ожидания
        QDialog *waitDialog = new QDialog(parent);
        waitDialog->setWindowTitle("Установка Ubuntu");
        waitDialog->setMinimumWidth(400);
        
        QVBoxLayout *layout = new QVBoxLayout(waitDialog);
        
        QLabel *label = new QLabel("Установка Ubuntu выполняется...\n\n"
                                   "Пожалуйста, дождитесь завершения установки\n"
                                   "и введите учетные данные в открывшемся окне.\n\n"
                                   "После завершения установка Ansible будет предложена автоматически.");
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        
        QProgressBar *progressBar = new QProgressBar(waitDialog);
        progressBar->setRange(0, 0);
        layout->addWidget(progressBar);
        
        QPushButton *hideButton = new QPushButton("Скрыть", waitDialog);
        layout->addWidget(hideButton);
        
        connect(hideButton, &QPushButton::clicked, waitDialog, &QDialog::accept);
        
        waitDialog->show();
        
        // Запускаем периодическую проверку установки
        m_checkAttempts = 0;
        m_checkTimer->start();
        
        // Таймер для закрытия диалога после обнаружения установки
        connect(m_checkTimer, &QTimer::timeout, this, [waitDialog]() {
            if (waitDialog && waitDialog->isVisible()) {
                // Не закрываем сразу, даем время на ввод пароля
            }
        });
    }
}

// Слот завершения установки дистрибутива
void WSLChecker::onDistroInstallFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    
    if (m_distroDialog) {
        if (exitCode == 0) {
            m_distroOutput->append("\n✅ Ubuntu успешно установлен!");
            m_distroOutput->append("\n⏳ Выполняется настройка...");
            m_checkAttempts = 0;
            m_checkTimer->start();
            // Ждем инициализации дистрибутива
            QTimer::singleShot(3000, this, [this]() {
                // Обновляем статус WSL
                m_lastInfo.hasDistributions = true;
                m_lastInfo.distributions.append("Ubuntu");
                m_lastInfo.defaultDistribution = "Ubuntu";
                
                m_distroOutput->append("\n✅ Дистрибутив готов к использованию!");
            });
            
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
        installWsl();
    } else {
    }
}

// Установка WSL
void WSLChecker::installWsl()
{
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox::information(parent, "Установка WSL", 
        "Запуск установки WSL. Это может занять несколько минут.\n"
        "После установки может потребоваться перезагрузка компьютера.\n");
    
    QStringList args;
    args << "/c" << "wsl" << "--install";
    
    m_installProcess->start("cmd.exe", args);
}

// Слот завершения установки WSL
void WSLChecker::onInstallProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    if (exitCode == 0) {
        QMessageBox::information(parent, "Установка WSL завершена", 
            "WSL успешно установлен! Теперь необходимо установить дистрибутив Linux.");
        
        // После установки WSL предлагаем установить дистрибутив
        QTimer::singleShot(500, this, &WSLChecker::showDistroInstallDialog);
        
        emit wslSetupFinished(true);
    } else {
        QString error = m_installProcess->readAllStandardError();
        QMessageBox::warning(parent, "Ошибка установки WSL", 
            "Не удалось установить WSL.\n\n" + error);
        emit wslSetupFinished(false);
    }
}

void WSLChecker::refreshAfterDistroInstall()
{
    // // qDebug() << "Refreshing after distro install";
    
    // Повторно проверяем WSL
    checkWSL();
    
    // Если есть дистрибутивы, проверяем Ansible
    if (m_lastInfo.hasDistributions) {
        QTimer::singleShot(500, this, &WSLChecker::checkAnsibleVersionAsync);
        
        // Через секунду предлагаем установить Ansible, если не установлен
        QTimer::singleShot(2000, this, [this]() {
            if (!m_lastInfo.ansibleInstalled) {
                offerAnsibleInstallation();
            }
        });
    }
}

void WSLChecker::periodicCheck()
{
    m_checkAttempts++;
    
    qDebug() << "Periodic check attempt" << m_checkAttempts;
    
    WSLInfo info = checkWSL();
    
    if (info.hasDistributions) {
        // Дистрибутив установлен, останавливаем проверку
        m_checkTimer->stop();
        
        // Обновляем информацию
        m_lastInfo = info;
        
        // Предлагаем установить Ansible только если он не установлен и предложение не было показано
        if (!info.ansibleInstalled && !m_offerShown) {
            QTimer::singleShot(500, this, &WSLChecker::offerAnsibleInstallation);
        }
    } else if (m_checkAttempts > 30) {
        // После 30 попыток (90 секунд) останавливаем проверку
        m_checkTimer->stop();
        qDebug() << "Stop checking - timeout";
    }
}