
#include "clientmainwindow.h"
#include "ui_clientmainwindow.h"
#include <QMessageBox>
#include <QTime>
#include <QStringList>
#include <QtSql/QSqlDatabase>

ClientMainWindow::ClientMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientMainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Chat Client");
    ui->ipLineEdit->setText("127.0.0.1");
    ui->portLineEdit->setText("5555");
    ui->messageToAllRadioButton->setEnabled(false);
    ui->privateMessageRadioButton->setEnabled(false);
    ui->privateMessageFromRadioButton->setEnabled(false);
    ui->allPrivateMessagesRadioButton->setEnabled(false);
    ui->privateMessageRadioButton->setChecked(false);
    ui->privateMessageFromRadioButton->setChecked(false);
    ui->loginButton->setEnabled(false);
    ui->registrationButton->setEnabled(false);
    ui->exitUserButton->setEnabled(false);
    ui->messageLabel->setEnabled(false);
    ui->messageLineEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);
    ui->currentUserLabel->setText("<font color=red>Current user:</font>");
    ui->chatWithTextEdit->hide();

    _isOnline = false;

    _socket=new QTcpSocket();

    connect(ui->registrationButton, &QPushButton::clicked, this, &ClientMainWindow::openRegistrationWidget);
    connect(ui->loginButton, &QPushButton::clicked, this, &ClientMainWindow::openLoginWidget);
    connect(_socket,&QTcpSocket::readyRead,this,&ClientMainWindow::readData);
    connect(_socket, &QTcpSocket::disconnected, this, &ClientMainWindow::disconnect);
    connect(ui->closeButton, &QPushButton::clicked, this, &ClientMainWindow::close);
    connect(ui->exitUserButton, &QPushButton::clicked, this, &ClientMainWindow::exitUser);

    auto * const userscb = ui->usersComboBox;
    connect(ui->privateMessageRadioButton, &QRadioButton::toggled, this, [this, userscb](const auto on){
        if(on) {
            userscb->setEnabled(true);
            userscb->setCurrentIndex(-1);
        }
        else {
            userscb->setEnabled(false);
            userscb->setCurrentIndex(-1);
            userscb->setCurrentText("");
            ui->usersStateLabel->setText("");
        }
    });
    connect(userscb, &QComboBox::currentTextChanged, this, [this,userscb](QString login){
        if(userscb->currentIndex() != -1 && ui->privateMessageRadioButton->isChecked()) {
            QString message = "146;" + login;
            _socket->write(message.toUtf8());
        }
    });

    auto* const userscb_private = ui->privateUsersComboBox;
    connect(ui->privateMessageFromRadioButton, &QRadioButton::toggled, this, [this, userscb_private](const auto on){
        if(on) {
            userscb_private->setEnabled(true);
            userscb_private->setCurrentIndex(-1);
            ui->chatWithTextEdit->show();
            ui->chatWithTextEdit->clear();
        }
        else {
            userscb_private->setCurrentIndex(-1);
            userscb_private->setCurrentText("");
            userscb_private->setEnabled(false);
            ui->chatWithTextEdit->hide();
        }
    });
    connect(userscb_private, &QComboBox::currentTextChanged, this, [this,userscb_private](QString login){
        ui->chatWithTextEdit->clear();
        if(userscb_private->currentIndex() != -1 && ui->privateMessageFromRadioButton->isChecked()) {
            QString message = "141;" + login;
            _socket->write(message.toUtf8());
        }
    });
}

ClientMainWindow::~ClientMainWindow()
{
    delete _socket;
    delete ui;
}

QTcpSocket *ClientMainWindow::getSocket()
{
    return _socket;
}

QString ClientMainWindow::getCurrentLogin() const
{
    return _currentLogin;
}

void ClientMainWindow::setCurrentLogin(const QString &newCurrentLogin)
{
    _currentLogin = newCurrentLogin;
}

