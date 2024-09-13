#include "raclient.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <rc_version.h>

const QString RAClient::baseUrl = "https://retroachievements.org/";
const QString RAClient::mediaUrl = "https://media.retroachievements.org/";
const QString RAClient::userAgent = "ra2snes/1.0";

RAClient::RAClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &RAClient::handleNetworkReply);
}

void RAClient::loginPassword(const QString username, const QString password)
{
    QList<QPair<QString, QString>> post_content;
    post_content.append(qMakePair("u", username));
    post_content.append(qMakePair("p", password));

    request("login", post_content);
}

void RAClient::loginToken(const QString username, const QString token)
{
    QList<QPair<QString, QString>> post_content;
    post_content.append(qMakePair("u", username));
    post_content.append(qMakePair("t", token));

    request("login", post_content);
}

void RAClient::loadGame(const QString md5hash)
{
    gameinfo.md5hash = md5hash;
    QList<QPair<QString, QString>> post_content;
    post_content.append(qMakePair("m", md5hash));

    request("gameid", post_content);
}

void RAClient::getAchievements(const unsigned int gameid)
{
    QList<QPair<QString, QString>> post_content;
    post_content.append(qMakePair("u", userinfo.username));
    post_content.append(qMakePair("t", userinfo.token));
    post_content.append(qMakePair("g", QString::number(gameid)));

    request("patch", post_content);
}

void RAClient::getUnlocks()
{
    QList<QPair<QString, QString>> post_content;
    post_content.append(qMakePair("u", userinfo.username));
    post_content.append(qMakePair("t", userinfo.token));
    post_content.append(qMakePair("h", QString::number(hardcore)));
    post_content.append(qMakePair("g", QString::number(gameinfo.id)));

    request("unlocks", post_content);
}

void RAClient::startSession()
{
    QList<QPair<QString, QString>> post_content;
    post_content.append(qMakePair("u", userinfo.username));
    post_content.append(qMakePair("t", userinfo.token));
    post_content.append(qMakePair("g", QString::number(gameinfo.id)));
    post_content.append(qMakePair("m", gameinfo.md5hash));
    post_content.append(qMakePair("l", RCHEEVOS_VERSION_STRING));

    request("startsession", post_content);
}

