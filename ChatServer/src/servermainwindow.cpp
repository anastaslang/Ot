#include "servermainwindow.h"
#include "ui_servermainwindow.h"
#include <QTime>
#include <QMessageBox>
#include <QStringList>
#include <QFont>
#include <algorithm>

ServerMainWindow::ServerMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerMainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Chat Server");
    ui->portLineEdit->setText("5555");

    _server = new QTcpServer();
    _server->setMaxPendingConnections(10);
    ui->clientCountLineEdit->setText(QString::number(_allClients.size()));
    _onlineUsers = 0;
    ui->authUsersLineEdit->setText(QString::number(_onlineUsers));
    _allUsers = 0;
    ui->allUsersLineEdit->setText(QString::number(_allUsers));
    ui->allUsersRadioButton->setEnabled(false);
    ui->usersOnlineRadioButton->setEnabled(false);
    ui->inBanRadioButton->setEnabled(false);
    ui->banUserPushButton->setEnabled(false);
    ui->disconnectUserPushButton->setEnabled(false);
    ui->user1ComboBox->setEnabled(false);
    ui->user2ComboBox->setEnabled(false);
    ui->showChatPushButton->setEnabled(false);

    // работа с базой данных
    _database = new SQLiteDataBase();
    if(_database->openDatabase())
    {
        ui->informationFromClientTextEdit->append(tr("<font color=darkGreen>%1 The Connection to the database was successful!</font>").arg(QTime::currentTime().toString()));
        if(_database->createTables()) {
            ui->informationFromClientTextEdit->append(tr("<font color=darkGreen>%1 Database Tables successfully found or created</font>").arg(QTime::currentTime().toString()));
        }
        else {
            ui->connectButton->setEnabled(false);
            ui->informationFromClientTextEdit->append(tr("<font color=darkRed>%1 The search or creation of database tables failed</font>").arg(QTime::currentTime().toString()));
        }
    }
    else {
        QMessageBox(QMessageBox::Information,
                    QObject::tr("Error"),
                    QObject::tr("Connection with database failed!"),
                    QMessageBox::Ok).exec();
        ui->connectButton->setEnabled(false);
        ui->informationFromClientTextEdit->append(tr("<font color=darkRed>%1 The Connection to the database failed!</font>").arg(QTime::currentTime().toString()));
    }

    connect(_server,&QTcpServer::newConnection,this,&ServerMainWindow::newConnect);
    connect(ui->closeButton, &QPushButton::clicked, this, &ServerMainWindow::close);

    connect(ui->allUsersRadioButton, &QRadioButton::toggled, this, [this](const auto on){
        if(on) {
            QVector<QString> users = _database->getAllUsers();
            for(auto& user : users) {
                ui->usersListWidget->addItem(user);
            }
            ui->banUserPushButton->setEnabled(false);
            ui->disconnectUserPushButton->setEnabled(false);
            ui->banUserPushButton->setText("Ban user");
        }
        else {
            ui->usersListWidget->clear();
        }
    });

    connect(ui->inBanRadioButton, &QRadioButton::toggled, this, [this](const auto on){
        if(on) {
            for(Client* user : _allClients) {
                if(user->isInBan())
                    ui->usersListWidget->addItem(user->getName());
            }
            ui->banUserPushButton->setEnabled(false);
            ui->disconnectUserPushButton->setEnabled(false);
        }
        else {
            ui->usersListWidget->clear();
        }
    });

    connect(ui->usersListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item){
        if(ui->usersOnlineRadioButton->isChecked()) {
            ui->banUserPushButton->setEnabled(true);
            ui->disconnectUserPushButton->setEnabled(true);
            for(Client* user : _allClients) {
                if((user->getName() == item->text()) && user->isInBan()) {
                    ui->banUserPushButton->setText("Unban user");
                    break;
                }
                else {
                    ui->banUserPushButton->setText("Ban user");
                }
            }
        }
        else if(ui->inBanRadioButton->isChecked()) {
            ui->banUserPushButton->setEnabled(true);
            ui->disconnectUserPushButton->setEnabled(false);
            ui->banUserPushButton->setText("Unban user");
        }
        else {
            ui->banUserPushButton->setEnabled(false);
            ui->disconnectUserPushButton->setEnabled(false);
        }
    });

    connect(ui->usersOnlineRadioButton, &QRadioButton::toggled, this, [this](const auto on){
        if(on) {
            for(Client* user : _allClients) {
                if(user->isOnline())
                    ui->usersListWidget->addItem(user->getName());
            }
        }
        else {
            ui->usersListWidget->clear();
        }
    });
}