void ClientMainWindow::enterUserInChat(QString login, QString users)
{
    QStringList lst;
    _isOnline = true;
    setCurrentLogin(login);
    lst = users.split(",");
    ui->usersOnlineListWidget->clear();
    for(auto& l : lst)
        ui->usersOnlineListWidget->addItem(l);
    ui->loginButton->setEnabled(false);
    ui->registrationButton->setEnabled(false);
    ui->exitUserButton->setEnabled(true);
    ui->currentUserLabel->setText(QString("<font color=green>Current user: %1</font>").arg(login));
    ui->messageToAllRadioButton->setEnabled(true);
    ui->messageToAllRadioButton->setChecked(true);
    ui->privateMessageRadioButton->setEnabled(true);
    ui->privateMessageFromRadioButton->setEnabled(true);
    ui->allPrivateMessagesRadioButton->setEnabled(true);
    ui->allPrivateMessagesRadioButton->setChecked(true);
    ui->privateUsersComboBox->setEnabled(false);
    ui->usersComboBox->setEnabled(false);
    ui->messageLabel->setEnabled(true);
    ui->messageLineEdit->setEnabled(true);
    ui->sendButton->setEnabled(true);
    _socket->write("145;");
}

void ClientMainWindow::banUser()
{
    ui->currentUserLabel->setText(QString("<font color=red>Current user: %1 (in ban)</font>").arg(getCurrentLogin()));
    ui->messageToAllRadioButton->setChecked(true);
    ui->messageToAllRadioButton->setEnabled(false);
    ui->privateMessageRadioButton->setEnabled(false);
    ui->allPrivateMessagesRadioButton->setChecked(true);
    ui->allPrivateMessagesRadioButton->setEnabled(false);
    ui->privateMessageFromRadioButton->setEnabled(false);
    ui->privateUsersComboBox->setEnabled(false);
    ui->messageLabel->setEnabled(false);
    ui->messageLineEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);
}

void ClientMainWindow::exitUser()
{
    _isOnline = false;
    setCurrentLogin("");
    ui->allMessagesTextEdit->clear();
    ui->privateMessagesTextEdit->clear();
    ui->usersOnlineListWidget->clear();

    ui->loginButton->setEnabled(true);
    ui->registrationButton->setEnabled(true);
    ui->exitUserButton->setEnabled(false);
    ui->currentUserLabel->setText("<font color=red>Current user: </font>");
    ui->messageToAllRadioButton->setChecked(true);
    ui->messageToAllRadioButton->setEnabled(false);
    ui->privateMessageRadioButton->setEnabled(false);
    ui->allPrivateMessagesRadioButton->setChecked(true);
    ui->allPrivateMessagesRadioButton->setEnabled(false);
    ui->privateMessageFromRadioButton->setEnabled(false);
    ui->privateUsersComboBox->setEnabled(false);
    ui->messageLabel->setEnabled(false);
    ui->messageLineEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);

    _socket->write("200");
}

void ClientMainWindow::addUsersInCombobox(QString str)
{
    QStringList users = str.split(",");
    ui->usersComboBox->clear();
    ui->privateUsersComboBox->clear();
    ui->usersComboBox->addItems(users);
    ui->usersComboBox->setCurrentIndex(-1);
    ui->usersComboBox->setCurrentText("");
    ui->privateUsersComboBox->addItems(users);
    ui->privateUsersComboBox->setCurrentIndex(-1);
    ui->privateUsersComboBox->setCurrentText("");
}

void ClientMainWindow::receiveMessagesBetweenTwoUsers(QString str)
{
    if(ui->privateUsersComboBox->currentIndex() != -1) {
        if(str.isEmpty()) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Private messages");
            msgBox.setText("No messages with selected user");
            msgBox.resize(60,40);
            msgBox.exec();
        }
        else {
            QStringList messages = str.split("***");
            QString user1, user2, text;
            for(const auto& message : messages) {
                user1 = message.section(';',0,0);
                user2 = message.section(';',1,1);
                text = message.section(';',2);
                ui->chatWithTextEdit->append(tr("%1 write to %2:").arg(user1).arg(user2));
                ui->chatWithTextEdit->append(QString("<font color=gray>%1</font>").arg(text));
            }
        }
    }
}

