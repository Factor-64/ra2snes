#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIconEngine>
#include "ra2snes.h"
#include "achievementsortfilterproxymodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<AchievementSortFilterProxyModel>("CustomModels", 1, 0, "AchievementSortFilterProxyModel");

    QQmlApplicationEngine engine;
    ra2snes ra2snesInstance;

    engine.rootContext()->setContextProperty("ra2snes", &ra2snesInstance);
    engine.rootContext()->setContextProperty("achievementModel", ra2snesInstance.achievementModel());
    engine.rootContext()->setContextProperty("gameInfoModel", ra2snesInstance.gameInfoModel());
    engine.rootContext()->setContextProperty("userInfoModel", ra2snesInstance.userInfoModel());
    engine.load(QUrl(QStringLiteral("qrc:/ui/login.qml")));

    QObject::connect(&ra2snesInstance, &ra2snes::loginSuccess, &engine, [&engine]() {
        const auto rootObjects = engine.rootObjects();
        QObject *loginWindow = nullptr;

        if (!rootObjects.isEmpty()) {
            loginWindow = rootObjects.first();
        }
        if(loginWindow)
            loginWindow->deleteLater();
        engine.load(QUrl(QStringLiteral("qrc:/ui/mainwindow.qml")));
        engine.clearComponentCache();
    });

    QObject::connect(&ra2snesInstance, &ra2snes::signedOut, &engine, [&engine]() {
        const auto rootObjects = engine.rootObjects();
        QObject *mainWindow = nullptr;

        if (!rootObjects.isEmpty()) {
            mainWindow = rootObjects.first();
        }
        if(mainWindow)
            mainWindow->deleteLater();
        engine.load(QUrl(QStringLiteral("qrc:/ui/login.qml")));
        engine.clearComponentCache();
    });

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
