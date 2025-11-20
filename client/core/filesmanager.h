#ifndef FILESMANAGER_H
#define FILESMANAGER_H

#include <QObject>
#include <QFile>
#include <QFileInfoList>
#include <QDir>
#include <QDirIterator>

class FilesManager : public QObject
{
    Q_OBJECT
public:
    explicit FilesManager(QObject *parent = nullptr);

    // Returns the absolute paths of program files
    // in a directory
    static QVector<QString> getProgramFiles(const QString &directoryPath);

    // Returns the absolute paths of program files
    // in a tree
    static QVector<QString> getProgramFilesInTree(const QString &tree_path);

    // Returns the absolute paths of executable files
    // in a directory
    static QVector<QString> getExeFiles(const QString &directoryPath);
};

#endif // FILESMANAGER_H
