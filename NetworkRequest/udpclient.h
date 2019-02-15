#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QDialog>
#include <QtNetwork>
#include <QFile>

namespace Ui {
class UDPClient;
}

class UDPClient : public QDialog
{
    Q_OBJECT

public:
    explicit UDPClient(QWidget *parent = 0);
    ~UDPClient();
private slots:
    void on_sendBtn_clicked();
    void slotReadMessage();  //接收数据
    void slotDisplayError(QAbstractSocket::SocketError);//显示错误
    void on_connectBtn_clicked();//连接服务器
    //tcp发送文件
    void slotStartTransfer();  //发送文件大小等信息
    void slotUpdateClientProgress(qint64 numBytes); //发送数据，更新进度条
private:
    Ui::UDPClient *ui;
    QUdpSocket* m_sender;
    //tcp
    QTcpSocket *m_tcpSocket;
    QString m_message;  //存放从服务器接收到的字符串
    quint16 m_blockSize; //存放文件的大小信息
    //tcp发送文件
    QFile *m_localFile;//要发送的文件
    qint64 m_totalBytes;//数据总大小
    qint64 m_bytesWritten;  //已经发送数据大小
    qint64 m_bytesToWrite;   //剩余数据大小
    qint64 m_loadSize;   //每次发送数据的大小
    QString m_fileName;  //保存文件路径
    QByteArray m_outBlock;  //数据缓冲区，即存放每次要发送的数据
};

#endif // UDPCLIENT_H
