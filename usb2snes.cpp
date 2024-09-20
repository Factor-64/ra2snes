/*
    This file is part of the SaveState2snes software
    Copyright (C) 2017  Sylvain "Skarsnik" Colinet <scolinet@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "usb2snes.h"
#include <QUrl>
#include <QDebug>

Q_LOGGING_CATEGORY(log_Usb2snes, "USB2SNES")
#define sDebug() qCDebug(log_Usb2snes)

Usb2Snes::Usb2Snes(bool autoAttach) : QObject()
{
    m_state = None;
    m_istate = INone;

    QObject::connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &Usb2Snes::onWebSocketTextReceived);
    QObject::connect(&m_webSocket, &QWebSocket::connected, this, &Usb2Snes::onWebSocketConnected);
    QObject::connect(&m_webSocket, &QWebSocket::errorOccurred, this, &Usb2Snes::onWebSocketError);
    QObject::connect(&m_webSocket, &QWebSocket::disconnected, this, &Usb2Snes::onWebSocketDisconnected);
    QObject::connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &Usb2Snes::onWebSocketBinaryReceived);
    QObject::connect(&timer, &QTimer::timeout, this, &Usb2Snes::onTimerTick);
    requestedBinaryReadSize = 0;
    m_autoAttach = autoAttach;
    metaCommands = QMetaEnum::fromType<Usb2SnesCommand>();
}

void Usb2Snes::usePort(QString port)
{
    m_port = port;
}

QString Usb2Snes::port()
{
    return m_port;
}

void Usb2Snes::connect()
{
    if (m_state == None)
        m_webSocket.open(QUrl(USB2SNESURL));
}

void Usb2Snes::close()
{
    if (m_state != None)
        m_webSocket.close();
}

void Usb2Snes::setAppName(QString name)
{
    sendRequest(Name, QStringList() << name);
}

void Usb2Snes::attach(QString deviceName)
{
    sendRequest(Attach, QStringList() << deviceName);
    changeState(Ready);
}

void Usb2Snes::onWebSocketConnected()
{
    sDebug() << "Websocket connected";
    changeState(Connected);
    emit connected();
    m_istate = IConnected;
    if (m_autoAttach)
    {
        m_istate = DeviceListRequested;
        sendRequest(DeviceList);
    }
}

void Usb2Snes::onWebSocketDisconnected()
{
    sDebug() << "Websocket disconnected";
    changeState(None);
    m_istate = INone;
    lastBinaryMessage = "";
    lastTextMessage = "";
    emit disconnected();
}

QStringList Usb2Snes::getJsonResults(QString json)
{
    QStringList toret;
    QJsonDocument   jdoc = QJsonDocument::fromJson(json.toLatin1());
    if (!jdoc.object()["Results"].toArray().isEmpty())
    {
        QJsonArray jarray = jdoc.object()["Results"].toArray();
        foreach(QVariant entry, jarray.toVariantList())
        {
            toret << entry.toString();
        }
    }
    return toret;
}

void Usb2Snes::onWebSocketTextReceived(QString message)
{
    sDebug() << "istate: " << m_istate;
    sDebug() << "state: " << m_state;
    sDebug() << "Command: " << m_currentCommand;
    sDebug() << "Message: " << message;
    lastTextMessage = message;
    switch (m_istate)
    {
        case DeviceListRequested:
        {
            QStringList results = getJsonResults(message);
            m_deviceList = results;
            if (m_autoAttach)
            {
                if (!results.isEmpty())
                {
                    timer.stop();
                    m_port = results.at(0);
                    sendRequest(Attach, QStringList() << m_port);
                    m_istate = AttachSent;
                    timer.start(200);
                } else {
                    timer.start(1000);
                }
            }
            break;
        }
        case FirmwareVersionRequested:
        {
            QStringList results = getJsonResults(message);
            if (!results.isEmpty())
            {
                m_firmwareString = results.at(0);
                if (m_firmwareString.right(3) == "gsu")
                    m_firmwareVersion = QVersionNumber(7);
                else
                {
                    // usb2snes firmware
                    if (m_firmwareString.contains("usb"))
                    {
                        int npos = m_firmwareString.indexOf("-v");
                        npos += 2;
                        sDebug() << "Firmware is : " << m_firmwareString << "Version" << m_firmwareString.mid(npos);
                        m_firmwareVersion = QVersionNumber(m_firmwareString.mid(npos).toInt());
                    } else { // oficial sd2snes
                        m_firmwareVersion = QVersionNumber(QVersionNumber::fromString(m_firmwareString).minorVersion());
                    }
                }
                sDebug() << "Firmware Version: " << m_firmwareVersion;
                m_istate = ServerVersionRequested;
                sendRequest(AppVersion);
            }
            break;
        }
        case ServerVersionRequested:
        {
            QStringList results = getJsonResults(message);
            if (!results.isEmpty())
            {
                m_serverVersion = QVersionNumber::fromString(results.at(0));
                m_istate = IReady;
                changeState(Ready);
            }
            break;
        }
        default:
            break;
    }
    switch (m_currentCommand)
    {
        case DeviceList: {
            emit deviceListDone(getJsonResults(message));
            break;
        }
        case Info: {
            QStringList results = getJsonResults(message);
            if(results.size() < 4)
            {
                infos();
                return;
            }
            Usb2Snes::DeviceInfo info;
            info.firmwareVersion = results.at(0);
            info.versionString = results.at(1);
            info.romPlaying = results.at(2);
            info.flags = results.mid(3);
            changeState(Ready);
            m_istate = IReady;
            emit infoDone(info);
            break;
        }
        case List: {
            QList<FileInfo> toret;
            QStringList infos = getJsonResults(message);
            for (int i = 0; i < infos.size(); i += 2)
            {
                FileInfo fi;
                fi.dir = infos.at(i) == "0";
                fi.name = infos.at(i + 1);
                toret << fi;
            }
            emit lsDone(toret);
            break;
        }
        case GetFile: {
            QStringList result = getJsonResults(message);
            bool    ok;
            requestedBinaryReadSize = result.at(0).toUInt(&ok, 16);
            sDebug() << "File Size:" << requestedBinaryReadSize;
            emit getFileSizeGet(requestedBinaryReadSize);
            if(m_state != GettingConfig)
                changeState(ReceivingFile);
            break;
        }
        default:
            break;
    }
    emit textMessageReceived();
}

void Usb2Snes::onWebSocketBinaryReceived(QByteArray message)
{
    sDebug() << "Binary Received";
    static QByteArray buffer;
    buffer.append(message);
    if (message.size() < 100)
        sDebug() << "<<B" << message.toHex('-') << message;
    else
        sDebug() << "<<B" << "Received " << message.size() << " byte of data " << buffer.size() << requestedBinaryReadSize;
    if ((unsigned int) buffer.size() == requestedBinaryReadSize)
    {
        lastBinaryMessage = buffer;
        buffer.clear();
        emit binaryMessageReceived();
        switch(m_state)
        {
            case GettingConfig: {
                emit getConfigDataReceived();
                break;
            }
            case ReceivingFile: {
                emit getFileDataReceived();
                break;
            }
            case GettingAddress: {
                emit getAddressDataReceived();
                break;
            }
            case GettingAddresses: {
                emit getAddressesDataReceived();
                break;
            }
            default:
                break;
        }
        sDebug() << "Finish Binary";
        changeState(Ready);
        m_istate = IReady;
    }
}

void Usb2Snes::onWebSocketError(QAbstractSocket::SocketError error)
{
    sDebug() << "Error " << error;
}

void Usb2Snes::onTimerTick()
{
    if (m_istate == AttachSent)
    {
        sendRequest(Info);
        m_istate = FirmwareVersionRequested;
        timer.stop();
    }
    if (m_istate == DeviceListRequested)
    {
        sendRequest(DeviceList);
    }
}

void Usb2Snes::sendRequest(Usb2SnesCommand opCode, QStringList operands, Space space, QStringList flags)
{
    Q_UNUSED(flags)
    QJsonArray      jOp;
    QJsonObject     jObj;

    m_currentCommand = opCode;
    jObj["Opcode"] = metaCommands.valueToKey(opCode);
    if (space == SNES)
        jObj["Space"] = "SNES";
    if (space == CMD)
        jObj["Space"] = "CMD";
    foreach(QString sops, operands)
        jOp.append(QJsonValue(sops));
    if (!operands.isEmpty())
        jObj["Operands"] = jOp;

    QString jsonString = QJsonDocument(jObj).toJson();
    sDebug() << ">>" << jsonString;

    m_webSocket.sendTextMessage(jsonString);
}

void Usb2Snes::changeState(Usb2Snes::State s)
{
    m_state = s;
    sDebug() << "State changed to " << s;
    emit stateChanged();
}

QByteArray Usb2Snes::getAddressSync(unsigned int addr, unsigned int size, Space space)
{
    m_istate = IBusy;
    sendRequest(GetAddress, QStringList() << QString::number(addr, 16) << QString::number(size, 16), space);
    requestedBinaryReadSize = size;
    QEventLoop  loop;
    QObject::connect(this, SIGNAL(binaryMessageReceived()), &loop, SLOT(quit()));
    QObject::connect(this, SIGNAL(disconnected()), &loop, SLOT(quit()));
    loop.exec();
    requestedBinaryReadSize = 0;
    sDebug() << "Getting data,  size : " << lastBinaryMessage.size() << "- MD5 : " << QCryptographicHash::hash(lastBinaryMessage, QCryptographicHash::Md5).toHex();
    m_istate = IReady;
    return lastBinaryMessage;
}

void Usb2Snes::getAddress(unsigned int addr, unsigned int size, Space space)
{
    m_istate = IBusy;
    binaryDataSent = 0;
    requestedBinaryReadSize = size;
    changeState(GettingAddress);
    sendRequest(GetAddress, QStringList() << QString::number(addr, 16) << QString::number(size, 16), space);
}

void Usb2Snes::getAddresses(QList<QPair<int,int>> addresses)
{
    // We can only send 8 addresses at a time
    m_istate = IBusy;
    changeState(GettingAddresses);
    unsigned int total_size = 0;
    QStringList operands;
    for(auto &pair : addresses)
    {
        unsigned int size = pair.second;
        total_size += size;
        operands.append(QString::number(pair.first + 0xF50000, 16));
        operands.append(QString::number(size, 16));
        if (operands.size() == 16)
        {
            sendRequest(GetAddress, operands);
            operands.clear();
        }
    }
    if(operands.isEmpty() == false)
        sendRequest(GetAddress, operands);
    requestedBinaryReadSize = total_size;
}

//To see if any NMI hook patch is applied you could technically look at $002A90 to see if it's $60 (RTS) = no patch.
void Usb2Snes::isPatchedROM()
{
    sDebug() << "Checking for patched rom";
    if (m_firmwareVersion > QVersionNumber(7))
        getAddress(0xFC0000, 4, CMD);
    else
        getAddress(0x2A90, 1, CMD);
}

void Usb2Snes::checkReset()
{
    m_istate = IBusy;
    changeState(CheckingReset);
    sendRequest(GetAddress, QStringList() << QString::number(0x002A00, 16) << QString::number(1, 16), CMD);
}

void Usb2Snes::setAddress(unsigned int addr, QByteArray data, Space space)
{
    m_istate = IBusy;
    sendRequest(PutAddress, QStringList() << QString::number(addr, 16) << QString::number(data.size(), 16), space);
    //Dumb shit for bad win7 C# websocket api
    sDebug() << "Sending data,  size : " << data.size() << "- MD5 : " << QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
    if (data.size() <= 1024)
        m_webSocket.sendBinaryMessage(data);
    else
    {
        while (data.size() != 0)
        {
            m_webSocket.sendBinaryMessage(data.left(1024));
            data.remove(0, 1024);
        }
    }
    m_istate = IReady;
    sDebug() << "Done sending data for setAddress " + QString::number(addr, 16);
}

void Usb2Snes::sendFile(QString path, QByteArray data)
{
    sendRequest(PutFile, QStringList() << path << QString::number(data.size(), 16));
    changeState(SendingFile);
    fileDataToSend = data;
    m_istate = IBusy;
    if (data.size() <= 1024)
        m_webSocket.sendBinaryMessage(data);
    else
    {
        int foo = 0;
        while (data.size() != 0)
        {
            m_webSocket.sendBinaryMessage(data.left(1024));
            data.remove(0, 1024);
            foo += 1024;
            emit fileSendProgress(foo);
        }
    }
    emit fileSent();
    m_istate = IReady;
    changeState(Ready);
}

void Usb2Snes::getConfig()
{
    m_istate = IBusy;
    changeState(GettingConfig);
    sendRequest(GetFile, QStringList() << "/sd2snes/config.yml");
}

void Usb2Snes::getFile(QString path)
{
    sendRequest(GetFile, QStringList() << path);
    m_istate = IBusy;
    binaryDataSent = 0;
}

void Usb2Snes::renameFile(QString oldPath, QString newPath)
{
    sendRequest(Rename, QStringList() << oldPath << newPath);
}

void Usb2Snes::deleteFile(QString fileName)
{
    sendRequest(Remove, QStringList() << fileName);
}

void Usb2Snes::boot(QString path)
{
    sendRequest(Boot, QStringList() << path);
}

void Usb2Snes::mkdir(QString dirPath)
{
    sendRequest(MakeDir, QStringList() << dirPath);
}

void Usb2Snes::reset()
{
    sendRequest(Reset);
}

void Usb2Snes::menu()
{
    sendRequest(Menu);
}

bool Usb2Snes::patchROM(QString patch)
{
    QFile fPatch(patch);
    if (fPatch.open(QIODevice::ReadOnly))
    {
        unsigned int size = fPatch.size();
        sendRequest(PutIPS, QStringList() << "hook" << QString::number(size, 16));
        QByteArray data = fPatch.readAll();
        m_webSocket.sendBinaryMessage(data);
        return true;
    }
    return false;
}

Usb2Snes::State Usb2Snes::state()
{
    return m_state;
}

void Usb2Snes::infos(bool f)
{
    if(f)
        m_istate = FirmwareVersionRequested;
    else
        m_istate = IBusy;
    changeState(GettingInfo);
    sendRequest(Info);
}

int Usb2Snes::fileDataSize() const
{
    return fileDataToSend.size();
}

void Usb2Snes::ls(QString path)
{
    sendRequest(List, QStringList() << path);
}

QString Usb2Snes::firmwareString()
{
    return m_firmwareString;
}

QVersionNumber Usb2Snes::firmwareVersion()
{
    return m_firmwareVersion;
}

void Usb2Snes::deviceList()
{
    sendRequest(DeviceList);
}

QVersionNumber Usb2Snes::serverVersion()
{
    return m_serverVersion;
}

QByteArray Usb2Snes::getBinaryData()
{
    return lastBinaryMessage;
}
