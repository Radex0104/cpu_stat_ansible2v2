#ifndef WSLCHECKER_H
#define WSLCHECKER_H

#include <QObject>
#include <QProcess>
#include <QDebug>

class WSLChecker : public QObject
{
    Q_OBJECT

public:
    explicit WSLChecker(QObject *parent = nullptr);
    ~WSLChecker();

    struct WSLInfo {
        bool isInstalled = false;
        bool hasDistributions = false;
        QString errorMessage;
        QStringList distributions;
        QString wslVersion;
        QString defaultDistribution;
        bool virtualizationEnabled = true;
    };

    // Синхронная проверка WSL (возвращает результат сразу)
    WSLInfo checkWSL();
    
    // Методы для показа диалогов
    void showWslSetupDialog();

signals:
    void wslCheckCompleted(const WSLChecker::WSLInfo &info);
    void wslSetupFinished(bool success);

private slots:
    void onInstallProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess *m_installProcess; // Только для установки
    WSLInfo m_lastInfo;
    
    void parseWSLOutput(const QString &output, const QString &error, WSLInfo &info);
    void parseDistributions(const QString &output, WSLInfo &info);
    
    // Методы для диалогов
    void showWslInstallDialog();
    void showWslDistroDialog();
    void installWsl();
    void installWslDistro(const QString &distroName);
    void openWslInstallationGuide();
};

#endif // WSLCHECKER_H