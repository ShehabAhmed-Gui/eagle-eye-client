/* Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "logger.h"
#include <iostream>
#include <QRegularExpression>

QFile Logger::m_logFile;
QTextStream Logger::m_textStream;

Logger::Logger(QString className)
{
    m_className = className;
}

void Logger::logsMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Log type as an upperCase
    QString stringType;
    switch (type) {
    case QtDebugMsg:
        stringType = "[DEBUG]";
        break;
    case QtWarningMsg:
        stringType = "[WARNING]";
        break;
    case QtCriticalMsg:
        stringType = "[CRITICAL]";
        break;
    case QtFatalMsg:
        stringType = "[FATAL]";
        break;
    default:
        stringType = "[INFO]";
        break;
    }

    QString formattedMessage = qFormatLogMessage(type, context, msg);

    // Replace lowercase type with uppercase type
    QRegularExpression re(R"((\d{2}-\d{2} \d{2}:\d{2}:\d{2} )(\w+))");
    formattedMessage.replace(re, QString(R"(\1%1)").arg(stringType));

    Logger::m_textStream << formattedMessage << Qt::endl << Qt::flush;

    // Log message to console
    std::cout << msg.toStdString() << std::endl << std::flush;
}

QString Logger::systemLogsDir()
{
    QStringList locationList = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    const QString primaryLocation = "ProgramData";

    for (QString &location : locationList) {
        if (location.contains(primaryLocation)) {
            QString systemLocation = QString("%1/%2/log").arg(location, "AppGuard");
            return systemLocation;
        }
    }

    return QString();
}

void Logger::init()
{
    QDir logsDir(systemLogsDir());
    if (!logsDir.mkpath(systemLogsDir())) {
        qCritical() << "Could not create logs dir";
        return;
    }

    const QString serviceLogsName = "AppGuard.log";
    m_logFile.setFileName(logsDir.filePath(serviceLogsName));

    if (!m_logFile.open(QIODevice::Append)) {
        qCritical() << "Could not open logs file. Error:" << m_logFile.errorString();
        return;
    }

    QString messagePattern = QString("%{time yyyy-MM-dd hh:mm:ss} %{type} %{message}");
    qSetMessagePattern(messagePattern);
    qInstallMessageHandler(logsMessageHandler);

    m_logFile.setTextModeEnabled(true);
    m_textStream.setDevice(&m_logFile);
}

void Logger::deinit()
{
    qInstallMessageHandler(nullptr);
    qSetMessagePattern("${message}");
    m_textStream.setDevice(nullptr);
}

Logger::Log::Log(Logger *logger, LogType logType)
    : m_data(new Data())
    , m_logLevel(logType)
    , m_logger(logger)
{
}

Logger::Log::~Log()
{
    // Write buffer to log file
    switch (m_logLevel) {
    case Debug:
        qDebug() << m_logger->m_className << m_data->m_buffer.trimmed();
        break;
    case Info:
        qInfo() << m_logger->m_className << m_data->m_buffer.trimmed();
        break;
    case Warning:
        qWarning() << m_logger->m_className << m_data->m_buffer.trimmed();
        break;
    case Critical:
        qCritical() << m_logger->m_className << m_data->m_buffer.trimmed();
    default:
        qCritical() << m_logger->m_className << m_data->m_buffer.trimmed();;
        break;
    }

    delete m_data;
}

#define DEFINE_OP(T)                                    \
Logger::Log &Logger::Log::operator<<(T t)               \
{                                                       \
    m_data->m_ts << t << " ";                           \
    return *this;                                       \
}

DEFINE_OP(uint64_t);
DEFINE_OP(const char*);
DEFINE_OP(const QString &);
DEFINE_OP(const QByteArray &);
DEFINE_OP(const void *);

Logger::Log &Logger::Log::operator<<(const QStringList &list)
{
    m_data->m_ts << list.join(",") << " ";
    return *this;
}

Logger::Log& Logger::Log::operator<<(const QVariant& t)
{
    m_data->m_ts << t.toString() << " ";
    return *this;
}

Logger::Log& Logger::Log::operator<<(const QJsonObject &t)
{
    m_data->m_ts << QJsonDocument(t).toJson(QJsonDocument::Indented) << " ";
    return *this;
}

Logger::Log Logger::info()
{
    return Log(this, LogType::Info);
}

Logger::Log Logger::debug()
{
    return Log(this, LogType::Debug);
}

Logger::Log Logger::warning()
{
    return Log(this, LogType::Warning);
}

Logger::Log Logger::critical()
{
    return Log(this, LogType::Critical);
}

Logger::Log Logger::error()
{
    return Log(this, LogType::Error);
}
