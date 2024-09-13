#include "ra2snes.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QThreadPool>

ra2snes::ra2snes(QObject *parent)
    : QObject(parent)
    , usb2snes(new Usb2Snes(false))
    , raclient(new RAClient(this))
    , reader(new MemoryReader(this))
{

    m_currentGame = "/sd2snes/m3nu.bin";
    loggedin = false;
    gameLoaded = false;
    tasksFinished = 0;
    raclient->setHardcore(false);

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
            m_currentGame = infos.romPlaying.remove(QChar('\u0000'));
            emit currentGameChanged();
            if(m_currentGame.contains("m3nu.bin") || m_currentGame.contains("menu.bin"))
            {
                gameLoaded = false;
                usb2snes->infos();
            }
            else if(!gameLoaded && loggedin)
                usb2snes->getFile(m_currentGame);
            else
                tasksFinished++;
        }
    });

    connect(usb2snes, &Usb2Snes::getFileDataReceived, this, [=] {
        QByteArray romData = usb2snes->getBinaryData();
        QByteArray md5Hash = QCryptographicHash::hash(romData, QCryptographicHash::Md5);
        usb2snes->isPatchedROM();
        QByteArray data = usb2snes->getBinaryData();
        if(data != QByteArray::fromHex("00000000") || data[0] != (char) 0x60)
            raclient->setHardcore(false);
        else
            usb2snes->getConfig();
        raclient->loadGame(md5Hash.toHex());
    });

    connect(usb2snes, &Usb2Snes::getConfigDataReceived, this, [=] {
        QString config = QString::fromUtf8(usb2snes->getBinaryData());
        if(config.contains("EnableCheats: false") && config.contains("EnableIngameSavestate: 0"))
            raclient->setHardcore(true);
        else
            raclient->setHardcore(false);
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
            raclient->setHardcore(false);
    });

    QTimer::singleShot(0, this, [=] {
        usb2snes->connect();
    });

    connect(raclient, &RAClient::loginSuccess, this, &ra2snes::onLoginSuccess);
    connect(raclient, &RAClient::requestFailed, this, &ra2snes::onLoginFailed);
    connect(raclient, &RAClient::requestError, this, &ra2snes::onRequestError);
    connect(raclient, &RAClient::gotGameID, this, [=] (int id){
        raclient->getAchievements(id);
        gameLoaded = true;
    });

    connect(raclient, &RAClient::finishedGameSetup, this, [=] {
        raclient->getUnlocks();
        //raclient->getLBPlacements();
    });

    connect(raclient, &RAClient::finishedUnlockSetup, this, [=] {
        raclient->startSession();
        reader->initTriggers(raclient->getAchievements(), raclient->getLeaderboards());
        //raclient->getLBPlacements();
    });

    connect(reader, &MemoryReader::finishedMemorySetup, this, [=] {
        usb2snes->getAddresses(reader->getUniqueMemoryAddresses());
    });

    connect(reader, &MemoryReader::achievementUnlocked, this, [=](unsigned int id) {
        raclient->awardAchievement(id);
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

}

ra2snes::~ra2snes()
{
    usb2snes->close();
    delete usb2snes;
    reader->freeConsoleMemory();
    delete raclient;
    delete reader;
}

void ra2snes::signIn(const QString &username, const QString &password)
{
    raclient->loginPassword(username, password);
}

void ra2snes::onLoginSuccess()
{
    usb2snes->infos();
    loggedin = true;
    emit loginSuccess();
}

void ra2snes::onLoginFailed()
{
    qDebug() << "login failed";
    emit loginFailed();
}

void ra2snes::onRequestError()
{
    qDebug() << "request error";
}

void ra2snes::onUsb2SnesStateChanged()
{
    qDebug() << "Tasks Finished: " << tasksFinished;
    if(usb2snes->state() == Usb2Snes::Ready)
    {
        switch(tasksFinished)
        {
            case 2: {
                usb2snes->infos();
                break;
            }
            case 3: {
                usb2snes->getAddresses(reader->getUniqueMemoryAddresses());
                break;
            }
            default:
                break;
        }
    }
}

QString ra2snes::currentGame() const
{
    return m_currentGame;
}