void ClientMainWindow::receiveCommandMessageFromServer(QString cmd)
{
    //QMessageBox msgBox;

    switch(cmd.toInt()) {
    case 1:
    case 2:
        QMessageBox(QMessageBox::Information,
                    QObject::tr("Message"),
                    QObject::tr("Message was send!"),
                    QMessageBox::Ok).exec();
        break;
    case 3:
        QMessageBox(QMessageBox::Information,
                    QObject::tr("Logout user"),
                    QObject::tr("You have logged out of the chat"),
                    QMessageBox::Ok).exec();
        break;
    case 4:
        QMessageBox(QMessageBox::Critical,
                    QObject::tr("Error"),
                    QObject::tr("Error send message"),
                    QMessageBox::Ok).exec();
        break;
    default:
        break;
    }
}

void ClientMainWindow::disconnect()
{
    ui->sendButton->setEnabled(false);
    ui->connectButton->setText("Connect");

    QMessageBox msgBox;
    msgBox.setWindowTitle("CAUTION");
    msgBox.setText("You are dissconnect from server!");
    msgBox.resize(60,30);
    msgBox.exec();

    ui->loginButton->setEnabled(false);
    ui->registrationButton->setEnabled(false);
    ui->exitUserButton->setEnabled(false);
    ui->messageToAllRadioButton->setEnabled(false);
    ui->messageToAllRadioButton->setChecked(true);
    ui->privateMessageRadioButton->setEnabled(false);
    ui->allPrivateMessagesRadioButton->setEnabled(false);
    ui->allPrivateMessagesRadioButton->setChecked(true);
    ui->privateMessageFromRadioButton->setEnabled(false);
    ui->messageLabel->setEnabled(false);
    ui->messageLineEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);
    ui->allMessagesTextEdit->clear();
    ui->privateMessagesTextEdit->clear();
    ui->usersOnlineListWidget->clear();
    ui->currentUserLabel->setText("<font color=red>Current user:</font>");
    ui->ipLineEdit->setEnabled(true);
    ui->portLineEdit->setEnabled(true);
}

void ClientMainWindow::openLoginWidget()
{
    _loginWidget = new LoginWidget(_socket);
    _loginWidget->setWindowModality(Qt::ApplicationModal);
    _loginWidget->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    _loginWidget->show();
}

void ClientMainWindow::openRegistrationWidget()
{
    _registrationWidget = new RegistrationWidget(_socket);
    _registrationWidget->setWindowModality(Qt::ApplicationModal);
    _registrationWidget->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    _registrationWidget->show();
}

void ClientMainWindow::on_connectButton_clicked()
{
    QString ip;
    qint16 port;

    if (ui->connectButton->text() == QString("Connect"))
    {
        ip=ui->ipLineEdit->text();
        port=ui->portLineEdit->text().toInt();

        _socket->abort();
        _socket->connectToHost(ip,port);

        if (!_socket->waitForConnected())
        {

            QMessageBox msgBox;
            msgBox.setWindowTitle("Client");
            msgBox.setText("Runtime Out");
            msgBox.resize(50,30);
            msgBox.exec();
            return;
        }

        QMessageBox msgBox;
        msgBox.setWindowTitle("Client");
        msgBox.setText("Successful Connection");
        msgBox.resize(50,40);
        msgBox.exec();

        ui->connectButton->setText("Disconnect");
        ui->loginButton->setEnabled(true);
        ui->registrationButton->setEnabled(true);
        ui->exitUserButton->setEnabled(false);
        ui->ipLineEdit->setEnabled(false);
        ui->portLineEdit->setEnabled(false);
    }
    else
    {
        _socket->disconnectFromHost();

        ui->connectButton->setText("Connect");
        ui->loginButton->setEnabled(false);
        ui->registrationButton->setEnabled(false);
        ui->exitUserButton->setEnabled(false);
        ui->messageToAllRadioButton->setEnabled(false);
        ui->messageToAllRadioButton->setChecked(true);
        ui->privateMessageRadioButton->setEnabled(false);
        ui->allPrivateMessagesRadioButton->setEnabled(false);
        ui->allPrivateMessagesRadioButton->setChecked(true);
        ui->privateMessageFromRadioButton->setEnabled(false);
        ui->messageLabel->setEnabled(false);
        ui->messageLineEdit->setEnabled(false);
        ui->sendButton->setEnabled(false);
        ui->allMessagesTextEdit->clear();
        ui->privateMessagesTextEdit->clear();
        ui->usersOnlineListWidget->clear();
        ui->currentUserLabel->setText("<font color=red>Current user:</font>");
        ui->ipLineEdit->setEnabled(true);
        ui->portLineEdit->setEnabled(true);
    }
}

