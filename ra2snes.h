#ifndef RA2SNES_H
#define RA2SNES_H

#include "usb2snes.h"
#include "raclient.h"
#include "memoryreader.h"
#include "achievementmodel.h"
#include "gameinfomodel.h"
#include "userinfomodel.h"

class ra2snes : public QObject
{
    Q_OBJECT

public:
    explicit ra2snes(QObject *parent = nullptr);
    ~ra2snes();

    AchievementModel* achievementModel();
    GameInfoModel* gameInfoModel();
    UserInfoModel* userInfoModel();
    bool isRemembered();
    QString xorEncryptDecrypt(const QString &token, const QString &key);


public slots:
    void signIn(const QString &username, const QString &password, bool remember);
    void signOut();
    void saveWindowSize(int w, int h);
    void changeMode();

signals:
    void loginSuccess();
    void loginFailed(QString error);
    void changeModeFailed(QString reason);
    void achievementModelReady();
    void signedOut();

private:
    Usb2Snes *usb2snes;
    RAClient *raclient;
    MemoryReader *reader;
    AchievementModel *achievement_model;
    GameInfoModel *gameinfo_model;
    UserInfoModel *userinfo_model;
    QString m_currentGame;
    bool loggedin;
    bool gameLoaded;
    bool remember_me;
    bool reset;
    QAtomicInt tasksFinished;
    QString console;

    void createSettingsFile();
    void loadSettings();
    void onLoginSuccess();
    void proccessRequestFailed(QJsonObject error);
    void onRequestError();
    void onUsb2SnesStateChanged();
    void setCurrentConsole();
};

#endif // RA2SNES_H