ServerMainWindow::~ServerMainWindow()
{
    _server->close();
    _server->deleteLater();
    delete ui;
}

Client* ServerMainWindow::getClientBySocket(QTcpSocket* socket)
{
    for(Client* client : _allClients) {
        QTcpSocket* clientSocket = client->getSocket();
        if(socket == clientSocket) {
            return client;
        }
    }
    return nullptr;
}

void ServerMainWindow::removeClient(QTcpSocket *socket)
{
    _allClients.remove(_allClients.indexOf(getClientBySocket(socket)));
}

void ServerMainWindow::userIsOnline(QTcpSocket* socket, QString login)
{
    if(!login.isEmpty()) {
        for(Client* user : _allClients) {
            if((user->getName() == login) && user->isOnline()) {
                socket->write("147;");
            }
        }
        socket->write("148;");
    }
}

void ServerMainWindow::newConnect()
{
    QTcpSocket* clientSocket = _server->nextPendingConnection();
    _client = new Client(clientSocket);
    int port = _client->getPort();

    connect(clientSocket, &QTcpSocket::readyRead, this, &ServerMainWindow::readData);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ServerMainWindow::disconnect);

    _allClients.push_back(_client);
    ui->clientCountLineEdit->setText(QString::number(_allClients.size()));
    ui->informationFromClientTextEdit->append(tr("<font color=darkGreen>%1 New client connect from port number %2</font>")
                                                  .arg(QTime::currentTime().toString())
                                                  .arg(QString::number(port)));
}

void ServerMainWindow::disconnect()
{
    QTcpSocket* currentSocket = qobject_cast<QTcpSocket*>(sender());
    for(Client* client : _allClients) {
        QTcpSocket* clientSocket = client->getSocket();
        if(currentSocket == clientSocket) {
            if(!client->getName().isEmpty()) {
                QListWidgetItem* currentItem = ui->usersListWidget->findItems(client->getName(), Qt::MatchExactly)[0];
                if(client->isOnline() && ui->usersOnlineRadioButton->isChecked()) {
                    delete ui->usersListWidget->takeItem(ui->usersListWidget->row(currentItem));
                }
                if(client->isInBan() && ui->inBanRadioButton->isChecked()) {
                    delete ui->usersListWidget->takeItem(ui->usersListWidget->row(currentItem));
                }
                ui->authUsersLineEdit->setText(QString::number(--_onlineUsers));
                QString message = "200;" + getClientBySocket(currentSocket)->getName();
                sendMessageToAllButOne(message.toUtf8(), currentSocket);
            }
            removeClient(currentSocket);
            currentSocket->deleteLater();
            ui->clientCountLineEdit->setText(QString::number(_allClients.size()));
            ui->informationFromClientTextEdit->append(tr("<font color=darkRed>%1 Client from port number %2 disconnect</font>")
                                                          .arg(QTime::currentTime().toString())
                                                          .arg(client->getPort()));
            break;
        }
    }
}

/* COMMANDS FROM CLIENTS
110 - registration
120 - login
130 - message to all online clients
140 - send private message
141 - get chat between two users
145 - send list of users
146 - user is online or not
200 - logout user*/

