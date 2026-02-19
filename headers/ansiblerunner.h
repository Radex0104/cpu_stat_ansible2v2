#ifndef ANSIBLERUNNER_H
#define ANSIBLERUNNER_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QList>
#include "configmanager.h"

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
    QString convertToWslPath(const QString& windowsPath) const;
    bool updateScriptPathInPlaybook(const QString& playbookPath, const QString& scriptPath);
    void stop();

signals:
    void outputReceived(const QString& text);
    void finished(bool success, int exitCode);
    void errorOccurred(const QString& message);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onProcessErrorOccurred(QProcess::ProcessError error);
    void readProcessOutput();

private:
    void createInventoryFile();

    QProcess *ansibleProcess;
    QString playbookPath;
    QString scriptPath;
    QString inventoryPath;
    QList<HostConfig> hostsConfig;
};

#endif // ANSIBLERUNNER_H
