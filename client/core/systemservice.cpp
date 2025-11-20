#include "systemservice.h"

SystemService::SystemService(int argc, char **argv)
    : QtService<QCoreApplication>(argc, argv, "AppGuardService")
{
    setServiceDescription("AppGuard service");
}

void SystemService::start()
{
}

void SystemService::stop()
{
    qDebug() << "Service stopped";
}
