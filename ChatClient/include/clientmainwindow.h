
#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

#include "loginwidget.h"
#include "registrationwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class ClientMainWindow;
    class LoginWidget;
    class RegistrationWidget;
}
QT_END_NAMESPACE

class ClientMainWindow : public QMainWindow

{
    Q_OBJECT

public:
    ClientMainWindow(QWidget *parent = nullptr);
    ~ClientMainWindow();
    QTcpSocket* getSocket();

    QString getCurrentLogin() const;
    void setCurrentLogin(const QString& newCurrentLogin);
    void enterUserInChat(QString login, QString users);
    void banUser();
    void addUsersInCombobox(QString str);
    void receiveMessagesBetweenTwoUsers(QString str);
    void receiveCommandMessageFromServer(QString cmd);

private slots:
    void disconnect();
    void openLoginWidget();
    void openRegistrationWidget();
    void on_connectButton_clicked();
    void on_sendButton_clicked();
    void readData();
    void exitUser();

private:
    Ui::ClientMainWindow *ui;
    QTcpSocket *_socket;

    LoginWidget* _loginWidget;
    RegistrationWidget* _registrationWidget;

    bool _isOnline {};
    QString _currentLogin {};
};

#endif // CLIENTMAINWINDOW_H
