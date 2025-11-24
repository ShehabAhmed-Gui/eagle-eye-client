#include "utils.h"
#include "logger.h"

namespace {
Logger logger("Utils");
}

Utils::Utils() {}

QString Utils::getOpenSSLPath()
{
#if defined(Q_OS_WIN)
    return QCoreApplication::applicationDirPath() + "/openssl/openssl.exe";
#endif
}

QByteArray Utils::getSecureKey()
{
    QString opensslPath = getOpenSSLPath();
    logger .debug() << "using openssl path:" << opensslPath;
    QStringList args;
    args << "rand" << "-hex" << "32";
    QProcess process;

    process.setArguments(args);
    process.setProgram(opensslPath);
    bool ok = process.waitForFinished(3000);

    if (!ok) {
        logger.error() << "QProcess failed with error:" << process.errorString();
        return QByteArray();
    }

    QByteArray output = process.readAllStandardOutput();

    logger.debug() << "generated secure key:" << output;

    return output;
}

QString Utils::getAppPath()
{
    return QCoreApplication::applicationDirPath();
}
