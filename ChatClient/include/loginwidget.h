#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <memory>
#include <QTcpSocket>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QTcpSocket* socket, QWidget *parent = nullptr);
    ~LoginWidget();
    void loginResult(int command);

signals:

private slots:
    void closeWidget();
    void on_okButton_clicked();

private:
    Ui::LoginWidget *ui;
    QTcpSocket* _socket;
};

#endif // LOGINWIDGET_H
