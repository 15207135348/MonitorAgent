#include "setdialog.h"
#include "ui_setdialog.h"
#include <util/ConfigUtil.h>
#include <QMessageBox>

SetDialog::SetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetDialog)
{
    ui->setupUi(this);
    QMap<QString,QString> map = ConfigUtil::loadConf();
    ui->lineEdit_1->setText(map["host.address"]);
    ui->lineEdit_2->setText(map["host.port"]);
    ui->lineEdit_3->setText(map["gather.internal"]);
    ui->radioButton->setChecked(map["self.start"]=="true");
}

SetDialog::~SetDialog()
{
    delete ui;
}

void SetDialog::on_buttonBox_accepted()
{
    QMap<QString,QString> map;
    QString ip = ui->lineEdit_1->text();
    QString port = ui->lineEdit_2->text();
    QString internal = ui->lineEdit_3->text();
    bool restart = ui->radioButton->isChecked();
    QRegExp ip_rx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if(ip!="localhost" && !ip_rx.exactMatch(ip))
    {
        QMessageBox::information(this, "warn", "Host ip is error");
        return;
    }
    QRegExp port_rx("^([0-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");
    if(!port_rx.exactMatch(port))
    {
        QMessageBox::information(this, "warn", "Host port is error");
        return;
    }
    if(internal.toLong() < 100)
    {
        QMessageBox::information(this, "warn", "Gather internal cannot less than 100!");
        return;
    }
    map["host.address"] = ip;
    map["host.port"] = port;
    map["gather.internal"] = internal;
    if(restart)
    {
        map["self.start"] = "true";
    }
    else
    {
        map["self.start"] = "false";
    }
    ConfigUtil::saveConf(map);
    QMessageBox::information(this, "info", "Save the configuration successfully and take effect after restart.");
    this->close();
    this->deleteLater();
}

void SetDialog::on_buttonBox_rejected()
{
    this->close();
    this->deleteLater();
}
