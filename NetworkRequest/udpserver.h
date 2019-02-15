#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QDialog>
#include <QtNetwork>

namespace Ui {
class UDPServer;
}

class UDPServer : public QDialog
{
    Q_OBJECT

public:
    explicit UDPServer(QWidget *parent = 0);
    ~UDPServer();
    //设置服务类型为tcp或udp,0->upd;1->tcp
    void SetServerType(int type);
private slots:
    void processPendingDatagram();
    void slotSendMessage();
    void on_UDPServerBtn_clicked();

    void on_TCPServerBtn_clicked();
    //接收文件
    void slotUpdateServerProgress();  //更新进度条，接收数据
    //显示错误
    void slotDisplayError(QAbstractSocket::SocketError socketError);
private:
    Ui::UDPServer *ui;
    QUdpSocket* m_receiver;
    QTcpServer* m_tcpServer;
    //接收文件
    QTcpSocket *m_tcpServerConnection;
    qint64 m_totalBytes;  //存放总大小信息
    qint64 m_bytesReceived;  //已收到数据的大小
    qint64 m_fileNameSize;  //文件名的大小信息
    QString m_fileName;   //存放文件名
    QFile *m_localFile;   //本地文件
    QByteArray m_inBlock;   //数据缓冲区
};

#endif // UDPSERVER_H
