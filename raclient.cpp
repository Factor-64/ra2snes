#include "raclient.h"
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "rc_version.h"

const QString RAClient::baseUrl = "https://retroachievements.org/";
const QString RAClient::mediaUrl = "https://media.retroachievements.org/";
const QString RAClient::userAgent = "ra2snes/1.0";

RAClient::RAClient(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager();
    userinfo_model = new UserInfoModel(this);
    gameinfo_model = new GameInfoModel(this);
    achievement_model = new AchievementModel(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &RAClient::handleNetworkReply);
    queue.clear();
    running = false;
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
    gameinfo_model->md5hash() = md5hash;
    QJsonObject post_content;
    post_content["m"] = md5hash;

    sendRequest("gameid", post_content);
}

void RAClient::getAchievements(unsigned int gameid)
{
    QJsonObject post_content;
    post_content["u"] = userinfo_model->username();
    post_content["t"] = userinfo_model->token();
    post_content["g"] = QString::number(gameid);

    sendRequest("patch", post_content);
}

void RAClient::getUnlocks()
{
    QJsonObject post_content;
    post_content["u"] = userinfo_model->username();
    post_content["t"] = userinfo_model->token();
    post_content["h"] = QString::number(userinfo_model->hardcore());
    post_content["g"] = QString::number(gameinfo_model->id());

    sendRequest("unlocks", post_content);
}

void RAClient::startSession()
{
    QJsonObject post_content;
    post_content["u"] = userinfo_model->username();
    post_content["t"] = userinfo_model->token();
    post_content["g"] = QString::number(gameinfo_model->id());
    post_content["h"] = userinfo_model->hardcore();
    post_content["m"] = gameinfo_model->md5hash();
    post_content["l"] = RCHEEVOS_VERSION_STRING;

    sendRequest("startsession", post_content);
}

void RAClient::awardAchievement(unsigned int id, bool hardcore, QDateTime achieved)
{
    QByteArray md5hash;
    md5hash.append(QString::number(id).toLocal8Bit());
    md5hash.append(userinfo_model->username().toLocal8Bit());
    md5hash.append(QString::number(userinfo_model->hardcore()).toLocal8Bit());
    int secondsPassed = std::abs(achieved.secsTo(QDateTime::currentDateTime()));
    md5hash.append(QString::number(id).toLocal8Bit());
    md5hash.append(QString::number(secondsPassed).toLocal8Bit());
    md5hash = QCryptographicHash::hash(md5hash, QCryptographicHash::Md5);

    QJsonObject post_content;
    post_content["u"] = userinfo_model->username();
    post_content["t"] = userinfo_model->token();
    post_content["a"] = QString::number(id);
    post_content["h"] = QString::number(hardcore);
    post_content["v"] = QString(md5hash.toHex());

    sendRequest("awardachievement", post_content);
}


/*void RAClient::getLBPlacements()
{
    if(!gameinfo_model->leaderboards.empty() && index < gameinfo_model->leaderboards.size())
    {
        //qDebug() << "Getting Placements";
        QList<QPair<QString, QString>> post_content;
        post_content.append(qMakePair("i", QString::number(gameinfo_model->leaderboards.at(index).id)));
        post_content.append(qMakePair("u", userinfo_model->username));
        post_content.append(qMakePair("c", "1"));

        request("lbinfo", post_content);
    }
    else index = 0;
}*/

void RAClient::queueAchievementRequest(unsigned int id, QDateTime achieved) {
    RequestData data = {AchievementRequest, id, userinfo_model->hardcore(), achieved, 0};
    queue.enqueue(data);
    if(!running)
        startQueue();
}

void RAClient::queueLeaderboardRequest(unsigned int id, QDateTime achieved, unsigned int score) {
    RequestData data = {LeaderboardRequest, id, true, achieved, score};
    queue.enqueue(data);
    if(!running)
        startQueue();
}

void RAClient::runQueue() {
    //qDebug() << "Queue Size: " << queue.size();
    if(!queue.isEmpty())
    {
        RequestData data = queue.first();
        switch (data.type) {
            case AchievementRequest:
                awardAchievement(data.id, data.hardcore, data.unlock_time);
                break;
            case LeaderboardRequest:
                //submitLeaderboardEntry(data.id, data.score);
                break;
            default:
                break;
        }
    }
    else running = false;
}

int RAClient::queueSize()
{
    return queue.size();
}

bool RAClient::isQueueRunning()
{
    return running;
}

void RAClient::stopQueue()
{
    //qDebug() << "Queue Stopped";
    running = false;
}

void RAClient::startQueue()
{
    //qDebug() << "Queue Started";
    running = true;
    runQueue();
}

void RAClient::clearQueue()
{
    queue.clear();
}

