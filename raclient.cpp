#include "raclient.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <rc_version.h>

const QString RAClient::baseUrl = "https://retroachievements.org/";
const QString RAClient::mediaUrl = "https://media.retroachievements.org/";
const QString RAClient::userAgent = "ra2snes/1.0";

RAClient::RAClient(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager();
    connect(networkManager, &QNetworkAccessManager::finished, this, &RAClient::handleNetworkReply);
}

void RAClient::loginPassword(const QString& username, const QString& password)
{
    QJsonObject post_content;
    post_content["u"] = username;
    post_content["p"] = password;

    sendRequest("login", post_content);
}

void RAClient::loginToken(const QString& username, const QString& token)
{
    QJsonObject post_content;
    post_content["u"] = username;
    post_content["t"] = token;

    sendRequest("login", post_content);
}

void RAClient::loadGame(const QString& md5hash)
{
    gameinfo.md5hash = md5hash;
    QJsonObject post_content;
    post_content["m"] = md5hash;

    sendRequest("gameid", post_content);
}

void RAClient::getAchievements(unsigned int gameid)
{
    QJsonObject post_content;
    post_content["u"] = userinfo.username;
    post_content["t"] = userinfo.token;
    post_content["g"] = QString::number(gameid);

    sendRequest("patch", post_content);
}

void RAClient::getUnlocks()
{
    QJsonObject post_content;
    post_content["u"] = userinfo.username;
    post_content["t"] = userinfo.token;
    post_content["h"] = QString::number(userinfo.hardcore);
    post_content["g"] = QString::number(gameinfo.id);

    sendRequest("unlocks", post_content);
}

void RAClient::startSession()
{
    QJsonObject post_content;
    post_content["u"] = userinfo.username;
    post_content["t"] = userinfo.token;
    post_content["g"] = QString::number(gameinfo.id);
    post_content["m"] = gameinfo.md5hash;
    post_content["h"] = userinfo.hardcore;

    sendRequest("startsession", post_content);
}

void RAClient::awardAchievement(unsigned int id)
{
    QByteArray md5hash;
    md5hash.append(QString::number(id).toLocal8Bit());
    md5hash.append(userinfo.username.toLocal8Bit());
    md5hash.append(QString::number(userinfo.hardcore).toLocal8Bit());
    md5hash = QCryptographicHash::hash(md5hash, QCryptographicHash::Md5);

    QJsonObject post_content;
    post_content["u"] = userinfo.username;
    post_content["t"] = userinfo.token;
    post_content["a"] = QString::number(id);
    post_content["h"] = QString::number(userinfo.hardcore);
    post_content["v"] = QString(md5hash.toHex());

    sendRequest("awardachievement", post_content);
}


/*void RAClient::getLBPlacements()
{
    if(!gameinfo.leaderboards.empty() && index < gameinfo.leaderboards.size())
    {
        qDebug() << "Getting Placements";
        QList<QPair<QString, QString>> post_content;
        post_content.append(qMakePair("i", QString::number(gameinfo.leaderboards.at(index).id)));
        post_content.append(qMakePair("u", userinfo.username));
        post_content.append(qMakePair("c", "1"));

        request("lbinfo", post_content);
    }
    else index = 0;
}*/

void RAClient::queueAchievementRequest(unsigned int id) {
    RequestData data = {AchievementRequest, id, 0};
    queue.append(data);
}

void RAClient::queueLeaderboardRequest(unsigned int id, unsigned int score) {
    RequestData data = {LeaderboardRequest, id, score};
    queue.append(data);
}

void RAClient::runQueue() {
    qDebug() << "Queue Size: " << queue.size();
    while (!queue.isEmpty()) {
        if (ready) {
            RequestData data = queue.dequeue();
            switch (data.type) {
            case AchievementRequest:
                awardAchievement(data.id);
                break;
            case LeaderboardRequest:
                //submitLeaderboardEntry(data.id, data.score);
                break;
            }
        }
    }
}

void RAClient::setWidthHeight(int w, int h)
{
    userinfo.height = h;
    userinfo.width = w;
}

void RAClient::setPatched(bool p)
{
    userinfo.patched = p;
}

void RAClient::setSaveStates(bool s)
{
    userinfo.savestates = s;
}

void RAClient::setCheats(bool c)
{
    userinfo.cheats = c;
}

void RAClient::setHardcore(bool h)
{
    userinfo.hardcore = h;
}

