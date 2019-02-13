#ifndef NETWORKOPE_H
#define NETWORKOPE_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>

class NetworkOpe : public QObject
{
    Q_OBJECT
public:
    NetworkOpe(QObject *parent = 0);
    ~NetworkOpe();
    void initArg();
    void StartRequest(QString url,int rtype,QString& postdata,QByteArray& resdata);
signals:
    void SigDownFinished();
public slots:
    void slotNerworkError(QNetworkReply::NetworkError networkError);
    void slotDownTimeOut();
    void slotDownFinished(QNetworkReply*);
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
    QByteArray m_requestData;//请求结果
};

#endif // NETWORKOPE_H
