#include "loginwidget.h"
#include "ui_loginwidget.h"
#include <QMessageBox>

LoginWidget::LoginWidget(QTcpSocket* socket, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget),
    _socket(socket)
{
    ui->setupUi(this);

    connect(ui->cancelButton, &QPushButton::clicked, this, &LoginWidget::closeWidget);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::loginResult(int command)
{
    switch (command) {
    case 121:
        ui->loginResultLabel->setText(tr("<font color=green>Authentication was successful</font>"));
        QMessageBox(QMessageBox::Information,
                    QObject::tr("Authentication"),
                    QObject::tr("Authentication user was successful!"),
                    QMessageBox::Ok).exec();
        closeWidget();
        break;
    case 122:
        ui->loginResultLabel->setText(tr("<font color=red>Authentication failed. Try again!</font>"));
        break;
    case 123:
        ui->loginResultLabel->setText(tr("<font color=red>You are already online</font>"));
        break;
    default:
        break;
    }
}

void LoginWidget::closeWidget()
{
    this->close();
}


void LoginWidget::on_okButton_clicked()
{
    QString message;
    if(!ui->loginEdit->text().isEmpty() && !ui->passwordEdit->text().isEmpty()) {
        message = "120;" +  ui->loginEdit->text() + ";" + ui->passwordEdit->text();
        _socket->write(message.toUtf8());
    }
    else {
        ui->loginResultLabel->setText(tr("<font color=red>All fields must be filled in</font>"));
    }
}

