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

#ifndef LOGGER_H
#define LOGGER_H

#include <QMessageLogger>
#include <QTextStreamFunction>
#include <QIODevice>
#include <QStandardPaths>
#include <QVariant>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

class Logger : public QObject
{
public:
    explicit Logger(QString className);

    static void init();
    static void deinit();
    static void logsMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    enum LogType {
        Info,
        Debug,
        Warning,
        Critical,
        Error
    };

class Log
{
    public:
        Log(Logger *logger, LogType logType);
        ~Log();

        Log &operator<<(uint64_t t);
        Log &operator<<(const char *t);
        Log &operator<<(const QString &t);
        Log &operator<<(const QStringList &t);
        Log &operator<<(const QByteArray &t);
        Log &operator<<(const QJsonObject &t);
        Log &operator<<(const QVariant& t);
        Log &operator<<(const void *t);
        template<typename T>
        Log &operator<<(const T &t) {
            m_data->m_ts << t << " ";
            return *this;
        };

    private:
        Logger *m_logger;
        LogType m_logLevel;

        struct Data
        {
            Data() : m_ts(&m_buffer, QIODevice::WriteOnly)
            {
            }

            QString m_buffer;
            QTextStream m_ts;
        };

        Data *m_data;
};

public:
    Log info();
    Log debug();
    Log warning();
    Log critical();
    Log error();

private:
    QString m_className;

    static QFile m_logFile;
    static QString systemLogsDir();
    static QTextStream m_textStream;
};

#endif // LOGGER_H
