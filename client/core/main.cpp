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

#include <QGuiApplication>
#include <QObject>
#include <QScopedPointer>

#include <QTimer>

#include "corecontroller.h"
#include "systemservice.h"
#include "logger.h"

int main(int argc, char *argv[])
{
    SystemService service(argc, argv);

    Logger::init();

    QCoreApplication::setApplicationName("EagleEye");
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