void RAClient::setTitle(QString t, QString i)
{
    gameinfo.title = t;
    gameinfo.image_icon_url = QUrl(i);
}

bool RAClient::getHardcore()
{
    return userinfo.hardcore;
}

UserInfo RAClient::getUserInfo()
{
    return userinfo;
}

GameInfo RAClient::getGameInfo()
{
    return gameinfo;
}

int RAClient::getWidth()
{
    return userinfo.width;
}

int RAClient::getHeight()
{
    return userinfo.height;
}

QList<AchievementInfo> RAClient::getAchievements()
{
    return gameinfo.achievements;
}

QList<LeaderboardInfo> RAClient::getLeaderboards()
{
    return gameinfo.leaderboards;
}

void RAClient::setConsole(const QString& c, const QUrl& icon)
{
    gameinfo.console = c;
    gameinfo.console_icon = icon;
}

void RAClient::sendRequest(const QString& request_type, const QJsonObject& post_content)
{
    ready = false;
    QNetworkRequest request{QUrl(baseUrl + "dorequest.php")};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent", userAgent.toUtf8());

    QUrlQuery query;
    query.addQueryItem("r", request_type);
    for (auto it = post_content.begin(); it != post_content.end(); ++it)
        query.addQueryItem(it.key(), QUrl::toPercentEncoding(it.value().toString()));

    QByteArray postData = query.query(QUrl::EncodeUnicode | QUrl::EncodeSpaces).toLocal8Bit();
    postData.replace("%20", "+");
    latestRequest = request_type;
    networkManager->post(request, postData);
}

void printJsonObject(const QJsonObject &jsonObject, int indent = 0) {
    QString indentString(indent, ' ');
    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        if (it.value().isObject()) {
            qDebug() << indentString << it.key() << ": {";
            printJsonObject(it.value().toObject(), indent + 4);
            qDebug() << indentString << "}";
        } else if (it.value().isArray()) {
            qDebug() << indentString << it.key() << ": [";
            for (const auto &value : it.value().toArray()) {
                if (value.isObject()) {
                    printJsonObject(value.toObject(), indent + 4);
                } else {
                    qDebug() << indentString << "  " << value.toString();
                }
            }
            qDebug() << indentString << "]";
        } else {
            qDebug() << indentString << it.key() << ":" << it.value().toString();
        }
    }
}

void printAchievements(QList<AchievementInfo> list)
{
    qDebug() << list.size();
    for(const auto achievement : list)
    {
        qDebug() << achievement.title;
        qDebug() << achievement.flags;
    }
}

void RAClient::handleNetworkReply(QNetworkReply *reply)
{
    qDebug() << "Latest Request: " << latestRequest;
    QJsonObject jsonObject = QJsonDocument::fromJson(reply->readAll()).object();
    //printJsonObject(jsonObject);

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network error:" << reply->errorString();
        emit requestError();
    }
    else if (jsonObject.contains("Error"))
    {
        qDebug() << "Error:" << jsonObject["Error"].toString();
        emit requestFailed(jsonObject);
    }
    else
    {
        handleSuccessResponse(jsonObject);
    }

    ready = true;
    reply->deleteLater();
}

void RAClient::handleSuccessResponse(const QJsonObject& jsonObject)
{
    if (latestRequest == "awardachievement")
    {
        handleAwardAchievementResponse(jsonObject);
    }
    else if (latestRequest == "login")
    {
        handleLoginResponse(jsonObject);
    }
    else if (latestRequest == "gameid")
    {
        handleGameIDResponse(jsonObject);
    }
    else if (latestRequest == "patch")
    {
        handlePatchResponse(jsonObject);
    }
    else if (latestRequest == "unlocks")
    {
        handleUnlocksResponse(jsonObject);
    }
    else if (latestRequest == "startsession")
    {
        handleStartSessionResponse(jsonObject);
    }
    else
    {
        qDebug() << "Unexpected response:" << jsonObject;
        emit requestError();
    }
}

void RAClient::handleAwardAchievementResponse(const QJsonObject& jsonObject)
{
    for (auto& achievement : gameinfo.achievements)
    {
        if (achievement.id == jsonObject["AchievementID"].toInt())
        {
            achievement.time_unlocked = QDateTime::currentDateTime().toString("MMMM d yyyy, h:mmap");
            achievement.unlocked = true;
            qDebug() << "AWARDED";
            gameinfo.completion_count++;
            emit awardedAchievement(achievement.id);
        }
    }
}

