#include "configmanager.h"
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
    configFilePath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/ansible_deployer.conf";
    QDir().mkpath(QFileInfo(configFilePath).path());
}

void ConfigManager::setConfigFilePath(const QString& path)
{
    configFilePath = path;
}

void ConfigManager::saveConfiguration(const QList<HostConfig>& hosts, const QString& defaultUser)
{
    QSettings settings(configFilePath, QSettings::IniFormat);

    QStringList hostsList;
    QStringList usersList;
    QStringList passwordsList;

    for (int i = 0; i < hosts.size(); ++i) {
        hostsList << hosts[i].address;
        usersList << hosts[i].sshUser;
        passwordsList << hosts[i].sshPass;
    }

    settings.setValue("hosts", hostsList);
    settings.setValue("ssh_users", usersList);
    settings.setValue("ssh_passwords", passwordsList);
    settings.setValue("default_ssh_user", defaultUser);

    settings.sync();
    qDebug() << "Конфигурация сохранена. Хостов:" << hosts.size();
}

void ConfigManager::loadConfiguration(QList<HostConfig>& hosts, QString& defaultUser)
{
    QSettings settings(configFilePath, QSettings::IniFormat);

    defaultUser = settings.value("default_ssh_user").toString();

    QStringList hostsList = settings.value("hosts").toStringList();
    QStringList usersList = settings.value("ssh_users").toStringList();
    QStringList passwordsList = settings.value("ssh_passwords").toStringList();

    hosts.clear();
    for (int i = 0; i < hostsList.size(); ++i) {
        HostConfig hostConfig;
        hostConfig.address = hostsList[i];

        if (i < usersList.size() && !usersList[i].isEmpty()) {
            hostConfig.sshUser = usersList[i];
        } else {
            hostConfig.sshUser = defaultUser;
        }

        if (i < passwordsList.size()) {
            hostConfig.sshPass = passwordsList[i];
        } else {
            hostConfig.sshPass = QString();
        }

        hosts.append(hostConfig);
    }

    qDebug() << "Конфигурация загружена. Хостов:" << hosts.size();
}