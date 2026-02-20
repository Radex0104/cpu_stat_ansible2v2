#ifndef WSLCHECKER_H
#define WSLCHECKER_H

#include <QObject>
#include <QProcess>
#include <QDebug>
#include <QTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QDialog>
#include <QTimer>

class WSLChecker : public QObject
{
    Q_OBJECT
public:
    struct WSLInfo {
        bool isInstalled = false;
        bool hasDistributions = false;
        bool ansibleInstalled = false;
        QString ansibleVersion;
        QStringList distributions;
        QString defaultDistribution;
        QString errorMessage;
    };
    
    explicit WSLChecker(QObject *parent = nullptr);
    ~WSLChecker();
    
    WSLInfo checkWSL();
    void showWslSetupDialog();
    
    // Проверка Ansible (запускается автоматически после checkWSL если есть дистрибутивы)
    void checkAnsibleVersionAsync();  // Асинхронная проверка
    
    // Синхронные методы (если нужны)
    bool isAnsibleInstalled();        // Синхронная проверка наличия
    QString getAnsibleVersion();      // Синхронное получение версии
    
    // Getters
    bool isWslInstalled() const { return m_lastInfo.isInstalled; }
    bool hasDistributions() const { return m_lastInfo.hasDistributions; }
    bool isAnsibleInstalled() const { return m_lastInfo.ansibleInstalled; }
    QString getAnsibleVersion() const { return m_lastInfo.ansibleVersion; }
    QString getDefaultDistribution() const { return m_lastInfo.defaultDistribution; }
    QStringList getDistributions() const { return m_lastInfo.distributions; }
    
signals:
    void wslSetupFinished(bool success);
    void distroInstallFinished(bool success);
    void ansibleInstallFinished(bool success);
    void ansibleOutputReceived(const QString &output);
    void ansibleInfoUpdated(bool installed, const QString &version);  // Новый сигнал
    
private slots:
    void onInstallProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onDistroInstallFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onDistroOutput();
    void onDistroError();
    void onAnsibleInstallFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onAnsibleOutput();
    void onAnsibleError();
    void onVersionCheckFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onAnsibleInstallOutput();
    void onAnsibleInstallError();
    
    
private:
    void showWslInstallDialog();
    void installWsl();
    void showDistroInstallDialog();
    void installUbuntu();
    void showAnsibleInstallDialog();
    void installAnsible();
    void getAnsibleVersionAsync();  // Внутренний метод для получения версии после проверки наличия
    void refreshAnsibleInfo();
    void checkAnsiblePresence();
    void offerAnsibleInstallation();
    void installAnsibleInWSL();

    QProcess *m_installProcess;
    QProcess *m_distroProcess;
    QProcess *m_ansibleProcess;
    QProcess *m_versionCheckProcess;
    WSLInfo m_lastInfo;
    QProcess *m_ansibleInstallProcess;
    
    // UI elements for distribution installation progress
    QDialog *m_distroDialog = nullptr;
    QTextEdit *m_distroOutput = nullptr;
    QPushButton *m_distroCloseButton = nullptr;
    QProgressBar *m_distroProgressBar = nullptr;
    
    // UI elements for Ansible installation progress
    QDialog *m_ansibleDialog = nullptr;
    QTextEdit *m_ansibleOutput = nullptr;
    QPushButton *m_ansibleCloseButton = nullptr;
    QProgressBar *m_ansibleProgressBar = nullptr;

    QDialog *m_installDialog = nullptr;
    QTextEdit *m_installOutput = nullptr;
    QPushButton *m_installCloseButton = nullptr;
    QProgressBar *m_installProgressBar = nullptr;
};

#endif // WSLCHECKER_H