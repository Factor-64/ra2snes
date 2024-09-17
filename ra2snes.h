#ifndef RA2SNES_H
#define RA2SNES_H

#include <QObject>
#include <QString>
#include "usb2snes.h"
#include "raclient.h"
#include "memoryreader.h"
#include "achievementmodel.h"

class ra2snes : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentGame READ currentGame NOTIFY currentGameChanged)

public:
    explicit ra2snes(QObject *parent = nullptr);
    ~ra2snes();

    QString currentGame() const;
    AchievementModel* achievementModel();

public slots:
    void signIn(const QString &username, const QString &password);

signals:
    void currentGameChanged();
    void loginSuccess();
    void loginFailed(QString error);
    void achievementModelReady();

private:
    Usb2Snes *usb2snes;
    RAClient *raclient;
    MemoryReader *reader;
    AchievementModel *achievement_model;
    QString m_currentGame;
    bool loggedin;
    bool gameLoaded;
    QAtomicInt tasksFinished;

    void onLoginSuccess();
    void proccessRequestFailed(QJsonObject error);
    void onRequestError();
    void onUsb2SnesStateChanged();
    void setCurrentConsole();
};

#endif // RA2SNES_H
