#ifndef REGISTRATIONWIDGET_H
#define REGISTRATIONWIDGET_H
#include <QWidget>
#include <memory>
#include <QTcpSocket>

namespace Ui {
class RegistrationWidget;
}

class RegistrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationWidget(QTcpSocket* socket, QWidget *parent = nullptr);
    ~RegistrationWidget();
    QString getName() const {return _userName;}
    QString getLogin() const {return _userLogin;}
    QString getPassword() const {return _userPassword;}
    void registrationResult(int command);

signals:

private slots:
    void closeWidget();
    void on_okButton_clicked();

private:
    Ui::RegistrationWidget *ui;
    QTcpSocket* _socket;
    QString _userName;
    QString _userLogin;
    QString _userPassword;
};

#endif // REGISTRATIONWIDGET_H
