#ifndef SYSTEMSERVICE_H
#define SYSTEMSERVICE_H

#include <QCoreApplication>
#include <QDebug>
#include <qtservice.h>

class SystemService : public QtService<QCoreApplication>
{
public:
    SystemService(int argc, char **argv);

public:
    void start() override;
    void stop() override;
};

#endif // SYSTEMSERVICE_H
