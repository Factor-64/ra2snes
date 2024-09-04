#ifndef RAWEBCLIENT_H
#define RAWEBCLIENT_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class RAWebClient : public QObject {
    Q_OBJECT

public:
    explicit RAWebClient(QObject *parent = nullptr);

    struct AchievementInfo {
        QUrl badgeLockedUrl;
        QString badgeName;
        QUrl badgeUrl;
        //QDateTime created;
        QString description;
        unsigned int flags;
        unsigned int id;
        QString memAddr;
        //QDateTime modified;
        unsigned int points;
        unsigned int rarity;
        unsigned int rarityHardcore;
        QString title;
        unsigned int type;
        QString author;
    };

    struct UserInfo {
        QString username;
        QString displayname;
        QString token;
        int softcore_score;
        int hardcore_score;
    };

    struct GameInfo {
        QString title;
        QString md5hash;
        unsigned int id;
        QString imageIcon;
        QUrl imageIconUrl;
        QList<AchievementInfo> achievements;
    };

    void login_password(const QString username, const QString password);
    void login_token(const QString username, const QString token);
    void loadGame(const QString md5hash);
    void checkAchievements(const unsigned int gameId);

signals:
    void loginFailed();
    void loginSuccess();
    void requestFailed();
    void requestError();
    void gotGameID(int gameid);
    void finishedGameSetup();

private:
    static const QString baseUrl;
    static const QString userAgent;
    void request(const QString request_type, const QList<QPair<QString, QString>> post_content);
    void handleNetworkReply(QNetworkReply *reply);
    QString latestRequest;
    QNetworkAccessManager *manager;
    UserInfo userinfo;
    GameInfo gameinfo;
};

#endif // RAWEBCLIENT_H
