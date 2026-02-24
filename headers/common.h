#ifndef COMMON_H
#define COMMON_H

#include <QString>

// Единое определение структуры HostConfig
struct HostConfig {
    QString address;
    QString sshUser;
    QString sshPass;
};

#endif // COMMON_H