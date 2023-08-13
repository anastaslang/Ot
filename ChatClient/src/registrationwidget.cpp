#include "registrationwidget.h"
#include "ui_registrationwidget.h"
#include <QMessageBox>
//#include <QRegExp>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

RegistrationWidget::RegistrationWidget(QTcpSocket* socket, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistrationWidget),
    _socket(socket)
{
    ui->setupUi(this);

    QRegularExpression regexp("[a-zA-Z]+");
    ui->nameEdit->setValidator(new QRegularExpressionValidator(regexp, this));
    QRegularExpression regExp("\\w{1,10}");
    ui->loginEdit->setValidator(new QRegularExpressionValidator(regExp, this));
    ui->passwordEdit->setValidator(new QRegularExpressionValidator(regExp, this));
    connect(ui->cancelButton, &QPushButton::clicked, this, &RegistrationWidget::closeWidget);
}

RegistrationWidget::~RegistrationWidget()
{
    delete ui;
}

void RegistrationWidget::registrationResult(int command)
{
    switch (command) {
    case 111:
        ui->registrationResultLabel->setText("<font color=red>User with login " + _userLogin + " not added in database! "
                                                "Registration failed</font>");
        break;
    case 112:
        ui->registrationResultLabel->setText("<font color=green>User with login " + _userLogin + " add in database! "
                                                "Registration success</font>");
        QMessageBox(QMessageBox::Information,
                    QObject::tr("Registration"),
                    QObject::tr("Registration of a new user was successful!"),
                    QMessageBox::Ok).exec();
        this->close();
        break;
    case 113:
        ui->registrationResultLabel->setText("<font color=red>User with login " + _userLogin + " is already exists! Try again</font>");
        break;
    default:
        break;
    }

}

void RegistrationWidget::closeWidget()
{
    this->close();
}


void RegistrationWidget::on_okButton_clicked()
{
    QString message;
    if(!ui->nameEdit->text().isEmpty() && !ui->loginEdit->text().isEmpty() && !ui->passwordEdit->text().isEmpty() && !ui->confirmPasswordEdit->text().isEmpty())
    {
        if(ui->passwordEdit->text() == ui->confirmPasswordEdit->text())
        {
            _userName = ui->nameEdit->text();
            _userLogin = ui->loginEdit->text();
            _userPassword = ui->passwordEdit->text();
            message = "110;" + _userName + ";" + _userLogin + ";" + _userPassword;
            _socket->write(message.toUtf8());
        }
        else
        {
            ui->registrationResultLabel->setText(tr("<font color=red>Passwords don't match. Try again.</font>"));
        }
    }
    else
    {
        ui->registrationResultLabel->setText(tr("<font color=red>All fields must be filled in</font>"));
    }
}

