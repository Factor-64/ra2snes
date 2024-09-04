#include "RAWebClient.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>

const QString RAWebClient::baseUrl = "https://retroachievements.org/dorequest.php";
const QString RAWebClient::userAgent = "ra2snes/1.0";

RAWebClient::RAWebClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &RAWebClient::handleNetworkReply);
}

void RAWebClient::login_password(const QString username, const QString password)
{
    QList<QPair<QString, QString>> post_content;
    post_content.append(qMakePair("u", username));
    post_content.append(qMakePair("p", password));

    request("login", post_content);
}

void RAWebClient::login_token(const QString username, const QString token)
{
    QList<QPair<QString, QString>> post_content;
    post_content.append(qMakePair("u", username));
    post_content.append(qMakePair("t", token));

    request("login", post_content);
}

void RAWebClient::loadGame(const QString md5hash)
{
    QList<QPair<QString, QString>> post_content;
    post_content.append(qMakePair("m", md5hash));

    request("gameid", post_content);
}

void RAWebClient::checkAchievements(const unsigned int gameId)
{
    QList<QPair<QString, QString>> post_content;
    post_content.append(qMakePair("u", userinfo.username));
    post_content.append(qMakePair("t", userinfo.token));
    post_content.append(qMakePair("g", QString::number(gameId)));

    request("patch", post_content);
}

void RAWebClient::request(const QString request_type, const QList<QPair<QString, QString>> post_content)
{
    QNetworkRequest request{QUrl(baseUrl)};
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

/*void printJsonObject(const QJsonObject &jsonObject, int indent = 0) {
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
}*/

void RAWebClient::handleNetworkReply(QNetworkReply *reply)
{
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

    else if(latestRequest == "login")
    {
        if(jsonObject.contains("Success") && jsonObject["Success"].toBool())
        {
            userinfo.username = jsonObject["User"].toString();
            userinfo.token = jsonObject["Token"].toString();
            userinfo.softcore_score = jsonObject["SoftcoreScore"].toInt();
            userinfo.hardcore_score = jsonObject["Score"].toInt();
            emit loginSuccess();
        }
        else
            emit loginFailed();
    }

    else if(latestRequest == "gameid")
    {
        gameinfo.id = jsonObject["GameID"].toInt();
        emit gotGameID(gameinfo.id);
    }

    else if(latestRequest == "patch")
    {
        QJsonObject patch_data = jsonObject["PatchData"].toObject();
        gameinfo.title = patch_data["Title"].toString();
        gameinfo.imageIcon = patch_data["ImageIcon"].toString();
        gameinfo.imageIconUrl = QUrl(patch_data["ImageIconURL"].toString());
        gameinfo.achievements.clear();
        QJsonArray achievements_data = patch_data["Achievements"].toArray();
        for(const auto achievement : achievements_data)
        {
            AchievementInfo info;
            QJsonObject data = achievement.toObject();
            info.badgeLockedUrl = QUrl(data["BadgeLockedURL"].toString());
            info.badgeName = data["BadgeName"].toString();
            info.badgeUrl = QUrl(data["BadgeURL"].toString());
            info.description = data["Desciption"].toString();
            //info.created = QDateTime::fromString(data["Created"].toString());
            info.flags = data["Flags"].toInt();
            info.id = data["ID"].toInt();
            info.memAddr = data["MemAddr"].toString();
            //info.modified = QDateTime::fromString(data["Modified"].toString());
            info.points = data["Points"].toInt();
            info.rarity = data["Rarity"].toInt();
            info.rarityHardcore = data["RarityHardcore"].toInt();
            info.title = data["Title"].toString();
            info.type = data["Type"].toInt();
            info.author = data["Author"].toString();
            gameinfo.achievements.append(info);
        }
        emit finishedGameSetup();
    }

    else
    {
        qDebug() << "Unexpected response:" << jsonObject;
        emit requestError();
    }

    reply->deleteLater();
}