void ServerMainWindow::readData()
{
    QByteArray message;
    QTcpSocket* currentSocket = qobject_cast<QTcpSocket*>(sender());
    message =  currentSocket->readAll();
    QString str = QString(message);
    QString cmd = str.section(';',0,0);

    switch (cmd.toInt()) {
    case 110:
        ui->informationFromClientTextEdit->append(tr("%1 Requested command from client: Registration").arg(QTime::currentTime().toString()));
        registration(currentSocket, str.section(';',1,1), str.section(';',2,2), str.section(';',3,3));
        break;
    case 120:
        ui->informationFromClientTextEdit->append(tr("%1 Requested command from client: Login").arg(QTime::currentTime().toString()));
        login(currentSocket, str.section(';',1,1), str.section(';',2,2));
        break;
    case 130:
        ui->informationFromClientTextEdit->append(tr("%1 Requested command from client: Send message to all").arg(QTime::currentTime().toString()));
        ui->allMessagesTextEdit->append(tr("%1 %2 write:").arg(QTime::currentTime().toString()).arg(getClientBySocket(currentSocket)->getName()));
        ui->allMessagesTextEdit->append(QString("<font color=dimGray>%1</font>").arg(str.section(';',1)));
        sendMessageToAll(currentSocket, str.section(';',1));
        break;
    case 140:
        ui->informationFromClientTextEdit->append(tr("%1 Requested command from client: Send private message").arg(QTime::currentTime().toString()));
        sendPrivateMessage(currentSocket, str.section(';',1,1), str.section(';',2));
        break;
    case 141:
        ui->informationFromClientTextEdit->append(tr("%1 Requested command from client: Get chat between two users").arg(QTime::currentTime().toString()));
        getChatBetweenTwoUsers(currentSocket,str.section(';',1));
        break;
    case 145:
        ui->informationFromClientTextEdit->append(tr("%1 Requested command from client: Send list of users").arg(QTime::currentTime().toString()));
        sendListOfUsers(currentSocket);
        break;
    case 146:
        ui->informationFromClientTextEdit->append(tr("%1 Requested command from client: User is online or not").arg(QTime::currentTime().toString()));
        userIsOnline(currentSocket,str.section(';',1));
        break;
    case 200:
        ui->informationFromClientTextEdit->append(tr("%1 Requested command from client: Logout").arg(QTime::currentTime().toString()));
        logOut(currentSocket);
        break;
    default:
        break;
    }
}

void ServerMainWindow::sendMessageToAllButOne(QByteArray message, QTcpSocket* socket)
{
    for (int i = 0; i < _allClients.size(); ++i) {
        QTcpSocket* clientSocket = _allClients.at(i)->getSocket();
        if(socket != clientSocket && _allClients.at(i)->isOnline()) {
            clientSocket->write(message);
        }
    }
}

void ServerMainWindow::sendMessageToOne(QByteArray message, QTcpSocket *socket)
{
    for (int i = 0; i < _allClients.size(); ++i) {
        QTcpSocket* clientSocket = _allClients.at(i)->getSocket();
        if(socket == clientSocket) {
            clientSocket->write(message);
        }
    }
}

void ServerMainWindow::registration(QTcpSocket* socket, QString name, QString login, QString password)
{
    int res = _database->addUser(name, login, password);
    QString message;
    switch (res) {
    case 0:
        message = QTime::currentTime().toString() + " User with login " + login + " not added in database! Registration failed...";
        ui->informationFromClientTextEdit->append(message);
        socket->write("111");
        break;
    case 1:
        message = QTime::currentTime().toString() + " User with login " + login + " add in database! Registration success...";
        getClientBySocket(socket)->setState(true);
        getClientBySocket(socket)->setName(login);
        _onlineUsers++;
        ui->authUsersLineEdit->setText(QString::number(_onlineUsers));
        ui->allUsersLineEdit->setText(QString::number((ui->allUsersLineEdit->text().toInt())+1));
        ui->informationFromClientTextEdit->append(message);

        if(ui->allUsersRadioButton->isChecked() || ui->usersOnlineRadioButton->isChecked())
            ui->usersListWidget->addItem(login);

        ui->user1ComboBox->addItem(login);
        ui->user2ComboBox->addItem(login);

        message = "112;" + login + ";";
        for(Client* user : _allClients) {
            if(user->isOnline())
                message += user->getName() + ",";
        }
        message.resize(message.size()-1);
        socket->write(message.toUtf8());

        message = "100;" + login;
        sendMessageToAllButOne(message.toUtf8(), socket);
        break;
    case 2:
        message = QTime::currentTime().toString() + " User with login " + login + " is already exists";
        ui->informationFromClientTextEdit->append(message);
        socket->write("113");
        break;
    default:
        break;
    }
}

