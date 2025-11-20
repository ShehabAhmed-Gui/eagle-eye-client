#include "filesmanager.h"
#include "logger.h"

#include <QStack>

namespace {
Logger logger("FilesManager");
}

FilesManager::FilesManager(QObject *parent)
    : QObject{parent}
{}

QVector<QString> FilesManager::getProgramFiles(const QString &directoryPath)
{
    const std::string programFileExtensions[] = { "exe", "dll" };
    const int programFileExtensionsCount = sizeof(programFileExtensions) / sizeof(std::string);

    QDir directory = QDir(directoryPath);
    directory.setFilter(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);

    QFileInfoList fileInfoList = directory.entryInfoList();
    QVector<QString> filePaths;

    for (QFileInfo &fileInfo : fileInfoList) {
        for (const std::string &extension : programFileExtensions) {
            if (fileInfo.suffix() == extension) {
                filePaths.append(fileInfo.absoluteFilePath());
                break;
            }
        }
    }

    return filePaths;
}

QVector<QString> FilesManager::getProgramFilesInTree(const QString &tree_path)
{
    QStack<QString> directoryStack;
    directoryStack.push(tree_path);

    QVector<QString> programFiles;
    while (directoryStack.isEmpty() == false)
    {
        QString directoryPath = directoryStack.pop();

        QVector<QString> directoryProgramFiles = getProgramFiles(directoryPath);
        programFiles.append(directoryProgramFiles);

        // Push all subdirectories to the stack
        QDir directory(directoryPath);
        directory.setFilter(QDir::AllDirs | QDir::Hidden | QDir::NoDotAndDotDot);
        QFileInfoList subDirectories = directory.entryInfoList();
        for (QFileInfo &subDirectory : subDirectories)
        {
            directoryStack.push(subDirectory.absoluteFilePath());
        }
    }

    return programFiles;
}

QVector<QString> FilesManager::getExeFiles(const QString &directoryPath)
{
    QDir directory = QDir(directoryPath);
    directory.setFilter(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);

    QFileInfoList fileInfoList = directory.entryInfoList();
    QVector<QString> filePaths;

    for (QFileInfo &fileInfo : fileInfoList) {
        if (fileInfo.suffix() == "exe") {
            filePaths.append(fileInfo.absoluteFilePath());
        }
    }

    return filePaths;
}
