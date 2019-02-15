#ifndef NETWORKOPE_H
#define NETWORKOPE_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QFile>

class NetworkOpe : public QObject
{
    Q_OBJECT
public:
    NetworkOpe(QObject *parent = 0);
    ~NetworkOpe();
    void initArg();
    //get or post 请求
    void StartRequest(QString url,int rtype,QString& postdata,QString& resdata);
signals:
    void SigDownFinished(QString);
public slots:
    void slotNerworkError(QNetworkReply::NetworkError networkError);
    void slotDownTimeOut();
    void slotDownFinished(QNetworkReply*);//下载完成时

    void slotHttpFinished();  //完成下载后的处理
    void slotHttpReadyRead();//接收到数据时的处理
    void slotUpdateDataReadProgress(qint64 bytesRead, qint64 totalBytes);//更新进度
private:
    QNetworkAccessManager* m_AccessManaget;//控制网络请求
    QNetworkReply* m_Reply;//网络请求返回
    QEventLoop* m_EventLoop;//事件循环控制,可用于在某处暂停程序

    int m_downError;//网络返回错误控制
    QTimer* m_timer;
    int m_timeout;

    int m_downNum; //尝试下载次数
    int m_requsetType;//网络请求类型
    QString m_strurl;//网络地址
    QString m_postData;//发送网络的数据
    QString m_requestData;//请求结果

    QFile *m_file;  //文件指针
};

#endif // NETWORKOPE_H
