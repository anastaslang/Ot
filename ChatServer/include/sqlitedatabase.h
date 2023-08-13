#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <QObject>
#include <QtSql>
#include <QVector>
#include <QString>

class SQLiteDataBase : public QObject
{
    Q_OBJECT
public:
    explicit SQLiteDataBase(QObject *parent = nullptr);
    bool openDatabase();
    bool createTables();
    bool checkUserByLogin(QString login);
    int getUserId(QString login);
    QVector<QString> getAllUsers();
    int addUser(QString name, QString login, QString password);
    bool checkUserByLoginAndPassword(QString login, QString password);
    bool addMessageToAll(QString sender, QString text);
    bool addPrivateMessage(QString sender, QString reciever, QString text);
    QVector<QString> getMessagesBetweenTwoUsers(QString user1, QString user2);
    QString getUserById(QString id);
    QVector<QString> get10MessagesToAll();

signals:

private:
    QSqlDatabase _db;
};

#endif // SQLITEDATABASE_H
