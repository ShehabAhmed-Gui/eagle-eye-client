#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QByteArray>
#include <QCoreApplication>
#include <QProcess>

// Static class for shared functions across all classes
class Utils
{
public:
    Utils();

    static QString getOpenSSLPath();
    static QByteArray getSecureKey();
    static QString getAppPath();
};

#endif // UTILS_H