void ServerMainWindow::login(QTcpSocket *socket, QString login, QString password)
{
    bool res = _database->checkUserByLoginAndPassword(login, password);
    bool isAlreadyOnline = false;
    for(Client* user : _allClients) {
        if((user->getName() == login) && user->isOnline()) {
            isAlreadyOnline = true;
        }
    }
    QString message;
    if(res && !isAlreadyOnline) {
        message = QTime::currentTime().toString() + " The user with login " + login + " was authenticated";
        getClientBySocket(socket)->setState(true);
        getClientBySocket(socket)->setName(login);
        _onlineUsers++;
        ui->authUsersLineEdit->setText(QString::number(_onlineUsers));
        ui->informationFromClientTextEdit->append(message);
        if(ui->usersOnlineRadioButton->isChecked())
            ui->usersListWidget->addItem(login);

        message = "121;" + login + ";";
        for(Client* user : _allClients) {
            if(user->isOnline())
                message += user->getName() + ",";
        }
        message.resize(message.size()-1);
        socket->write(message.toUtf8());

        message = "100;" + login;
        sendMessageToAllButOne(message.toUtf8(), socket);
    }
    else if(isAlreadyOnline) {
        message = QTime::currentTime().toString() + " The user with login " + login + " is already online";
        ui->informationFromClientTextEdit->append(message);
        socket->write("123;");
    }
    else {
        message = QTime::currentTime().toString() + " The user with login " + login + " was not authenticated";
        ui->informationFromClientTextEdit->append(message);
        socket->write("122");
    }
}

void ServerMainWindow::logOut(QTcpSocket *socket)
{
    ui->informationFromClientTextEdit->append(tr("<font color=darkRed>%1 The user with login %2 logged out the chat</font>").arg(QTime::currentTime().toString()).arg(getClientBySocket(socket)->getName()));
    ui->authUsersLineEdit->setText(QString::number(--_onlineUsers));
    if(ui->usersOnlineRadioButton->isChecked()) {
        QListWidgetItem* currentItem = ui->usersListWidget->findItems(getClientBySocket(socket)->getName(), Qt::MatchExactly)[0];
        delete ui->usersListWidget->takeItem(ui->usersListWidget->row(currentItem));
    }
    QString message = "200;" + getClientBySocket(socket)->getName();
    sendMessageToAllButOne(message.toUtf8(), socket);
    socket->write("500;3");
    getClientBySocket(socket)->setName("");
    getClientBySocket(socket)->setState(false);
}

void ServerMainWindow::sendMessageToAll(QTcpSocket *socket, QString text)
{
    QString message;
    bool res = _database->addMessageToAll(getClientBySocket(socket)->getName(), text);
    if(res) {
        for (int i = 0; i < _allClients.size(); ++i) {
            QTcpSocket* onlineClientSocket = _allClients.at(i)->getSocket();
            if(socket != onlineClientSocket && _allClients.at(i)->isOnline()) {
                message = "131;" + getClientBySocket(socket)->getName() + ";" + text;
                onlineClientSocket->write(message.toUtf8());
            }
        }
        socket->write("500;1");
    }
    else {
        socket->write("500;4");
    }
}

