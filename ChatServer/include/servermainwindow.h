
#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QMap>

#include "client.h"
#include "sqlitedatabase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ServerMainWindow; }
QT_END_NAMESPACE

class ServerMainWindow : public QMainWindow

{
    Q_OBJECT

public:
    ServerMainWindow(QWidget *parent = nullptr);
    ~ServerMainWindow();

    Client* getClientBySocket(QTcpSocket* socket);
    void removeClient(QTcpSocket* socket);
    void userIsOnline(QTcpSocket* socket, QString login);
    void sendMessageToAllButOne(QByteArray message, QTcpSocket* socket);
    void sendMessageToOne(QByteArray message, QTcpSocket* socket);
    void registration(QTcpSocket* socket, QString name, QString login, QString password);
    void login(QTcpSocket* socket, QString login, QString password);
    void logOut(QTcpSocket* socket);
    void sendMessageToAll(QTcpSocket* socket, QString text);
    void sendPrivateMessage(QTcpSocket* socket, QString reciever, QString text);
    void sendListOfUsers(QTcpSocket* socket);
    void getChatBetweenTwoUsers(QTcpSocket* socket, QString user2);

private slots:
    void newConnect();
    void disconnect();
    void readData();
    void on_connectButton_clicked();
    void on_showChatPushButton_clicked();
    void on_disconnectUserPushButton_clicked();
    void on_banUserPushButton_clicked();

private:
    Ui::ServerMainWindow* ui;
    QTcpServer* _server;
    QVector<Client*> _allClients{};
    Client* _client;
    int _clientsCount {};
    int _onlineUsers {};
    int _allUsers {};

    SQLiteDataBase* _database;
};

#endif // SERVERMAINWINDOW_H
