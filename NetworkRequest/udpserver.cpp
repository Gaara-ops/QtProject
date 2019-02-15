#include "udpserver.h"
#include "ui_udpserver.h"
#include <QDebug>
UDPServer::UDPServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UDPServer)
{
    ui->setupUi(this);
    m_receiver = NULL;
    m_tcpServer = NULL;

    m_totalBytes = 0;
    m_bytesReceived = 0;
    m_fileNameSize = 0;
}

UDPServer::~UDPServer()
{
    delete ui;
    if(m_receiver != NULL){
        delete m_receiver;
        m_receiver = NULL;
    }
    if(m_tcpServer != NULL){
        delete m_tcpServer;
        m_tcpServer = NULL;
    }
}

void UDPServer::SetServerType(int type)
{
    if(type == 0){
        if(m_receiver != NULL)
            return;
        m_receiver = new QUdpSocket(this);
        //ShareAddress,它表明其他服务也可以绑定到这个端口上
        m_receiver->bind(45454,QUdpSocket::ShareAddress);
        connect(m_receiver,SIGNAL(readyRead()),
                this,SLOT(processPendingDatagram()));
    }else if(type == 1){
        if(m_tcpServer != NULL)
            return;
        m_tcpServer = new QTcpServer(this);
        if(!m_tcpServer->listen(QHostAddress::LocalHost,6666))
        {  //**本地主机的6666端口，如果出错就输出错误信息，并关闭
            qDebug() << m_tcpServer->errorString();
            close();
        }
        //连接信号和相应槽函数
        connect(m_tcpServer,SIGNAL(newConnection()),this,SLOT(slotSendMessage()));
    }
}
//处理等待的数据报
void UDPServer::processPendingDatagram()
{
    //拥有等待的数据报
    while (m_receiver->hasPendingDatagrams()) {
        QByteArray datagram;//拥于存放接收的数据报
        //让datagram的大小为等待处理的数据报的大小，这样才能接收到完整的数据
        datagram.resize(m_receiver->pendingDatagramSize());
        //接收数据报，将其存放到datagram中
        m_receiver->readDatagram(datagram.data(),datagram.size());
        //将数据报内容显示出来
        ui->label->setText(ui->label->text()+datagram);
    }
}

void UDPServer::slotSendMessage()
{
    /**
      *发送数据
    //用于暂存我们要发送的数据
    QByteArray block;
    //使用数据流写入数据
    QDataStream out(&block,QIODevice::WriteOnly);
    //设置数据流的版本，客户端和服务器端使用的版本要相同
    out.setVersion(QDataStream::Qt_5_3);
    out<<(quint16) 0;
    out<<tr("hello Tcp!!!");
    out.device()->seek(0);
    out<<(quint16) (block.size() - sizeof(quint16));
    //我们获取已经建立的连接的子套接字
    QTcpSocket *clientConnection = m_tcpServer->nextPendingConnection();
    connect(clientConnection,SIGNAL(disconnected()),clientConnection,
           SLOT(deleteLater()));
    clientConnection->write(block);
    clientConnection->disconnectFromHost();
    //发送数据成功后，显示提示
    ui->label_TCP->setText("send message successful!!!");
    */

    /**
      *接收数据
      */
    m_tcpServerConnection = m_tcpServer->nextPendingConnection();
    connect(m_tcpServerConnection,SIGNAL(readyRead()),
            this,SLOT(slotUpdateServerProgress()));
    connect(m_tcpServerConnection,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(slotDisplayError(QAbstractSocket::SocketError)));
    qDebug() << ("接受连接");
    m_tcpServer->close();
}

void UDPServer::on_UDPServerBtn_clicked()
{
    SetServerType(0);
    ui->label->setText("start udp succeed!!");
    ui->UDPServerBtn->setEnabled(false);
}

void UDPServer::on_TCPServerBtn_clicked()
{
    m_bytesReceived =0;
    SetServerType(1);
    ui->label_TCP->setText("start tcp succeed!!");
    ui->TCPServerBtn->setEnabled(false);
}

void UDPServer::slotUpdateServerProgress()
{
    QDataStream in(m_tcpServerConnection);
    in.setVersion(QDataStream::Qt_5_3);
    if(m_bytesReceived <= sizeof(qint64)*2)
    { //如果接收到的数据小于16个字节，那么是刚开始接收数据，我们保存到//来的头文件信息
        if((m_tcpServerConnection->bytesAvailable() >= sizeof(qint64)*2)
            && (m_fileNameSize == 0))
        { //接收数据总大小信息和文件名大小信息
            in >> m_totalBytes >> m_fileNameSize;
            m_bytesReceived += sizeof(qint64) * 2;
        }
        if((m_tcpServerConnection->bytesAvailable() >= m_fileNameSize)
            && (m_fileNameSize != 0))
        {  //接收文件名，并建立文件
            in >> m_fileName;
            qDebug() << QString(tr("接收文件 %1 ...").arg(m_fileName));
            m_bytesReceived += m_fileNameSize;
            m_fileName = "tt_gg.txt";
            m_localFile= new QFile(m_fileName);
            if(!m_localFile->open(QFile::WriteOnly))
            {
                 qDebug() << "open file error!";
                 return;
            }
        }
        else return;
    }
    if(m_bytesReceived < m_totalBytes)
    {  //如果接收的数据小于总数据，那么写入文件
        m_bytesReceived += m_tcpServerConnection->bytesAvailable();
        m_inBlock= m_tcpServerConnection->readAll();
        m_localFile->write(m_inBlock);
        m_inBlock.resize(0);
    }
    //更新进度条
    qDebug() << "all:"<<m_totalBytes<<",had get:"<<m_bytesReceived;

    if(m_bytesReceived == m_totalBytes)
    { //接收数据完成时
        m_tcpServerConnection->close();
        m_localFile->close();
    }
}

void UDPServer::slotDisplayError(QAbstractSocket::SocketError socketError)
{
    qDebug() << m_tcpServerConnection->errorString();
    m_tcpServerConnection->close();
}
