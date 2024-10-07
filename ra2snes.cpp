#include "ra2snes.h"
//#include <QDebug>

ra2snes::ra2snes(QObject *parent)
    : QObject(parent)
{
    usb2snes = new Usb2Snes(false);
    raclient = new RAClient(this);
    reader = new MemoryReader(this);
    m_currentGame = "";
    loggedin = false;
    gameSetup = false;
    gameLoaded = false;
    isGB = false;
    reset = false;
    doThisTaskNext = None;
    m_console = "SNES";
    remember_me = false;
    framesPassed = 0;

    raclient->setHardcore(true);
    raclient->setAutoHardcore(false);
    saveWindowSize(600, 600);

    connect(usb2snes, &Usb2Snes::connected, this, [=]() {
        usb2snes->setAppName("ra2snes");
        //qDebug() << "Connected to usb2snes server, trying to find a suitable device";
        raclient->clearAchievements();
        raclient->stopQueue();
        usb2snes->deviceList();
        emit displayMessage("QUsb2Snes Connected", false);
    });

    connect(usb2snes, &Usb2Snes::disconnected, this, [=]() {
        //qDebug() << "Disconnected, trying to reconnect in 1 sec";
        emit displayMessage("QUsb2Snes Not Connected", true);
        raclient->clearAchievements();
        raclient->stopQueue();
        emit switchingMode();
        QTimer::singleShot(1000, this, [=] {
            usb2snes->connect();
        });
    });

    connect(usb2snes, &Usb2Snes::deviceListDone, this, [=] (QStringList devices) {
        if (!devices.empty())
        {
            usb2snes->attach(devices.at(0));
            usb2snes->infos(true);
            reset = true;
            emit displayMessage("Console Connected", false);
        }
        else
        {
            m_currentGame = "";
            emit displayMessage("Console Not Connected", true);
            QTimer::singleShot(1000, this, [=] {
                if (usb2snes->state() == Usb2Snes::Connected)
                    usb2snes->deviceList();
            });
        }
    });

    connect(usb2snes, &Usb2Snes::stateChanged, this, &ra2snes::onUsb2SnesStateChanged);
    connect(usb2snes, &Usb2Snes::gotServerVersion, this, [=] {usb2snes->infos(); });
    connect(usb2snes, &Usb2Snes::infoDone, this, &ra2snes::onUsb2SnesInfoDone);
    connect(usb2snes, &Usb2Snes::getFileDataReceived, this, &ra2snes::onUsb2SnesGetFileDataReceived);
    connect(usb2snes, &Usb2Snes::getConfigDataReceived, this, &ra2snes::onUsb2SnesGetConfigDataReceived);
    connect(usb2snes, &Usb2Snes::getAddressesDataReceived, this, &ra2snes::onUsb2SnesGetAddressesDataReceived);
    connect(usb2snes, &Usb2Snes::getAddressDataReceived, this, &ra2snes::onUsb2SnesGetAddressDataReceived);

    QTimer::singleShot(0, this, [=] { usb2snes->connect(); });

    connect(raclient, &RAClient::continueQueue, this, [=] { raclient->runQueue(); });
    connect(raclient, &RAClient::loginSuccess, this, &ra2snes::onLoginSuccess);
    connect(raclient, &RAClient::requestFailed, this, &ra2snes::onRequestFailed);
    connect(raclient, &RAClient::requestError, this, &ra2snes::onRequestError);
    connect(raclient, &RAClient::gotGameID, this, [=] (int id){
        gameSetup = false;
        gameLoaded = true;
        emit displayMessage("Game Loaded", false);
        raclient->getAchievements(id);
    });

    connect(raclient, &RAClient::finishedGameSetup, this, [=] {
        raclient->getUnlocks();
    });

    connect(raclient, &RAClient::finishedUnlockSetup, this, [=] {
        raclient->startSession();
    });
    connect(raclient, &RAClient::sessionStarted, this, [=] {
        emit achievementModelReady();
        if(!raclient->isQueueRunning())
            raclient->startQueue();
        reader->initTriggers(raclient->getAchievementModel()->getAchievements(), raclient->getLeaderboards());
    });

    connect(reader, &MemoryReader::finishedMemorySetup, this, [=] {
        doThisTaskNext = None;
        millisecPassed = QDateTime::currentDateTime();
        usb2snes->getAddresses(reader->getUniqueMemoryAddresses());
    });

    connect(reader, &MemoryReader::achievementUnlocked, this, [=](unsigned int id, QDateTime time) {
        raclient->queueAchievementRequest(id, time);
    });

    //connect(reader, &MemoryReader::leaderboardCompleted, this, [=](unsigned int id, unsigned int score) {
    //    raclient->queueLeaderboardRequest(id, score);
    //    if(!raclient->isQueueRunning())
    //        QThreadPool::globalInstance()->start([=] { raclient->runQueue(); });
    //});

    loadSettings();
}