void RAClient::freeModelMemory()
{
    delete userinfo_model;
    delete gameinfo_model;
    delete achievement_model;
}

void RAClient::setWidthHeight(int w, int h)
{
    userinfo_model->height(h);
    userinfo_model->width(w);
}

void RAClient::setPatched(bool p)
{
    userinfo_model->patched(p);
}

void RAClient::setSaveStates(bool s)
{
    userinfo_model->savestates(s);
}

void RAClient::setCheats(bool c)
{
    userinfo_model->cheats(c);
}

void RAClient::setHardcore(bool h)
{
    userinfo_model->hardcore(h);
}

void RAClient::setAutoHardcore(bool ac)
{
    userinfo_model->autohardcore(ac);
}

bool RAClient::getAutoHardcore()
{
    return userinfo_model->autohardcore();
}

void RAClient::setTitle(QString t, QString i, QString l)
{
    gameinfo_model->title(t);
    gameinfo_model->image_icon_url(QUrl(i));
    gameinfo_model->game_link(QUrl(l));
}

bool RAClient::getHardcore()
{
    return userinfo_model->hardcore();
}

UserInfoModel* RAClient::getUserInfoModel()
{
    return userinfo_model;
}

GameInfoModel* RAClient::getGameInfoModel()
{
    return gameinfo_model;
}

int RAClient::getWidth()
{
    return userinfo_model->width();
}

int RAClient::getHeight()
{
    return userinfo_model->height();
}

AchievementModel* RAClient::getAchievementModel()
{
    return achievement_model;
}

void RAClient::clearAchievements()
{
    achievement_model->clearAchievements();
}

void RAClient::clearUser()
{
    userinfo_model->clearUser();
}

void RAClient::clearGame()
{
    gameinfo_model->clearGame();
}

QList<LeaderboardInfo> RAClient::getLeaderboards()
{
    return leaderboards;
}

void RAClient::setConsole(const QString& c, const QUrl& icon)
{
    gameinfo_model->console(c);
    gameinfo_model->console_icon(icon);
}

void RAClient::setAchievementInfo(unsigned int id, AchievementInfoType infotype, int value)
{
    switch(infotype)
    {
        case Value:
            achievement_model->updateAchievementValue(id, value);
            break;
        case Percent:
            achievement_model->updateAchievementPercent(id, value);
            break;
        case Primed:
            achievement_model->primeAchievement(id, value);
            break;
        case Target:
            achievement_model->updateAchievementTarget(id, value);
            break;

    }
}

void RAClient::sendRequest(const QString& request_type, const QJsonObject& post_content)
{
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

/*void printJsonObject(const QJsonObject &jsonObject, int indent = 0) {
    QString indentString(indent, ' ');
    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        if (it.value().isObject()) {
            //qDebug() << indentString << it.key() << ": {";
            printJsonObject(it.value().toObject(), indent + 4);
            //qDebug() << indentString << "}";
        } else if (it.value().isArray()) {
            //qDebug() << indentString << it.key() << ": [";
            for (const auto &value : it.value().toArray()) {
                if (value.isObject()) {
                    printJsonObject(value.toObject(), indent + 4);
                } else {
                    //qDebug() << indentString << "  " << value.toString();
                }
            }
            //qDebug() << indentString << "]";
        } else {
            //qDebug() << indentString << it.key() << ":" << it.value().toString();
        }
    }
}

void printAchievements(QList<AchievementInfo> list)
{
    //qDebug() << list.size();
    for(const auto achievement : list)
    {
        //qDebug() << achievement.title;
        //qDebug() << achievement.flags;
    }
}*/

void RAClient::handleNetworkReply(QNetworkReply *reply)
{
    //qDebug() << "Latest Request: " << latestRequest;
    QJsonObject jsonObject = QJsonDocument::fromJson(reply->readAll()).object();
    //printJsonObject(jsonObject);

    if (reply->error() != QNetworkReply::NoError)
    {
        //qDebug() << "Network error:" << reply->errorString();
        emit requestError();
        if(running)
            emit continueQueue();
    }
    else if (jsonObject.contains("Error"))
    {
        //qDebug() << "Error:" << jsonObject["Error"].toString();
        emit requestFailed(jsonObject);
    }
    else
    {
        handleSuccessResponse(jsonObject);
    }
    if(running)
    {
        queue.removeFirst();
        if(!gameinfo_model->mastered())
            if(!isGameMastered())
                isGameBeaten();
        emit continueQueue();
    }
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
        //qDebug() << "Unexpected response:" << jsonObject;
        emit requestError();
    }
}

