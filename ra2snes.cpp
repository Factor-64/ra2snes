#include "ra2snes.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QThreadPool>

ra2snes::ra2snes(QObject *parent)
    : QObject(parent)
    , usb2snes(new Usb2Snes(false))
    , raclient(new RAClient(this))
    , reader(new MemoryReader(this))
    , achievement_model(new AchievementModel(this))
    , gameinfo_model(new GameInfoModel(this))
    , userinfo_model(new UserInfoModel(this))
{

    m_currentGame = "/sd2snes/m3nu.bin";
    loggedin = false;
    gameLoaded = false;
    tasksFinished = 0;
    raclient->setHardcore(true);
    console = "SNES";
    remember_me = false;

    connect(usb2snes, &Usb2Snes::stateChanged, this, &ra2snes::onUsb2SnesStateChanged);

    connect(usb2snes, &Usb2Snes::connected, this, [=]() {
        usb2snes->setAppName("ra2snes");
        qDebug() << "Connected to usb2snes server, trying to find a suitable device";
        usb2snes->deviceList();
    });

    connect(usb2snes, &Usb2Snes::disconnected, this, [=]() {
        qDebug() << "Disconnected, trying to reconnect in 1 sec";
        QTimer::singleShot(1000, this, [=] {
            usb2snes->connect();
        });
    });

    connect(usb2snes, &Usb2Snes::deviceListDone, this, [=] (QStringList devices) {
        if (!devices.empty())
        {
            usb2snes->attach(devices.at(0));
            usb2snes->infos(true);
            QTimer::singleShot(1000, this, [=] {
                usb2snes->infos();
            });
        }
        else
        {
            QTimer::singleShot(1000, this, [=] {
                if (usb2snes->state() == Usb2Snes::Connected)
                    usb2snes->deviceList();
            });
        }
    });

    connect(usb2snes, &Usb2Snes::infoDone, this, [=] (Usb2Snes::DeviceInfo infos) {
        if (!infos.flags.contains("NO_FILE_CMD"))
        {
            if(m_currentGame.contains("m3nu.bin") || m_currentGame.contains("menu.bin"))
            {
                gameLoaded = false;
                usb2snes->getConfig();
                raclient->setPatched(false);
                userinfo_model->setPatched(false);
                /*QTimer::singleShot(1000, this, [=] {
                    usb2snes->infos();
                });*/
            }
            else if(gameLoaded && loggedin)
                tasksFinished += 2;
            else if(!gameLoaded && loggedin)
            {
                usb2snes->getConfig();
                setCurrentConsole();
                QTimer::singleShot(1000, this, [=] {
                    usb2snes->getFile(m_currentGame);
                });
            }
        }
    });

    connect(usb2snes, &Usb2Snes::getFileDataReceived, this, [=] {
        QByteArray romData = usb2snes->getBinaryData();
        if (romData.size() & 512)
            romData = romData.mid(512);
        QByteArray md5Hash = QCryptographicHash::hash(romData, QCryptographicHash::Md5);
        usb2snes->isPatchedROM();
        raclient->loadGame(md5Hash.toHex());
    });

    connect(usb2snes, &Usb2Snes::getConfigDataReceived, this, [=] {
        qDebug() << "Checking config";
        QString config = QString::fromUtf8(usb2snes->getBinaryData());
        bool c = true;
        bool s = true;
        if(config.contains("EnableCheats: false"))
        {
            c = false;
        }
        if(config.contains("EnableIngameSavestate: 0"))
        {
            s = false;
        }
        if(c || s)
        {
            raclient->setHardcore(false);
            userinfo_model->setHardcore(false);
        }
        else
        {
            qDebug() << "Config check passed";
            raclient->setHardcore(true);
            userinfo_model->setHardcore(true);
        }

        raclient->setCheats(c);
        raclient->setSaveStates(s);
        userinfo_model->setCheats(c);
        userinfo_model->setSaveStates(s);
    });

    connect(usb2snes, &Usb2Snes::getAddressesDataReceived, this, [=] {
        QThreadPool::globalInstance()->start([=] { raclient->runQueue(); });
        QByteArray data = usb2snes->getBinaryData();
        tasksFinished = 0;
        memcpy(reader->getConsoleMemory(), data.data(), data.size());
        QThreadPool::globalInstance()->start([=] { reader->checkAchievements(); });
        QThreadPool::globalInstance()->start([=] { reader->checkLeaderboards(); });
        usb2snes->isPatchedROM();
    });

    connect(usb2snes, &Usb2Snes::getAddressDataReceived, this, [=] {
        QByteArray data = usb2snes->getBinaryData();
        qDebug() << "Checking for patched rom";
        if(data != QByteArray::fromHex("00000000") || data[0] != (char) 0x60)
        {
            qDebug() << "ROM PATCHED!";
            raclient->setHardcore(false);
            userinfo_model->setHardcore(false);
            raclient->setPatched(true);
            userinfo_model->setPatched(true);
        }
    });

    QTimer::singleShot(0, this, [=] {
        usb2snes->connect();
    });

    connect(raclient, &RAClient::loginSuccess, this, &ra2snes::onLoginSuccess);
    connect(raclient, &RAClient::requestFailed, this, [=] (QJsonObject error){
        proccessRequestFailed(error);
    });
    connect(raclient, &RAClient::requestError, this, &ra2snes::onRequestError);
    connect(raclient, &RAClient::gotGameID, this, [=] (int id){
        raclient->getAchievements(id);
        gameLoaded = true;
    });

    connect(raclient, &RAClient::finishedGameSetup, this, [=] {
        gameinfo_model->setGameInfo(raclient->getGameInfo());
        raclient->getUnlocks();
        //raclient->getLBPlacements();
    });

    connect(raclient, &RAClient::finishedUnlockSetup, this, [=] {
        raclient->startSession();
        //raclient->getLBPlacements();
    });
    connect(raclient, &RAClient::sessionStarted, this, [=] {
        achievement_model->setAchievements(raclient->getAchievements());
        emit achievementModelReady();
        //reader->initTriggers(raclient->getAchievements(), raclient->getLeaderboards());
    });

    connect(reader, &MemoryReader::finishedMemorySetup, this, [=] {
        usb2snes->getAddresses(reader->getUniqueMemoryAddresses());
    });

    connect(reader, &MemoryReader::achievementUnlocked, this, [=](unsigned int id) {
        raclient->awardAchievement(id);
    });

    connect(raclient, &RAClient::awardedAchievement, this, [=](unsigned int id) {
        achievement_model->setUnlockedState(id, true);
        gameinfo_model->updateCompletionCount();
    });

    //connect(reader, &MemoryReader::leaderboardCompleted, this, [=](unsigned int id, unsigned int score) {
    //    raclient->queueLeaderboardRequest(id, score);
    //});

    connect(reader, &MemoryReader::achievementsChecked, this, [=]{
        tasksFinished++;
        onUsb2SnesStateChanged();
    });
    connect(reader, &MemoryReader::leaderboardsChecked, this, [=]{
        tasksFinished++;
        onUsb2SnesStateChanged();
    });

    loadSettings();
}