ra2snes::~ra2snes()
{
    createSettingsFile();
    usb2snes->close();
    delete usb2snes;
    raclient->freeModelMemory();
    delete raclient;
    delete reader;
}

void ra2snes::signIn(const QString &username, const QString &password, bool remember)
{
    remember_me = remember;
    raclient->loginPassword(username, password);
}

void ra2snes::onUsb2SnesInfoDone(Usb2Snes::DeviceInfo infos)
{
    if (!infos.flags.contains("NO_FILE_CMD"))
    {
        m_currentGame = infos.romPlaying.remove(QChar('\u0000'));
        m_currentGame.replace("?", " ");
        if (m_currentGame.contains("m3nu.bin") || m_currentGame.contains("menu.bin") || reset)
        {
            reset = false;
            if(gameLoaded)
                doThisTaskNext = Reset;
            else
            {
                gameSetup = false;
                raclient->setPatched(false);
                raclient->clearAchievements();
                emit clearedAchievements();
                raclient->clearGame();
                setCurrentConsole();
                //qDebug() << "Menu";
                usb2snes->getConfig();
            }
        }
        else if (!gameLoaded && loggedin)
        {
            emit switchingMode();
            if(raclient->getAutoHardcore() && !raclient->getHardcore())
                changeMode();
            setCurrentConsole();
            gameSetup = true;
            emit displayMessage("Loading... Do Not Turn Off Console!", false);
            usb2snes->getConfig();
        }
    }
}

void ra2snes::onUsb2SnesGetFileDataReceived()
{
    QByteArray romData = usb2snes->getBinaryData();
    if (romData.size() & 512)
        romData = romData.mid(512);
    QByteArray md5Hash = QCryptographicHash::hash(romData, QCryptographicHash::Md5);
    usb2snes->isPatchedROM();
    raclient->loadGame(md5Hash.toHex());
}

void ra2snes::onUsb2SnesGetConfigDataReceived()
{
    //qDebug() << "Checking config";
    QString config = QString::fromUtf8(usb2snes->getBinaryData());
    bool c = !config.contains("EnableCheats: false");
    bool s = !config.contains("EnableIngameSavestate: 0");

    if(isGB)
        s = !config.contains("SGBEnableState: false");
    raclient->setCheats(c);
    raclient->setSaveStates(s);

    if(c || s)
    {
        if(raclient->getHardcore())
            changeMode();
    }
    if(raclient->getAutoHardcore())
        changeMode();
    if(gameSetup)
        doThisTaskNext = GetCurrentGameFile;
    else if(!gameLoaded)
        usb2snes->infos();
}

void ra2snes::onUsb2SnesGetAddressesDataReceived()
{
    doThisTaskNext = GetConsoleInfo;
    //qDebug() << "Current Time:" << QDateTime::currentDateTime();
    framesPassed = std::round(std::abs(millisecPassed.msecsTo(QDateTime::currentDateTime())) * 0.06009);
    //qDebug() << "Frames: " << framesPassed;
    millisecPassed = QDateTime::currentDateTime();
    reader->addFrameToQueues(usb2snes->getBinaryData(), framesPassed);
    //qDebug() << "Queue Size: " << reader->achievementQueueSize();
    if(reader->achievementQueueSize() == 1)
        QThreadPool::globalInstance()->start([=] { reader->checkAchievements(); });
}

void ra2snes::onUsb2SnesGetAddressDataReceived()
{
    QByteArray data = usb2snes->getBinaryData();
    bool patched = false;
    //qDebug() << "Checking for patched rom";
    //qDebug() << data;
    if (usb2snes->firmwareVersion() > QVersionNumber(7))
    {
        if (data != QByteArray::fromHex("00000000"))
            patched = true;
    }
    else if (data[0] != (char)0x60)
        patched = true;
    if (patched)
    {
        //qDebug() << "ROM PATCHED!";
        raclient->setPatched(true);
        changeMode();
    }
    else
        raclient->setPatched(false);
    //qDebug() << "Finished Patch Check";
}

void ra2snes::onLoginSuccess()
{
    loggedin = true;
    createSettingsFile();
    reset = true;
    onUsb2SnesStateChanged();
    emit loginSuccess();
}

void ra2snes::onRequestFailed(QJsonObject error)
{
    QString errorMessage = error["Error"].toString();
    //qDebug() << "Code:" << error["Code"].toString() << "Error:" << errorMessage;
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
    //qDebug() << "request error";
}

