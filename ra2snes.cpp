#include "ra2snes.h"
#include "./ui_ra2snes.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QThreadPool>

ra2snes::ra2snes(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ra2snes)
    , raclient(new RAClient(this))
    , usb2snes(new Usb2Snes(false))
    , reader(new MemoryReader(this))
{
    ui->setupUi(this);
    ui->profile->setEnabled(false);
    ui->profile->setVisible(false);

    currentGame = "/sd2snes/m3nu.bin";
    loggedin = false;
    gameLoaded = false;
    tasksFinished = 0;

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
            usb2snes->attach(devices.at(0));
        else
        {
            QTimer::singleShot(1000, this, [=] {
                if (usb2snes->state() == Usb2Snes::Connected)
                    usb2snes->deviceList();
            });
        }
    });

    connect(usb2snes, &Usb2Snes::infoDone, this, [=] (Usb2Snes::DeviceInfo infos) {
        if (infos.flags.contains("NO_FILE_CMD"))
        {
            QMessageBox::information(this, tr("Device error"), tr("The device does not support file operation"));
        }
        else
        {
            currentGame = infos.romPlaying.remove(QChar('\u0000'));
            ui->currentGame->setText(QString(tr("Firmware version : %1 - Rom Playing : %2")).arg(infos.firmwareVersion, currentGame));
            if(currentGame.contains("m3nu.bin") || currentGame.contains("menu.bin"))
            {
                usb2snes->infos();
                gameLoaded = false;
            }
            else
            {
                if(!gameLoaded && loggedin)
                {
                    qDebug() << "Sending";
                    QTimer::singleShot(0, this, [=] {
                        usb2snes->getFile(currentGame);
                    });
                }
            }
        }
    });

    connect(usb2snes, &Usb2Snes::getFileDataReceived, this, [=] {
        QByteArray romData = usb2snes->getBinaryData();
        QByteArray md5Hash = QCryptographicHash::hash(romData, QCryptographicHash::Md5);
        raclient->loadGame(md5Hash.toHex());
    });

    connect(usb2snes, &Usb2Snes::getAddressDataReceived, this, [=] {
        QByteArray data = usb2snes->getBinaryData();
        tasksFinished = 0;
        memcpy(reader->getConsoleMemory(), data.data(), data.size());
        QThreadPool::globalInstance()->start([=] { reader->checkAchievements(); });
        QThreadPool::globalInstance()->start([=] { reader->checkLeaderboards(); });
        usb2snes->infos();
    });

    QTimer::singleShot(0, this, [=] {
        usb2snes->connect();
    });

    connect(raclient, &RAClient::loginSuccess, this, &ra2snes::onLoginSuccess);
    connect(raclient, &RAClient::requestFailed, this, &ra2snes::onLoginFailed);
    connect(raclient, &RAClient::requestError, this, &ra2snes::onRequestError);
    connect(raclient, &RAClient::gotGameID, this, [=] (int id){
        raclient->checkAchievements(id);
        gameLoaded = true;
    });

    connect(raclient, &RAClient::finishedGameSetup, this, [=] {
        reader->initTriggers(raclient->getAchievements(), raclient->getLeaderboards());
        raclient->getUnlocks();
        //raclient->getLBPlacements();
    });

    connect(raclient, &RAClient::finishedUnlockSetup, this, [=] {
        raclient->startSession();
        //raclient->getLBPlacements();
    });

    connect(reader, &MemoryReader::finishedMemorySetup, this, [=] {
        usb2snes->getAddresses(reader->getUniqueMemoryAddresses());
    });

    connect(reader, &MemoryReader::achievementUnlocked, this, [=] {
    });

    connect(reader, &MemoryReader::leaderboardCompleted, this, [=] {
    });

    connect(reader, &MemoryReader::achievementsChecked, this, &ra2snes::onUsb2SnesStateChanged);
    connect(reader, &MemoryReader::leaderboardsChecked, this, &ra2snes::onUsb2SnesStateChanged);

}

ra2snes::~ra2snes()
{
    reader->freeConsoleMemory();
    delete raclient;
    delete reader;
    delete usb2snes;
    delete ui;
}

void ra2snes::on_signin_button_clicked()
{
    QString username = ui->username_input->text();
    QString password = ui->password_input->text();
    raclient->loginPassword(username, password);
}

void ra2snes::onLoginSuccess()
{
    qDebug() << "logged in";
    usb2snes->infos();
    loggedin = true;
}

void ra2snes::onLoginFailed()
{
    qDebug() << "login failed";
}

void ra2snes::onRequestError()
{
    qDebug() << "request error";
}

void ra2snes::resizeWindow(int width, int height)
{
    this->resize(width, height);
}

void ra2snes::onUsb2SnesStateChanged()
{
    if (++tasksFinished == 2 && usb2snes->state() == Usb2Snes::Ready)
        usb2snes->getAddresses(reader->getUniqueMemoryAddresses());
}
