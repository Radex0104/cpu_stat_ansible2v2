#ifndef ANSIBLERUNNER_H
#define ANSIBLERUNNER_H

#include <QObject>
#include <QProcess>
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
    void executePlaybook();
    bool convertScriptToUnixFormat(const QString& filePath, QString& convertedPath);
    bool updateScriptPathInPlaybook(const QString& playbookPath, const QString& scriptPath);
    void stop();
    
    // Новый метод для установки менеджера прогресса
    void setProgressManager(ProgressManager* manager);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onProcessErrorOccurred(QProcess::ProcessError error);
    void readProcessOutput();

private:
    void createInventoryFile();
    QString convertToWslPath(const QString& windowsPath) const;
    void parseProgressFromOutput(const QString& output);

    QProcess* ansibleProcess;
    QString playbookPath;
    QString scriptPath;
    QString inventoryPath;
    QList<HostConfig> hostsConfig;
    
    // Новый член класса для управления прогрессом
    ProgressManager* m_progressManager;
    
    // Для отслеживания этапов выполнения
    int m_currentTaskIndex;
    QStringList m_taskNames;

signals:
    void outputReceived(const QString& text);
    void errorOccurred(const QString& error);
    void finished(bool success, int exitCode);
    
    // Новые сигналы для прогресса
    void progressUpdated(int percent, const QString& taskName);
    void taskStarted(const QString& taskName);
    void taskCompleted(const QString& taskName);
};

#endif // ANSIBLERUNNER_H