void ServerMainWindow::sendPrivateMessage(QTcpSocket *socket, QString reciever, QString text)
{
    QString message;
    bool res = _database->addPrivateMessage(getClientBySocket(socket)->getName(), reciever, text);
    if(res) {
        for (Client* client : _allClients) {
            if(client->getName() == reciever) {
                message = "141;" + getClientBySocket(socket)->getName() + ";" + text;
                client->getSocket()->write(message.toUtf8());
            }
        }
        socket->write("500;2");
    }
    else {
        socket->write("500;4");
    }
}

void ServerMainWindow::sendListOfUsers(QTcpSocket *socket)
{
    QVector<QString> users = _database->getAllUsers();
    QString str;
    for (auto& user : users) {
        if(user != getClientBySocket(socket)->getName())
            str += user + ",";
    }
    str.resize(str.size()-1);
    QString message = "146;" + str;
    socket->write(message.toUtf8());
}

void ServerMainWindow::getChatBetweenTwoUsers(QTcpSocket *socket, QString user2)
{
    QVector<QString> messages = _database->getMessagesBetweenTwoUsers(getClientBySocket(socket)->getName(),user2);
    QString str;
    if(messages.size()>0) {
        for(auto& mes : messages) {
            str += mes + "***";
        }
        str.resize(str.size()-3);
        QString message = "142;" + str;
        socket->write(message.toUtf8());
    }
    else
        socket->write("142;");
}

void ServerMainWindow::on_connectButton_clicked()
{
    if(ui->connectButton->text() == QString("Connect")) {
        qint16 port = ui->portLineEdit->text().toInt();

        if(!_server->listen(QHostAddress::Any, port))
        {
            QMessageBox(QMessageBox::Critical,
                        QObject::tr("Error"),
                        _server->errorString(),
                        QMessageBox::Ok).exec();
            return;
        }
        ui->connectButton->setText("Disconnect");
        ui->portLineEdit->setEnabled(false);
        ui->allUsersRadioButton->setEnabled(true);
        ui->usersOnlineRadioButton->setEnabled(true);
        ui->inBanRadioButton->setEnabled(true);
        ui->user1ComboBox->setEnabled(true);
        ui->user2ComboBox->setEnabled(true);
        ui->showChatPushButton->setEnabled(true);
        ui->allUsersLineEdit->setText(QString::number(_database->getAllUsers().size()));
        QVector<QString> users = _database->getAllUsers();
        for(auto& user : users) {
            ui->usersListWidget->addItem(user);
            ui->user1ComboBox->addItem(user);
            ui->user2ComboBox->addItem(user);
        }
        ui->user1ComboBox->setCurrentIndex(-1);
        ui->user2ComboBox->setCurrentIndex(-1);

        QVector<QString> messages = _database->get10MessagesToAll();
        QString user1, text;
        for(const auto& message : messages) {
            user1 = message.section(';',0,0);
            text = message.section(';',1);
            ui->allMessagesTextEdit->append(tr("%1 write:").arg(user1));
            ui->allMessagesTextEdit->append(QString("<font color=gray>%1</font>").arg(text));
        }
    }
    else {
        for(Client* client : _allClients) {
            if(client->getSocket()->state() == QAbstractSocket::ConnectedState)
            {
                client->getSocket()->disconnectFromHost();
            }
        }
        _server->close();
        ui->connectButton->setText("Connect");
        ui->portLineEdit->setEnabled(true);
        ui->allUsersRadioButton->setEnabled(false);
        ui->usersOnlineRadioButton->setEnabled(false);
        ui->user1ComboBox->setEnabled(false);
        ui->user2ComboBox->setEnabled(false);
        ui->showChatPushButton->setEnabled(false);
        ui->allUsersRadioButton->setChecked(true);
        ui->banUserPushButton->setEnabled(false);
        ui->disconnectUserPushButton->setEnabled(false);
        ui->user1ComboBox->clear();
        ui->user2ComboBox->clear();
        ui->usersListWidget->clear();
        ui->allMessagesTextEdit->clear();
        ui->privateMessagesTextEdit->clear();
        ui->informationFromClientTextEdit->clear();
    }
}

