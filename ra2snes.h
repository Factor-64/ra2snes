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
    void switchingMode();
    void clearedAchievements();
    void displayMessage(QString error, bool iserror);

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
    bool gameSetup;
    bool isGB;
    QAtomicInt tasksFinished;
    QString console;
    QThread* thread;

    void createSettingsFile();
    void loadSettings();
    void onLoginSuccess();
    void onRequestFailed(QJsonObject error);
    void onRequestError();
    void onUsb2SnesStateChanged();
    void onUsb2SnesGetAddressDataReceived();
    void onUsb2SnesGetAddressesDataReceived();
    void onUsb2SnesGetConfigDataReceived();
    void onUsb2SnesGetFileDataReceived();
    void onUsb2SnesInfoDone(Usb2Snes::DeviceInfo infos);
    void setCurrentConsole();
};

#endif // RA2SNES_H
