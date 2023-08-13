#include "sqlitedatabase.h"

#include <QMessageBox>

SQLiteDataBase::SQLiteDataBase(QObject *parent)
    : QObject{parent}
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    QString _path_to_database = "D:/ChatQt/ChatServer/db/_chatdb.db";
    _db.setDatabaseName(_path_to_database);
}

bool SQLiteDataBase::openDatabase()
{

    if(!_db.open())
        return false;
    else
        return true;
}

bool SQLiteDataBase::createTables()
{
    QSqlQuery query;
    bool result;
    int res = 0;

    QString str =   "create table if not exists Users"
                    "(id integer primary key autoincrement, "
                    "name varchar(255) not null, "
                    "login varchar(255) not null, "
                    "password varchar(255) not null);";
    result = query.exec(str);
    if(!result) {
        qInfo() << "Table Users is not created or not finded!";
    }
    else {
        res++;
        qInfo() << "Table Users is created or finded!";
    }

    str = "create table if not exists MessagesPrivate"
          "(id integer primary key autoincrement,"
          "senderId int references Users(id), "
          "recieverId int references Users(id), "
          "text varchar(1024));";
    result = query.exec(str);
    if(!result) {
        qInfo() << "Table MessagesPrivate is not created or not finded!";
    }
    else {
        res++;
        qInfo() << "Table MessagesPrivate is created or finded!";
    }

    str =   "create table if not exists MessagesToAll"
            "(id integer primary key autoincrement,"
            "senderId integer references Users(id), "
            "text varchar(1024));";
    result = query.exec(str);
    if(!result) {
        qInfo() << "Table MessagesToAll is not created or not finded!";
    }
    else {
        res++;
        qInfo() << "Table MessagesToAll is created or finded!";
    }
    return (res == 3);
}

bool SQLiteDataBase::checkUserByLogin(QString login)
{
    QSqlQuery query;
    QString str = "select id from Users where login=\'" + login + "\'";
    if(query.exec(str)) {
        if(query.next())
            return true;
    }
    return false;
}

QString SQLiteDataBase::getUserById(QString id)
{
    QSqlQuery query;
    QString str = "select login from Users where id=\'" + id + "\'";
    if(query.exec(str)) {
        if(query.next())
            return query.value(0).toString();
    }
    return "";
}

QVector<QString> SQLiteDataBase::get10MessagesToAll()
{
    QVector<QString> messages {};
    QString message {};
    QSqlQuery query;
    QString str = "select id,senderId,text from (select id,senderId,text from MessagesToAll order by id desc limit 10) order by id asc";

    if(query.exec(str)) {
        while(query.next()) {
            message = getUserById(query.value(1).toString()) + ";" + query.value(2).toString();
            messages.push_back(message);
        }
    }
    return messages;
}

int SQLiteDataBase::getUserId(QString login)
{
    QSqlQuery query;
    QString str = "select id from Users where login=\'" + login + "\'";
    if(query.exec(str)) {
        if(query.next())
            return query.value(0).toInt();
    }
    return -1;
}

QVector<QString> SQLiteDataBase::getAllUsers()
{
    QSqlQuery query;
    QVector<QString> users;
    QString str = "select login from Users";
    if(query.exec(str)) {
        while(query.next()) {
            users.push_back(query.value(0).toString());
        }
    }
    return users;
}

int SQLiteDataBase::addUser(QString name, QString login, QString password)
{
    if(!checkUserByLogin(login)) {
        QSqlQuery query;
        query.prepare("insert into Users(name, login, password) "
                      "values(:name, :login, :password);");
        query.bindValue(":name", name);
        query.bindValue(":login", login);
        query.bindValue(":password", password);
        if(query.exec()) {
            qInfo() << "User with login " << login << " added in database!" ;
            return 1;
        }
        else {
            qInfo() << "User with login " << login << " not added in database!" ;
            return 0;
        }
    }
    else {
        qInfo() << "User with login " << login << " is already exists!" ;
        return 2;
    }
}

bool SQLiteDataBase::checkUserByLoginAndPassword(QString login, QString password)
{
    QSqlQuery query;
    QString str = "select login, password from Users where login=\'" + login + "\' and password=\'" + password + "\'";
    if(query.exec(str)) {
        if(query.next())
            return true;

    }
    return false;
}

bool SQLiteDataBase::addMessageToAll(QString sender, QString text)
{
    QSqlQuery query;
    query.prepare("insert into MessagesToAll(senderId, text) "
                  "values(:senderId, :text);");
    query.bindValue(":senderId", getUserId(sender));
    query.bindValue(":text", text);
    if(query.exec()) {
        qInfo() << "Message to all from " << sender << " added in database!" ;
        return true;
    }
    else {
        qInfo() << "Message to all from " << sender << " not added in database!" ;
        return false;
    }
}

bool SQLiteDataBase::addPrivateMessage(QString sender, QString reciever, QString text)
{
    QSqlQuery query;
    query.prepare("insert into MessagesPrivate(senderId, recieverId, text) "
                  "values(:senderId, :recieverId, :text);");
    query.bindValue(":senderId", getUserId(sender));
    query.bindValue(":recieverId", getUserId(reciever));
    query.bindValue(":text", text);
    if(query.exec()) {
        qInfo() << "Message to " <<  reciever << "from " << sender << " added in database!" ;
        return true;
    }
    else {
        qInfo() << "Message to " <<  reciever << "from " << sender << " not added in database!" ;
        return false;
    }
}

QVector<QString> SQLiteDataBase::getMessagesBetweenTwoUsers(QString user1, QString user2)
{
    QString userId1 = QString::number(getUserId(user1));
    QString userId2 = QString::number(getUserId(user2));
    QVector<QString> privateMessages {};
    QString message {};
    QSqlQuery query;
    QString str = "select senderId,recieverId,text from MessagesPrivate where (senderId=\'" + userId1 + "\' and recieverId=\'" + userId2 + "\') or (senderId=\'" + userId2 + "\' and recieverId=\'" + userId1 + "\')";

    if(query.exec(str)) {
        while(query.next()) {
            message = getUserById(query.value(0).toString()) + ";" + getUserById(query.value(1).toString()) + ";" + query.value(2).toString();
            privateMessages.push_back(message);
        }
    }
    return privateMessages;
}




