#include <QGuiApplication>
#include <QObject>
#include <QScopedPointer>

#include "corecontroller.h"
#include "systemservice.h"
#include "logger.h"

int main(int argc, char *argv[])
{    
    SystemService service(argc, argv);

    QCoreApplication::setApplicationName("AppGuard");
    QCoreApplication::setApplicationVersion("1.0");

    QScopedPointer<CoreController> coreController;
    coreController.reset(new CoreController());

    QTimer::singleShot(0, [&]() {
        coreController->init();
    });

    qDebug().noquote() << QCoreApplication::applicationName()
             << "version"
             << QCoreApplication::applicationVersion()
             << "started";

    return service.exec();
}