ra2snes::~ra2snes()
{
    createSettingsFile();
    usb2snes->close();
    delete usb2snes;
    reader->freeConsoleMemory();
    delete raclient;
    delete reader;
    delete userinfo_model;
    delete gameinfo_model;
    delete achievement_model;
}

void ra2snes::signIn(const QString &username, const QString &password, bool remember)
{
    remember_me = remember;
    raclient->loginPassword(username, password);
}

void ra2snes::onLoginSuccess()
{
    loggedin = true;
    createSettingsFile();
    userinfo_model->setUserInfo(raclient->getUserInfo());
    emit loginSuccess();
}

void ra2snes::proccessRequestFailed(QJsonObject error)
{
    QString errorMessage = error["Error"].toString();
    qDebug() << "Code:" << error["Code"].toString() << "Error:" << errorMessage;
    if(error["Code"].toString() == "invalid_credentials")
    {
        if(errorMessage.contains("token"))
        {
            remember_me = false;
            createSettingsFile();
        }
        emit loginFailed(errorMessage.remove(" Please try again."));
    }
}

void ra2snes::onRequestError()
{
    qDebug() << "request error";
}

void ra2snes::onUsb2SnesStateChanged()
{
    qDebug() << "Tasks Finished: " << tasksFinished;
    if(usb2snes->state() == Usb2Snes::Ready && gameLoaded)
    {
        switch(tasksFinished)
        {
            case 1:
            case 2: {
                usb2snes->infos();
                break;
            }
            case 4: {
                usb2snes->getAddresses(reader->getUniqueMemoryAddresses());
                break;
            }
            case 5: {
                usb2snes->infos();
                tasksFinished = 0;
                break;
            }
            default:
                break;
        }
    }
}