void RAClient::awardAchievement(unsigned int id)
{
    QByteArray md5hash;
    md5hash.append(QString::number(id).toLocal8Bit());
    md5hash.append(userinfo.username.toLocal8Bit());
    md5hash.append(QString::number(hardcore).toLocal8Bit());
    md5hash = QCryptographicHash::hash(md5hash, QCryptographicHash::Md5);

    QList<QPair<QString, QString>> post_content;
    post_content.append(qMakePair("u", userinfo.username));
    post_content.append(qMakePair("t", userinfo.token));
    post_content.append(qMakePair("a", QString::number(id)));
    post_content.append(qMakePair("h", QString::number(hardcore)));
    post_content.append(qMakePair("v", md5hash.toHex()));

    request("awardachievement", post_content);
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

void RAClient::queueAchievementRequest(unsigned int id)
{
    QJsonObject data;
    data.insert("Request", "Achievement");
    data.insert("ID", QString::number(id));
    queue.append(data);
}

void RAClient::queueLeaderboardRequest(unsigned int id, unsigned int score)
{
    QJsonObject data;
    data.insert("Request", "Leaderboard");
    data.insert("ID", QString::number(id));
    data.insert("Score", QString::number(score));
    queue.append(data);
}

void RAClient::runQueue()
{
    qDebug() << "Queue Size: " << queue.size();
    while(!queue.isEmpty())
    {
        if(ready)
        {
            QJsonObject data = queue.first();
            if(data["Request"].toString() == "Achievement")
                awardAchievement(data["ID"].toInt());
            //if(data["Request"].toString() == "Leaderboard")
                //submitLeaderboardEntry(data["ID"].toInt(), data["Score"].toInt());
            queue.removeFirst();
        }
    }
}

void RAClient::setHardcore(bool h)
{
    hardcore = h;
}

QList<AchievementInfo> RAClient::getAchievements()
{
    return gameinfo.achievements;
}

QList<LeaderboardInfo> RAClient::getLeaderboards()
{
    return gameinfo.leaderboards;
}

void RAClient::request(const QString request_type, const QList<QPair<QString, QString>> post_content)
{
    ready = false;
    QNetworkRequest request{QUrl(baseUrl + "dorequest.php")};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent", userAgent.toUtf8());
    QUrlQuery query;
    query.addQueryItem("r", request_type);
    for(const auto& pair : post_content)
        query.addQueryItem(pair.first, QUrl::toPercentEncoding(pair.second));

    QByteArray postData = query.query(QUrl::EncodeUnicode | QUrl::EncodeSpaces).toLocal8Bit();
    postData.replace("%20", "+");
    latestRequest = request_type;
    manager->post(request, postData);
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
    else if(jsonObject.contains("Error"))
    {
        qDebug() << "Error:" << jsonObject["Error"].toString();
        emit requestFailed();
    }
    else if (latestRequest == "awardachievement")
    {
        if(jsonObject.contains("Success") && jsonObject["Success"].toBool())
        {
            for(auto& achievement : gameinfo.achievements)
                if(achievement.id == jsonObject["AchivementID"].toInt())
                    achievement.unlocked = true;
            emit awardedAchievement();
        }
    }
    else if(latestRequest == "login")
    {
        if(jsonObject.contains("Success") && jsonObject["Success"].toBool())
        {
            if(jsonObject.contains("Success") && jsonObject["Success"].toBool())
            {
                userinfo.username = jsonObject["User"].toString();
                userinfo.token = jsonObject["Token"].toString();
                userinfo.softcore_score = jsonObject["SoftcoreScore"].toInt();
                userinfo.hardcore_score = jsonObject["Score"].toInt();
                userinfo.pfp = (mediaUrl + "UserPic/" + userinfo.username + ".png");
                emit loginSuccess();
            }
            else
                emit loginFailed();
        }
    }

    else if(latestRequest == "gameid")
    {
        if(jsonObject.contains("Success") && jsonObject["Success"].toBool())
        {
            gameinfo.id = jsonObject["GameID"].toInt();
            emit gotGameID(gameinfo.id);
        }
        else
            emit gameLoadFailed();
    }

    else if(latestRequest == "patch")
    {
        QJsonObject patch_data = jsonObject["PatchData"].toObject();
        gameinfo.title = patch_data["Title"].toString();
        gameinfo.image_icon = patch_data["ImageIcon"].toString();
        gameinfo.image_icon_url = QUrl(patch_data["ImageIconURL"].toString());
        gameinfo.game_link = QUrl(baseUrl + "game/" + QString::number(gameinfo.id));

        gameinfo.achievements.clear();
        gameinfo.leaderboards.clear();

        QJsonArray achievements_data = patch_data["Achievements"].toArray();
        for(const auto& achievement : achievements_data)
        {
            AchievementInfo info;
            QJsonObject data = achievement.toObject();
            if(data["Flags"].toInt() == 3)
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
                info.achievement_link = QUrl(baseUrl + "achievement/" + QString::number(info.id));
                gameinfo.achievements.append(info);
            }
        }

        if(hardcore)
        {
            QJsonArray leaderboards_data = patch_data["Leaderboards"].toArray();
            for(const auto& leaderboard : leaderboards_data)
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
    else if(latestRequest == "unlocks")
    {
        QJsonArray data = jsonObject["UserUnlocks"].toArray();
        for(auto id : data)
        {
            for(auto& achievement : gameinfo.achievements)
            {
                if(achievement.id == id.toInt())
                    achievement.unlocked = true;
            }

        }
        emit finishedUnlockSetup();
    }
    else if(latestRequest == "startsession")
    {
        qDebug() << jsonObject;
        QJsonArray unlock_data = jsonObject["Unlocks"].toArray();
        for(const auto& unlock_value : unlock_data)
        {
            QJsonObject unlock = unlock_value.toObject();
            for(auto& achievement : gameinfo.achievements)
            {
                if(unlock["ID"].toInt() == achievement.id)
                    achievement.time_unlocked = QDateTime::fromSecsSinceEpoch(unlock["When"].toInt());
            }
        }
    }
    else
    {
        qDebug() << "Unexpected response:" << jsonObject;
        emit requestError();
    }
    ready = true;
    reply->deleteLater();
}
