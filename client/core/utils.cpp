#include "utils.h"
#include "logger.h"

namespace {
Logger logger("Utils");
}

Utils::Utils() {}

QString Utils::getOpenSSLPath()
{
#if defined(Q_OS_WIN)
    return QString(QCoreApplication::applicationDirPath() + "//openssl/openssl.exe");
#endif
}

QByteArray Utils::getSecureKey()
{
    QString opensslPath = getOpenSSLPath();
    QStringList args;
    args << "rand" << "-hex" << "32";
    QProcess process;

    process.setProgram(opensslPath);
    process.setArguments(args);
    process.start();

    bool ok = process.waitForFinished(3000);
    if (!ok) {
        logger.error() << "QProcess failed with error:" << process.errorString();
        return QByteArray();
    }

    QByteArray output = process.readAllStandardOutput();

    // TODO: remove this debug statement
    logger.debug() << "Generated secure key:" << output;

    return output;
}

QString Utils::getAppPath()
{
    return QCoreApplication::applicationDirPath();
}

QString Utils::getPrimaryAppPath()
{
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();

    return dir.absolutePath();
}
