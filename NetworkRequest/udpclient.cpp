#include "udpclient.h"
#include "ui_udpclient.h"

UDPClient::UDPClient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UDPClient)
{
    ui->setupUi(this);
    m_sender = new QUdpSocket(this);

    //tcp
    ui->lineEdit_Host->setText("localhost");
    ui->lineEdit_Port->setText("6666");
    m_tcpSocket = new QTcpSocket(this);
    //当有数据到来时发出readyRead()信号
    connect(m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slotReadMessage()));
    //当出现错误时发出error()信号
    connect(m_tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
             this,SLOT(slotDisplayError(QAbstractSocket::SocketError)));
    //tcp发送文件
    m_fileName = "./testsend.txt";
    m_loadSize = 4*1024;
    m_totalBytes = 0;
    m_bytesWritten = 0;
    m_bytesToWrite = 0;
    //当连接服务器成功时，发出connected()信号，我们开始传送文件
    connect(m_tcpSocket,SIGNAL(connected()),this,SLOT(slotStartTransfer()));
    //当有数据发送成功时，我们更新进度条
    connect(m_tcpSocket,SIGNAL(bytesWritten(qint64)),this,
           SLOT(slotUpdateClientProgress(qint64)));
}

UDPClient::~UDPClient()
{
    delete ui;
    delete m_sender;
    delete m_tcpSocket;
}

void UDPClient::on_sendBtn_clicked()
{
    QByteArray postdata = "hello,gg!";
    m_sender->writeDatagram(postdata.data(),postdata.size(),
                            QHostAddress::Broadcast,45454);
}

void UDPClient::slotReadMessage()
{
    QDataStream in(m_tcpSocket);
    //设置数据流版本，这里要和服务器端相同
    in.setVersion(QDataStream::Qt_5_3);
    if(m_blockSize==0) //如果是刚开始接收数据
    {
       //判断接收的数据是否有两字节，也就是文件的大小信息
       //如果有则保存到blockSize变量中，没有则返回，继续接收数据
       if(m_tcpSocket->bytesAvailable() < (int)sizeof(quint16)) return;
       in >> m_blockSize;
    }
    //如果没有得到全部的数据，则返回，继续接收数据
    if(m_tcpSocket->bytesAvailable() < m_blockSize) return;
    //将接收到的数据存放到变量中
    in >> m_message;
    //显示接收到的数据
    ui->textEdit->append(m_message);
}

void UDPClient::slotDisplayError(QAbstractSocket::SocketError)
{
    qDebug() << m_tcpSocket->errorString(); //输出错误信息
    m_tcpSocket->close();
}

void UDPClient::on_connectBtn_clicked()
{
    ui->connectBtn->setEnabled(false);
    m_bytesWritten=0;

    m_blockSize = 0; //初始化其为0
    m_tcpSocket->abort(); //取消已有的连接
    //连接到主机，这里从界面获取主机地址和端口号
    m_tcpSocket->connectToHost(ui->lineEdit_Host->text(),
                               ui->lineEdit_Port->text().toInt());
}
//实现文件头结构的发送。
void UDPClient::slotStartTransfer()
{
    m_localFile = new QFile(m_fileName);
    if(!m_localFile->open(QFile::ReadOnly)){
       qDebug() << "open file error!";
       return;
    }
    //文件总大小
    m_totalBytes = m_localFile->size();

    QDataStream sendOut(&m_outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_3);
    QString currentFileName = m_fileName.right(m_fileName.size()
                              - m_fileName.lastIndexOf('/')-1);
    //依次写入总大小信息空间，文件名大小信息空间，文件名
    sendOut << qint64(0) << qint64(0) << currentFileName;
    //这里的总大小是文件名大小等信息和实际文件大小的总和
    m_totalBytes += m_outBlock.size();
    sendOut.device()->seek(0);
    //返回outBolock的开始，用实际的大小信息代替两个qint64(0)空间
    sendOut<<m_totalBytes<<qint64((m_outBlock.size()-sizeof(qint64)*2));
    //发送完头数据后剩余数据的大小
    m_bytesToWrite = m_totalBytes - m_tcpSocket->write(m_outBlock);
    qDebug() << ("已连接");
    m_outBlock.resize(0);
}
//更新进度条，实现文件的传送
void UDPClient::slotUpdateClientProgress(qint64 numBytes)
{
    //已经发送数据的大小
    m_bytesWritten += (int)numBytes;

    if(m_bytesToWrite > 0) //如果已经发送了数据
    {
       //每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，
       //就发送剩余数据的大小
       m_outBlock = m_localFile->read(qMin(m_bytesToWrite,m_loadSize));
       //发送完一次数据后还剩余数据的大小
       m_bytesToWrite -= (int)m_tcpSocket->write(m_outBlock);
       //清空发送缓冲区
       m_outBlock.resize(0);
    } else {
       m_localFile->close(); //如果没有发送任何数据，则关闭文件
    }
    //更新进度条
    qDebug() << "all:"<<m_totalBytes<<",had send:" <<m_bytesWritten;
    if(m_bytesWritten == m_totalBytes) //发送完毕
    {
       m_localFile->close();
       m_tcpSocket->close();
    }
}
