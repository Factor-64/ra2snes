#include "ra2snes.h"
#include "./ui_ra2snes.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include "usb2snes.h"
#include "rawebclient.h"

ra2snes::ra2snes(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ra2snes)
    , webclient(new RAWebClient(this))
    , usb2snes(new Usb2Snes(false))
{
    ui->setupUi(this);
    ui->profile->setEnabled(false);
    ui->profile->setVisible(false);

    currentGame = "/sd2snes/m3nu.bin";
    loggedin = false;
    gameLoaded = false;

    connect(webclient, &RAWebClient::loginSuccess, this, &ra2snes::onLoginSuccess);
    connect(webclient, &RAWebClient::requestFailed, this, &ra2snes::onLoginFailed);
    connect(webclient, &RAWebClient::requestError, this, &ra2snes::onRequestError);
    connect(webclient, &RAWebClient::gotGameID, this, [=] (int id){
        webclient->checkAchievements(id);
        gameLoaded = true;
    });

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
                usb2snes->infos();
            else
            {
                if(!gameLoaded && loggedin)
                {
                    qDebug() << "Sending";
                    QTimer::singleShot(0, this, [=] {
                        usb2snes->getFile(currentGame);
                    });
                }
                else
                    usb2snes->infos();
            }
        }
    });

    connect(usb2snes, &Usb2Snes::getFileDataReceived, this, [=] {
        QByteArray romData = usb2snes->getBinaryData();
        QByteArray md5Hash = QCryptographicHash::hash(romData, QCryptographicHash::Md5);
        webclient->loadGame(md5Hash.toHex());
    });

    /*connect(usb2snes, &Usb2Snes::getAddressDataReceived, this, [=] {
        QByteArray data = usb2snes->getBinaryData();
        memcpy(snesMemory, data.data(), data.size());
        usb2snes->infos();
    });*/

    QTimer::singleShot(0, this, [=] {
        usb2snes->connect();
    });

}

ra2snes::~ra2snes()
{
    delete ui;
}

void ra2snes::on_signin_button_clicked()
{
    QString username = ui->username_input->text();
    QString password = ui->password_input->text();
    webclient->login_password(username, password);
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
