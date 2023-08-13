#include "client.h"

Client::Client(QTcpSocket* socket, QObject *parent)
    : QObject{parent}
{
    _socket = socket;
    _isOnline = false;
    _inBan = false;
}

Client::~Client()
{
     _socket->deleteLater();
}

QTcpSocket* Client::getSocket() const
{
    return _socket;
}

int Client::getPort() const
{
    return _socket->peerPort();
}

void Client::setName(QString name)
{
    _clientName = name;
}

QString Client::getName() const
{
    return _clientName;
}

void Client::setState(bool state)
{
    _isOnline = state;
}

bool Client::isOnline()
{
    return _isOnline;
}

bool Client::isInBan()
{
    return _inBan;
}

void Client::setBan(bool ban)
{
    _inBan = ban;
}
