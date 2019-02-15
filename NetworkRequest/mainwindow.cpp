#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->UrlLineEdit->setText("http://192.168.1.68:8888/GetUserAIInfo");
    m_netOpe = new NetworkOpe;
    connect(m_netOpe,SIGNAL(SigDownFinished(QString)),
            this,SLOT(slotGetRequestData(QString)));

    m_udpClient = new UDPClient;
    m_udpServer = new UDPServer;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_netOpe;
    delete m_udpClient;
    delete m_udpServer;
}

void MainWindow::slotGetRequestData(QString resdata)
{
    ui->textEdit->setText(resdata);
    ui->textEdit->update();
}

void MainWindow::on_StartBtn_clicked()
{
    QString strurl = ui->UrlLineEdit->text();
    QString method = ui->comboBox->currentText();
    QString postdata = ui->lineEdit->text();
    QString resdata;
    if(method == "Post"){
        ui->textEdit_Error->append("post request!");
        m_netOpe->StartRequest(strurl,2,postdata,resdata);
    }else if(method == "Get"){
        ui->textEdit_Error->append("get request!");
        m_netOpe->StartRequest(strurl,3,postdata,resdata);
    }
    ui->textEdit->setText(resdata);
    ui->textEdit->update();
    ui->textEdit_Error->update();
}

void MainWindow::on_ClearBtn_clicked()
{
    ui->textEdit_Error->clear();
}

void MainWindow::on_actionUDPClient_triggered()
{
    m_udpClient->show();
}

void MainWindow::on_actionUDPServer_triggered()
{
    m_udpServer->show();
}