void ServerMainWindow::on_showChatPushButton_clicked()
{
    ui->privateMessagesTextEdit->clear();
    QString sender, reciever, text;
    QString user1 = ui->user1ComboBox->currentText();
    QString user2 = ui->user2ComboBox->currentText();
    QVector<QString> messages = _database->getMessagesBetweenTwoUsers(user1,user2);
    if(messages.isEmpty()) {
        ui->privateMessagesTextEdit->append("No messages between users!");
    }
    else {
        for(const auto& message : messages) {
            sender = message.section(';',0,0);
            reciever = message.section(';',1,1);
            text = message.section(';',2);
            ui->privateMessagesTextEdit->append(tr("%1 write to %2:").arg(sender).arg(reciever));
            ui->privateMessagesTextEdit->append(QString("<font color=gray>%1</font>").arg(text));
        }
    }
}

void ServerMainWindow::on_disconnectUserPushButton_clicked()
{
    QListWidgetItem* item = ui->usersListWidget->currentItem();
    QString user = item->text();
    for(Client* client : _allClients) {
        if((client->getName() == user) && client->isOnline()) {
            QString message = "200;" + client->getName();
            sendMessageToAllButOne(message.toUtf8(), client->getSocket());
            client->getSocket()->deleteLater();
            removeClient(client->getSocket());
            QListWidgetItem* currentItem = ui->usersListWidget->findItems(user, Qt::MatchExactly)[0];
            delete ui->usersListWidget->takeItem(ui->usersListWidget->row(currentItem));
            ui->authUsersLineEdit->setText(QString::number(--_onlineUsers));
            ui->clientCountLineEdit->setText(QString::number(_allClients.size()));
            ui->informationFromClientTextEdit->append(tr("<font color=darkRed>%1 Client from port number %2 disconnect</font>")
                                                          .arg(QTime::currentTime().toString())
                                                          .arg(client->getPort()));
            break;
        }
    }
}

void ServerMainWindow::on_banUserPushButton_clicked()
{
    QListWidgetItem* item = ui->usersListWidget->currentItem();
    QString user = item->text();
    for(Client* client : _allClients) {
        if(client->getName() == user) {
            if(ui->banUserPushButton->text() == QString("Ban user")) {
                ui->banUserPushButton->setText("Unban user");
                client->setBan(true);
                QString message = "300;";
                sendMessageToOne(message.toUtf8(), client->getSocket());

                message = "301;" + client->getName();
                sendMessageToAllButOne(message.toUtf8(), client->getSocket());

                ui->informationFromClientTextEdit->append(tr("<font color=darkRed>%1 Client with login %2 sent to ban</font>")
                                                              .arg(QTime::currentTime().toString())
                                                              .arg(client->getName()));
                break;
            }
            else {
                ui->banUserPushButton->setText("Ban user");
                client->setBan(false);
                QString message;
                ui->informationFromClientTextEdit->append(tr("<font color=darkGreen>%1 Client with login %2 unban</font>")
                                                              .arg(QTime::currentTime().toString())
                                                              .arg(client->getName()));
                message = "302;" + client->getName() + ";";
                for(Client* user1 : _allClients) {
                    if(user1->isOnline())
                        message += user1->getName() + ",";
                }
                message.resize(message.size()-1);
                client->getSocket()->write(message.toUtf8());

                message = "303;" + client->getName();
                sendMessageToAllButOne(message.toUtf8(), client->getSocket());
                if(ui->inBanRadioButton->isChecked()) {
                    QListWidgetItem* currentItem = ui->usersListWidget->findItems(user, Qt::MatchExactly)[0];
                    delete ui->usersListWidget->takeItem(ui->usersListWidget->row(currentItem));
                }
            }
        }
    }
}

