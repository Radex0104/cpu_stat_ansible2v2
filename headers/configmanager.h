#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QSettings>

struct HostConfig {
    QString address;
    QString sshUser;
    QString sshPass;  // Пароль для SSH подключения
};

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    explicit ConfigManager(QObject *parent = nullptr);

    void saveConfiguration(const QList<HostConfig>& hosts, const QString& defaultUser);
    void loadConfiguration(QList<HostConfig>& hosts, QString& defaultUser);
    void setConfigFilePath(const QString& path);

private:
    QString configFilePath;
};

#endif // CONFIGMANAGER_H