void ra2snes::onUsb2SnesStateChanged()
{
    //qDebug() << "Tasks Finished: " << doThisTaskNext;
    //qDebug() << "State: " << usb2snes->state();
    //qDebug() << "Reset? " << reset;
    if(usb2snes->state() == Usb2Snes::Ready)
    {
        if(reset)
            doThisTaskNext = Reset;
        switch(doThisTaskNext)
        {
            case GetConsoleInfo:
                doThisTaskNext = CheckPatched;
                usb2snes->infos();
                break;
            case CheckPatched:
                doThisTaskNext = GetConsoleAddresses;
                if(raclient->getHardcore())
                    usb2snes->isPatchedROM();
                else
                {
                    doThisTaskNext = GetConsoleInfo;
                    usb2snes->getAddresses(reader->getUniqueMemoryAddresses());
                }
                break;
            case GetConsoleAddresses:
                doThisTaskNext = GetConsoleInfo;
                usb2snes->getAddresses(reader->getUniqueMemoryAddresses());
                break;
            case GetCurrentGameFile:
                doThisTaskNext = None;
                usb2snes->getFile(m_currentGame);
                break;
            case Reset:
                doThisTaskNext = None;
                gameLoaded = false;
                usb2snes->infos();
                break;
            default:
                break;
        }

    }
    else if(usb2snes->state() == Usb2Snes::None)
    {
        doThisTaskNext = None;
        gameLoaded = false;
        m_currentGame = "";
        setCurrentConsole();
    }
    else if(usb2snes->state() == Usb2Snes::ReceivingFile)
        emit displayMessage("Loading... Do Not Turn Off Console!", false);
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
            setConsole("SNES");
            isGB = false;
        }
        else if(extension == "gb")
        {
            setConsole("SNES");
            isGB = true;
        }
        if(m_console == "SNES")
        {
            raclient->setTitle("SD2SNES Menu", "", "");
            if(isGB)
            {
                icon += "gb.png";
                raclient->setConsole("Super Game Boy", QUrl(icon));
            }
            else
            {
                icon += "snes.png";
                raclient->setConsole("SNES/Super Famicom", QUrl(icon));
            }
        }
    }
    else
    {
        raclient->setTitle("", "", "");
        raclient->setConsole("", QUrl(""));
    }
}

AchievementModel* ra2snes::achievementModel()
{
    return raclient->getAchievementModel();
}

GameInfoModel* ra2snes::gameInfoModel()
{
    return raclient->getGameInfoModel();
}

UserInfoModel* ra2snes::userInfoModel()
{
    return raclient->getUserInfoModel();
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
    settings.setValue("Console", m_console);
    settings.setValue("Width", raclient->getWidth());
    settings.setValue("Height", raclient->getHeight());
    settings.setValue("Auto", raclient->getAutoHardcore());

    if(remember_me)
    {
        QString time = QString::number(QDateTime::currentDateTime().toSecsSinceEpoch());
        UserInfoModel* user = raclient->getUserInfoModel();
        settings.setValue("Username", user->username());
        settings.setValue("Token", xorEncryptDecrypt(user->token(), time));
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
        bool autoh = settings.value("Auto").toBool();

        raclient->setHardcore(hardcore);
        raclient->setAutoHardcore(autoh);
        raclient->setWidthHeight(width, height);
        setConsole(console_v);

        if(username != "" && token != "" && time != "")
        {
            remember_me = true;
            raclient->loginToken(username, xorEncryptDecrypt(token, time));
        }
    }
    //else
    //{
      //qDebug() << "Settings file does not exist.";
    //}
    setCurrentConsole();
}

void ra2snes::signOut()
{
    raclient->clearQueue();
    loggedin = false;
    gameLoaded = false;
    remember_me = false;
    reset = true;
    raclient->clearAchievements();
    raclient->clearUser();
    raclient->clearGame();
    createSettingsFile();
    loadSettings();
    onUsb2SnesStateChanged();
    emit signedOut();
}

void ra2snes::changeMode()
{
    UserInfoModel* user = raclient->getUserInfoModel();
    QString reason = "Hardcore Disabled: ";
    bool needsChange = false;

    if(user->cheats()) {
        reason += QString("Cheats Enabled");
        needsChange = true;
    }
    if(user->savestates()) {
        if(isGB)
            reason += (QString(needsChange ? ", " : "") + "Super Game Boy SaveStates Enabled");
        else
            reason += (QString(needsChange ? ", " : "") + "SaveStates Enabled");
        needsChange = true;
    }
    if(user->patched()) {
        reason += (QString(needsChange ? ", " : "") + "ROM Patched");
        needsChange = true;
    }
    if(!needsChange)
    {
        if(raclient->getAutoHardcore())
            user->hardcore(true);
        else
            user->hardcore(!user->hardcore());
        emit changeModeFailed("");
    }
    else
    {
        user->hardcore(false);
        emit changeModeFailed(reason);
    }
    if(gameLoaded && loggedin)
    {
        reset = true;
        raclient->clearAchievements();
        raclient->stopQueue();
        onUsb2SnesStateChanged();
        emit switchingMode();
    }
}

void ra2snes::autoChange(bool ac)
{
    raclient->setAutoHardcore(ac);
    if(raclient->getAutoHardcore() && !raclient->getHardcore())
        changeMode();
    emit autoModeChanged();
}

QString ra2snes::console() const
{
    return m_console;
}

void ra2snes::setConsole(const QString &console)
{
    if (m_console != console) {
        m_console = console;
        emit consoleChanged();
    }
}
