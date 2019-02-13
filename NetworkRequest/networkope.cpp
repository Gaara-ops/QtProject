#include "networkope.h"
#include <QDebug>

#define TIMEOUT 8*60*1000  //8 minute
#define MaxDownNum 3 //最大尝试下载次数

NetworkOpe::NetworkOpe(QObject *parent):QObject(parent)
{
    m_Reply = NULL;
    m_EventLoop = new QEventLoop();
    m_AccessManaget = new QNetworkAccessManager(this);
    connect(m_AccessManaget, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotDownFinished(QNetworkReply*)));

    m_downError = 0;
    m_timeout = TIMEOUT;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotDownTimeOut()));

    m_downNum = 0;
    m_requsetType = 0;
}

NetworkOpe::~NetworkOpe()
{
    m_EventLoop->exit(0);
    delete m_EventLoop;
    m_timer->stop();
    delete m_timer;
    delete m_AccessManaget;
}

void NetworkOpe::initArg()
{
    m_timer->stop();
    m_EventLoop->exit(0);
    m_downError = 0;
}

void NetworkOpe::StartRequest(QString url, int rtype, QString &postdata,
                              QByteArray &resdata)
{
    initArg();
    m_strurl = url;
    m_requsetType = rtype;
    m_postData = postdata;

    QNetworkRequest* request= new QNetworkRequest(QUrl(url));
    request->setHeader(QNetworkRequest::ContentTypeHeader,
                       "application/x-www-form-urlencoded");
    if(rtype == 2){
        qDebug() << "method is Post";
        m_Reply = m_AccessManaget->post(*request,postdata.toUtf8());
    }else if(rtype == 3){
        qDebug() << "method is Get";
        m_Reply = m_AccessManaget->get(*request);
    }
    connect(m_Reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(slotNerworkError(QNetworkReply::NetworkError)));

    m_timer->start(m_timeout);
    if(!m_EventLoop->isRunning()){
       m_EventLoop->exec();
        if(m_Reply->error() == QNetworkReply::NoError){
            qDebug() << "HttpRequest Succ!!";
            m_requestData = m_Reply->readAll();
            resdata = m_requestData;
        }else{
            qDebug() << "HttpRequest Failure!!";
        }
    }
}

void NetworkOpe::slotNerworkError(QNetworkReply::NetworkError networkError)
{
    QString errorinfo = "networkError::" +QString::number(networkError);
    qDebug() << errorinfo;
    m_downError = networkError;
    m_timer->stop();
    if( m_downError == QNetworkReply::ContentNotFoundError)
    {
        m_EventLoop->exit(0);
        emit SigDownFinished();
    }
    m_Reply->deleteLater();
}

void NetworkOpe::slotDownTimeOut()
{
    qDebug() << ("network timeout!!!");
    m_timer->stop();
    m_downError = -1;
}

void NetworkOpe::slotDownFinished(QNetworkReply *)
{
    m_timer->stop();
    if(m_downError != 0 && m_downNum<MaxDownNum){
        m_downNum++;
        m_downError = 0;
        if( m_requsetType == 0 ){

        }else if( 1 == m_requsetType){

        }else if(2 == m_requsetType){

        }else if(3 == m_requsetType){

        }
    }else{
        if(m_downNum == MaxDownNum ){
            qDebug() << ("had try MaxDownNum!!");
        }
        m_downNum = 0;
        m_EventLoop->exit(0);
        emit SigDownFinished();
        m_Reply->deleteLater();
    }
}