void ra2snes::setCurrentConsole()
{
    int extensionIndex = m_currentGame.lastIndexOf('.');
    if(extensionIndex != -1)
    {
        QString icon = "https://static.retroachievements.org/assets/images/system/";
        QString extension = m_currentGame.mid(extensionIndex + 1);
        if(extension == "sfc" || extension == "smc" || extension == "swc" || extension == "bs" || extension == "fig")
        {
            icon += "snes.png";
            raclient->setConsole("SNES/Super Famicom", QUrl(icon));
        }
        else if(extension == "gb")
        {
            icon += "gb.png";
            raclient->setConsole("Game Boy", QUrl(icon));
        }
        else
        {
            if(console == "SNES")
            {
                icon += "snes.png";
                raclient->setConsole("SNES/Super Famicom", QUrl(icon));
                raclient->setTitle("SD2SNES Menu", "https://media.retroachievements.org/UserPic/user.png");
                gameinfo_model->setGameInfo(raclient->getGameInfo());
            }
        }
    }
    else
        raclient->setConsole("", QUrl(""));
}

AchievementModel* ra2snes::achievementModel()
{
    return achievement_model;
}

GameInfoModel* ra2snes::gameInfoModel()
{
    return gameinfo_model;
}

UserInfoModel* ra2snes::userInfoModel()
{
    return userinfo_model;
}

bool ra2snes::isRemembered()
{
    return remember_me;
}

QString ra2snes::xorEncryptDecrypt(const QString &token, const QString &key) {
    QString result = token;
    int keyLength = key.length();

    for (int i = 0; i < token.length(); ++i) {
        result[i] = QChar(token[i].unicode() ^ key[i % keyLength].unicode());
    }

    return result;
}

void ra2snes::saveWindowSize(int w, int h)
{
    raclient->setWidthHeight(w, h);
}

void ra2snes::createSettingsFile()
{
    QString appDir = QCoreApplication::applicationDirPath();

    QString settingsFilePath = appDir + QDir::separator() + "settings.ini";

    QSettings settings(settingsFilePath, QSettings::IniFormat);

    settings.setValue("Hardcore", raclient->getHardcore());
    settings.setValue("Console", console);
    settings.setValue("Width", raclient->getWidth());
    settings.setValue("Height", raclient->getHeight());
    if(remember_me)
    {
        QString time = QString::number(QDateTime::currentDateTime().toSecsSinceEpoch());
        UserInfo user = raclient->getUserInfo();
        settings.setValue("Username", user.username);
        settings.setValue("Token", xorEncryptDecrypt(user.token, time));
        settings.setValue("Time", time);
    }
    else
    {
        settings.setValue("Username", "");
        settings.setValue("Token", "");
        settings.setValue("Time", "");
    }

    settings.sync();
}

void ra2snes::loadSettings() {
    QString appDir = QCoreApplication::applicationDirPath();

    QString settingsFilePath = appDir + QDir::separator() + "settings.ini";

    if (QFile::exists(settingsFilePath)) {
        QSettings settings(settingsFilePath, QSettings::IniFormat);

        bool hardcore = settings.value("Hardcore").toBool();
        QString console_v = settings.value("Console").toString();
        QString username = settings.value("Username").toString();
        QString token = settings.value("Token").toString();
        QString time = settings.value("Time").toString();
        int width = settings.value("Width").toInt();
        int height = settings.value("Height").toInt();

        raclient->setHardcore(hardcore);
        raclient->setWidthHeight(width, height);
        console = console_v;

        if(username != "" && token != "" && time != "")
        {
            remember_me = true;
            raclient->loginToken(username, xorEncryptDecrypt(token, time));
        }
    }
    else
    {
        qDebug() << "Settings file does not exist.";
    }
    setCurrentConsole();
}
