#ifndef ANSIBLERUNNER_H
#define ANSIBLERUNNER_H

#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QStringList>
#include "progressmanager.h"
#include "common.h"

class AnsibleRunner : public QObject
{
    Q_OBJECT
public:
    explicit AnsibleRunner(QObject *parent = nullptr);
    ~AnsibleRunner();

    void setPlaybookPath(const QString& path);
    void setScriptPath(const QString& path);
    void setHosts(const QList<HostConfig>& hosts);
    void setProgressManager(ProgressManager* manager);
    
    // JMeter настройки
    void setJmeterHost(const QString& host) { 
        jmeterHost = host; 
        qDebug() << "AnsibleRunner::setJmeterHost:" << host;
    }
    void setJmeterArchiveSrc(const QString& src) { 
        jmeterArchiveSrc = src; 
        qDebug() << "AnsibleRunner::setJmeterArchiveSrc:" << src;
    }
    void setJmeterRemoteTestDir(const QString& dir) { 
        jmeterRemoteTestDir = dir; 
        qDebug() << "AnsibleRunner::setJmeterRemoteTestDir:" << dir;
    }
    void setJmeterResultsRemoteDir(const QString& dir) { 
        jmeterResultsRemoteDir = dir; 
        qDebug() << "AnsibleRunner::setJmeterResultsRemoteDir:" << dir;
    }
    void setJmeterResultsLocalDir(const QString& dir) { 
        jmeterResultsLocalDir = dir; 
        qDebug() << "AnsibleRunner::setJmeterResultsLocalDir:" << dir;
    }
    void setJmeterTestDuration(int duration) { 
        jmeterTestDuration = duration; 
        qDebug() << "AnsibleRunner::setJmeterTestDuration:" << duration;
    }

    
    void executePlaybook();
    void stop();
    
    bool updateArchivePathInPlaybook(const QString& playbookPath, const QString& archivePath);
    bool filesFinder(const QString& filePath, QString* archivePath = nullptr);

signals:
    void outputReceived(const QString& output);
    void errorOccurred(const QString& error);
    void finished(bool success, int exitCode);
    void taskStarted(const QString& taskName);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onProcessErrorOccurred(QProcess::ProcessError error);
    void readProcessOutput();

private:
    void createInventoryFile();
    void parseProgressFromOutput(const QString& output);
    QString convertToWslPath(const QString& windowsPath) const;

private:
    QProcess* ansibleProcess;
    QString playbookPath;
    QString scriptPath;
    QString inventoryPath;
    QList<HostConfig> hostsConfig;
    ProgressManager* m_progressManager;
    
    // Индексация задач
    int m_currentTaskIndex;
    QStringList m_taskNames;
    
    // JMeter переменные
    QString jmeterHost;
    QString jmeterArchiveSrc;
    QString jmeterRemoteTestDir;
    QString jmeterResultsRemoteDir;
    QString jmeterResultsLocalDir;
    int jmeterTestDuration = 300;
    bool jmeterGenerateReport;
};

#endif // ANSIBLERUNNER_H