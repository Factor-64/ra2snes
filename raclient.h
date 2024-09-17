#ifndef RACLIENT_H
#define RACLIENT_H

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQueue>
#include <QJsonObject>
#include "rastructs.h"

class RAClient : public QObject {
    Q_OBJECT

public:
    enum RequestType {
        AchievementRequest,
        LeaderboardRequest
    };

    struct RequestData {
        RequestType type;
        unsigned int id;
        unsigned int score;
    };

    explicit RAClient(QObject *parent = nullptr);

    void loginPassword(const QString& username, const QString& password);
    void loginToken(const QString& username, const QString& token);
    void loadGame(const QString& md5hash);
    void getAchievements(unsigned int gameId);
    void getUnlocks();
    void startSession();
    void awardAchievement(unsigned int id);
    //void getLBPlacements();
    QList<AchievementInfo> getAchievements();
    QList<LeaderboardInfo> getLeaderboards();
    void queueAchievementRequest(unsigned int id);
    void queueLeaderboardRequest(unsigned int id, unsigned int score);
    void runQueue();
    void setHardcore(bool h);
    void setConsole(const QString& c, const QUrl& icon);

signals:
    void loginSuccess();
    void requestFailed(QJsonObject error);
    void requestError();
    void gotGameID(int gameid);
    void finishedGameSetup();
    void finishedUnlockSetup();
    void awardedAchievement(unsigned int id);
    void sessionStarted();

private:
    static const QString baseUrl;
    static const QString userAgent;
    static const QString mediaUrl;
    void sendRequest(const QString& request_type, const QJsonObject& post_content);
    void handleNetworkReply(QNetworkReply *reply);
    void handleSuccessResponse(const QJsonObject& jsonObject);
    void handleAwardAchievementResponse(const QJsonObject& jsonObject);
    void handleLoginResponse(const QJsonObject& jsonObject);
    void handleGameIDResponse(const QJsonObject& jsonObject);
    void handlePatchResponse(const QJsonObject& jsonObject);
    void handleUnlocksResponse(const QJsonObject& jsonObject);
    void handleStartSessionResponse(const QJsonObject& jsonObject);
    QString latestRequest;
    bool ready;
    QNetworkAccessManager* networkManager;
    UserInfo userinfo;
    GameInfo gameinfo;
    QQueue<RequestData> queue;
    bool hardcore;
};

#endif // RACLIENT_H