void RAClient::handleAwardAchievementResponse(const QJsonObject& jsonObject)
{
    for (auto& achievement : achievement_model->getAchievements())
    {
        if (achievement.id == jsonObject["AchievementID"].toInt())
        {
            achievement_model->setUnlockedState(achievement.id, true, queue.first().unlock_time);
            gameinfo_model->updatePointCount(achievement.points);
            //qDebug() << "AWARDED";
            gameinfo_model->updateCompletionCount();
        }
    }
}

void RAClient::handleLoginResponse(const QJsonObject& jsonObject)
{
    userinfo_model->username(jsonObject["User"].toString());
    userinfo_model->token(jsonObject["Token"].toString());
    userinfo_model->softcore_score(jsonObject["SoftcoreScore"].toInt());
    userinfo_model->hardcore_score(jsonObject["Score"].toInt());
    userinfo_model->pfp((mediaUrl + "UserPic/" + userinfo_model->username() + ".png"));
    userinfo_model->link(("https://retroachievement_model->org/user/" + userinfo_model->username()));
    emit loginSuccess();
}

void RAClient::handleGameIDResponse(const QJsonObject& jsonObject)
{
    gameinfo_model->id(jsonObject["GameID"].toInt());
    emit gotGameID(gameinfo_model->id());
}

void RAClient::handlePatchResponse(const QJsonObject& jsonObject)
{
    QJsonObject patch_data = jsonObject["PatchData"].toObject();
    gameinfo_model->title(patch_data["Title"].toString());
    gameinfo_model->image_icon(patch_data["ImageIcon"].toString());
    gameinfo_model->image_icon_url(QUrl(patch_data["ImageIconURL"].toString()));
    gameinfo_model->game_link(QUrl(baseUrl + "game/" + QString::number(gameinfo_model->id())));
    gameinfo_model->missable_count(0);
    gameinfo_model->point_total(0);
    gameinfo_model->mastered(false);
    gameinfo_model->completion_count(0);
    gameinfo_model->beaten(false);
    gameinfo_model->point_count(0);
    gameinfo_model->achievement_count(0);
    int missables = 0;
    int total = 0;

    achievement_model->clearAchievements();

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
            info.time_unlocked_string = "";
            info.time_unlocked = QDateTime(QDate(1990, 11, 21), QTime(0, 0, 0));
            info.achievement_link = QUrl(baseUrl + "achievement/" + QString::number(info.id));
            info.primed = false;
            info.value = 0;
            info.target = 0;
            info.percent = 0;
            if(info.type == "missable")
                missables++;
            total += info.points;
            achievement_model->appendAchievement(info);
        }
    }
    gameinfo_model->missable_count(missables);
    gameinfo_model->point_total(total);
    gameinfo_model->achievement_count(achievement_model->rowCount());

    if (userinfo_model->hardcore())
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
        }
    }

    emit finishedGameSetup();
}

void RAClient::handleUnlocksResponse(const QJsonObject& jsonObject)
{
    emit finishedUnlockSetup();
}

void RAClient::handleStartSessionResponse(const QJsonObject& jsonObject)
{
    //qDebug() << jsonObject;
    QJsonArray unlock_data;
    if(userinfo_model->hardcore())
    {
        unlock_data = jsonObject["HardcoreUnlocks"].toArray();
    }
    else
        unlock_data = jsonObject["Unlocks"].toArray();
    gameinfo_model->completion_count(unlock_data.count());
    for (int i = 0; i < unlock_data.size(); ++i)
    {
        QJsonObject unlock = unlock_data[i].toObject();
        for (auto& achievement : achievement_model->getAchievements())
        {
            if(achievement.type == "progression" || achievement.type == "win_condition")
                progressionMap[achievement.id] = achievement.unlocked;
            if (unlock["ID"].toInt() == achievement.id)
            {
                achievement_model->setUnlockedState(achievement.id, true, QDateTime::fromSecsSinceEpoch(unlock["When"].toInt()));
                gameinfo_model->updatePointCount(achievement.points);
                if(userinfo_model->hardcore())
                    userinfo_model->updateHardcoreScore(achievement.points);
                else
                    userinfo_model->updateSoftcoreScore(achievement.points);
            }
        }
    }
    isGameBeaten();
    isGameMastered();
    emit sessionStarted();
}

bool RAClient::isGameBeaten()
{
    if(progressionMap.empty() && achievement_model->rowCount() > 0)
        return false;
    for(auto it = progressionMap.constBegin(); it != progressionMap.constEnd(); ++it) {
        if(!it.value()) {
            gameinfo_model->beaten(false);
            return false;
        }
    }
    gameinfo_model->beaten(true);
    return true;
}

bool RAClient::isGameMastered()
{
    if(!(achievement_model->rowCount() > 0))
        return false;
    if(gameinfo_model->completion_count() == achievement_model->rowCount())
    {
        gameinfo_model->mastered(true);
        return true;
    }
    else
    {
        gameinfo_model->mastered(false);
        return false;
    }
}