void RAClient::handleLoginResponse(const QJsonObject& jsonObject)
{
    userinfo.username = jsonObject["User"].toString();
    userinfo.token = jsonObject["Token"].toString();
    userinfo.softcore_score = jsonObject["SoftcoreScore"].toInt();
    userinfo.hardcore_score = jsonObject["Score"].toInt();
    userinfo.pfp = (mediaUrl + "UserPic/" + userinfo.username + ".png");
    userinfo.link = ("https://retroachievements.org/user/" + userinfo.username);
    emit loginSuccess();
}

void RAClient::handleGameIDResponse(const QJsonObject& jsonObject)
{
    gameinfo.id = jsonObject["GameID"].toInt();
    emit gotGameID(gameinfo.id);
}

void RAClient::handlePatchResponse(const QJsonObject& jsonObject)
{
    QJsonObject patch_data = jsonObject["PatchData"].toObject();
    gameinfo.title = patch_data["Title"].toString();
    gameinfo.image_icon = patch_data["ImageIcon"].toString();
    gameinfo.image_icon_url = QUrl(patch_data["ImageIconURL"].toString());
    gameinfo.game_link = QUrl(baseUrl + "game/" + QString::number(gameinfo.id));

    gameinfo.achievements.clear();
    gameinfo.leaderboards.clear();

    QJsonArray achievements_data = patch_data["Achievements"].toArray();
    for (const auto& achievement : achievements_data)
    {
        AchievementInfo info;
        QJsonObject data = achievement.toObject();
        if (data["Flags"].toInt() == 3)
        {
            info.badge_locked_url = QUrl(data["BadgeLockedURL"].toString());
            info.badge_name = data["BadgeName"].toString();
            info.badge_url = QUrl(data["BadgeURL"].toString());
            info.description = data["Description"].toString();
            info.flags = data["Flags"].toInt();
            info.id = data["ID"].toInt();
            info.mem_addr = data["MemAddr"].toString();
            info.points = data["Points"].toInt();
            info.rarity = data["Rarity"].toInt();
            info.rarity_hardcore = data["RarityHardcore"].toInt();
            info.title = data["Title"].toString();
            info.type = data["Type"].toString();
            info.author = data["Author"].toString();
            info.unlocked = false;
            info.time_unlocked = "";
            info.achievement_link = QUrl(baseUrl + "achievement/" + QString::number(info.id));
            gameinfo.achievements.append(info);
        }
    }

    if (userinfo.hardcore)
    {
        QJsonArray leaderboards_data = patch_data["Leaderboards"].toArray();
        for (const auto& leaderboard : leaderboards_data)
        {
            LeaderboardInfo info;
            QJsonObject data = leaderboard.toObject();
            info.description = data["Description"].toString();
            info.format = data["Format"].isString();
            info.id = data["id"].toInt();
            info.lower_is_better = data["LowerIsBetter"].toInt();
            info.mem_addr = data["Mem"].toString();
            info.leaderboard_link = QUrl(baseUrl + "leaderboard/" + QString::number(info.id));
            gameinfo.leaderboards.append(info);
        }
    }

    emit finishedGameSetup();
}

void RAClient::handleUnlocksResponse(const QJsonObject& jsonObject)
{
    QJsonArray data = jsonObject["UserUnlocks"].toArray();
    for (auto id : data)
    {
        for (auto& achievement : gameinfo.achievements)
        {
            if (achievement.id == id.toInt())
                achievement.unlocked = true;
        }
    }
    emit finishedUnlockSetup();
}

void RAClient::handleStartSessionResponse(const QJsonObject& jsonObject)
{
    qDebug() << jsonObject;
    QJsonArray unlock_data = jsonObject["Unlocks"].toArray();
    gameinfo.completion_count = unlock_data.count();
    for (const auto& unlock_value : unlock_data)
    {
        QJsonObject unlock = unlock_value.toObject();
        for (auto& achievement : gameinfo.achievements)
        {
            if (unlock["ID"].toInt() == achievement.id)
            {
                achievement.time_unlocked = QDateTime::fromSecsSinceEpoch(unlock["When"].toInt()).toString("MMMM d yyyy, h:mmap");
            }
        }
    }
    emit sessionStarted();
}