void ClientMainWindow::on_sendButton_clicked()
{
    QString message;
    if(ui->messageToAllRadioButton->isChecked()) {
        if (ui->messageLineEdit->text().isEmpty()) {
            QMessageBox msgb;
            msgb.setText("Can't Send Empty Message");   // Cant send Empty Message
            msgb.resize(60, 40);
            msgb.exec();
            return;
        }

        ui->allMessagesTextEdit->append(tr("<font color=darkSlateBlue>%1 From Me:</font>").arg(QTime::currentTime().toString()));
        ui->allMessagesTextEdit->append(QString("<font color=grey>%1</font>").arg(ui->messageLineEdit->text()));
        message = "130;" + ui->messageLineEdit->text();
        _socket->write(message.toUtf8());
        _socket->flush();
        ui->messageLineEdit->clear();
        ui->messageLineEdit->setFocus();
    }
    if(ui->privateMessageRadioButton->isChecked()) {
        QString reciever = ui->usersComboBox->currentText();
        if(ui->messageLineEdit->text().isEmpty()) {
            QMessageBox msgb;
            msgb.setText("Can't Send Empty Message");   // Cant send Empty Message
            msgb.resize(60, 40);
            msgb.exec();
            return;
        }
        if(ui->usersComboBox->currentIndex() == -1) {
            QMessageBox msgb;
            msgb.setText("You need to select a user!");
            msgb.resize(60, 40);
            msgb.exec();
            return;
        }
        if(ui->privateMessageFromRadioButton->isChecked() && ui->privateUsersComboBox->currentText() == reciever) {
            ui->chatWithTextEdit->append(tr("%1 write to %2:").arg(getCurrentLogin()).arg(reciever));
            ui->chatWithTextEdit->append(QString("<font color=gray>%1</font>").arg(ui->messageLineEdit->text()));
        }
        message = "140;" + reciever + ";" + ui->messageLineEdit->text();
        _socket->write(message.toUtf8());
        _socket->flush();
        ui->messageLineEdit->clear();
        ui->messageLineEdit->setFocus();
    }
}

/* COMMANDS FROM SERVER
100 - new user join to chat
111 - registration (user not addet in database)
112 - registration (user add in database)
113 - registration (user is already exists)
121 - login (Authentication success)
122 - login (Authentication failed)
123 - login (user already online)
131 - send message to all online clients
141 - private message
142 - receive messages between two users
146 - list of users add in combobox
147-148 - user's state for private message
200 - user left chat
300 - current user in ban
301 - user in ban
302 - current user unban
303 - user unban
500 - command messages*/

