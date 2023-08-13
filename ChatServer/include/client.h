#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QTcpSocket* socket, QObject *parent = nullptr);
    ~Client();
    QTcpSocket* getSocket() const;
    int getPort() const;
    void setName(QString name);
    QString getName() const;
    void setState(bool state);
    bool isOnline();
    bool isInBan();
    void setBan(bool ban);
private:
    QTcpSocket* _socket;
    QString _clientName {};
    bool _isOnline {};
    bool _inBan {};
signals:

};

#endif // CLIENT_H