void ClientMainWindow::readData()
{
    QByteArray message;
    message = _socket->readAll();
    QString str = QString(message);
    QString cmd = str.section(';',0,0);
    int ind;
    QListWidgetItem* currentItem;

    switch (cmd.toInt()) {
    case 100:
        ui->allMessagesTextEdit->append(tr("<font color=green>%1 %2 join to Chat!</font>").arg(QTime::currentTime().toString()).arg(str.section(';',1)));
        ui->usersOnlineListWidget->addItem(str.section(';',1));
        ind = ui->usersComboBox->findText(str.section(';',1), Qt::MatchExactly);
        if(ind == -1) {
            ui->usersComboBox->addItem(str.section(';',1));
        }
        ind = ui->privateUsersComboBox->findText(str.section(';',1), Qt::MatchExactly);
        if(ind == -1) {
            ui->privateUsersComboBox->addItem(str.section(';',1));
        }
        break;
    case 111:
        _registrationWidget->registrationResult(111);
        break;
    case 112:
        _registrationWidget->registrationResult(112);
        enterUserInChat(str.section(';',1,1),str.section(';',2));
        break;
    case 113:
        _registrationWidget->registrationResult(113);
        break;
    case 121:
        _loginWidget->loginResult(121);
        enterUserInChat(str.section(';',1,1),str.section(';',2));
        break;
    case 122:
        _loginWidget->loginResult(122);
        break;
    case 123:
        _loginWidget->loginResult(123);
        break;
    case 131:
        ui->allMessagesTextEdit->append(tr("%1 %2 write:").arg(QTime::currentTime().toString()).arg(str.section(';',1,1)));
        ui->allMessagesTextEdit->append(QString("<font color=grey>%1</font>").arg(str.section(';',2)));
        break;
    case 141:
        if(ui->privateMessageFromRadioButton->isChecked() && ui->privateUsersComboBox->currentText() == str.section(";",1,1)) {
            ui->chatWithTextEdit->append(tr("%1 write to %2:").arg(str.section(";",1,1)).arg(getCurrentLogin()));
            ui->chatWithTextEdit->append(QString("<font color=gray>%1</font>").arg(str.section(';',2)));
        }
            ui->privateMessagesTextEdit->append(tr("%1 Message from %2:").arg(QTime::currentTime().toString()).arg(str.section(";",1,1)));
            ui->privateMessagesTextEdit->append(QString("<font color=grey>%1</font>").arg(str.section(';',2)));
        break;
    case 142:
        receiveMessagesBetweenTwoUsers(str.section(';',1));
        break;
    case 146:
        addUsersInCombobox(str.section(';',1));
        break;
    case 147:
        if(ui->usersComboBox->currentIndex() != -1)
            ui->usersStateLabel->setText("<font color=green>User online</font>");
        break;
    case 148:
        if(ui->usersComboBox->currentIndex() != -1)
            ui->usersStateLabel->setText("<font color=red>User offline</font>");
        break;
    case 200:
        ui->allMessagesTextEdit->append(QString("<font color=red>%1 %2 left the Chat!</font>").arg(QTime::currentTime().toString()).arg(str.section(';',1)));
        currentItem = ui->usersOnlineListWidget->findItems(str.section(';',1), Qt::MatchExactly)[0];
        delete ui->usersOnlineListWidget->takeItem(ui->usersOnlineListWidget->row(currentItem));
        break;
    case 300:
        ui->allMessagesTextEdit->append(QString("<font color=red>%1 You are in BAN!</font>").arg(QTime::currentTime().toString()));
        banUser();
        break;
    case 301:
        ui->allMessagesTextEdit->append(QString("<font color=red>%1 User with login %2 sent to ban!</font>").arg(QTime::currentTime().toString()).arg(str.section(';',1)));
        break;
    case 302:
        ui->allMessagesTextEdit->append(QString("<font color=green>%1 You are UNBAN!</font>").arg(QTime::currentTime().toString()));
        enterUserInChat(str.section(';',1,1),str.section(';',2));
        break;
    case 303:
        ui->allMessagesTextEdit->append(QString("<font color=green>%1 User with login %2 Unban!</font>").arg(QTime::currentTime().toString()).arg(str.section(';',1)));
        break;
    case 500:
        receiveCommandMessageFromServer(str.section(';',1));
        break;
    default:
        break;
    }
}



