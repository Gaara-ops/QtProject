#include "networkope.h"
#include <QDebug>
#include <QTextCodec>

#define TIMEOUT 8*60*1000  //8 minute
#define MaxDownNum 3 //最大尝试下载次数

NetworkOpe::NetworkOpe(QObject *parent):QObject(parent)
{
    m_Reply = NULL;
    m_EventLoop = new QEventLoop();
    m_AccessManaget = new QNetworkAccessManager(this);
    /*connect(m_AccessManaget, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotDownFinished(QNetworkReply*)));*/

    m_downError = 0;
    m_timeout = TIMEOUT;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotDownTimeOut()));

    m_downNum = 0;
    m_requsetType = 0;

    m_file = NULL;
}

NetworkOpe::~NetworkOpe()
{
    m_EventLoop->exit(0);
    delete m_EventLoop;
    m_timer->stop();
    delete m_timer;
    delete m_AccessManaget;

    if(m_file != NULL){
        m_file->close();
        delete m_file;
    }
}

void NetworkOpe::initArg()
{
    m_timer->stop();
    m_EventLoop->exit(0);
    m_downError = 0;
}

void NetworkOpe::StartRequest(QString url, int rtype, QString &postdata,
                              QString &resdata)
{
    m_file = new QFile("./index.html");
    if(!m_file->open(QIODevice::WriteOnly))
    {
        qDebug() << "file open error";
        delete m_file;
        m_file = 0;
        return;
    }
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
    //接收到数据时--每当有新的数据可以读取时，都会发射该信号
    connect(m_Reply, SIGNAL(readyRead()), this, SLOT(slotHttpReadyRead()));
    //更新进度--每当网络请求的下载进度更新时
    connect(m_Reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(slotUpdateDataReadProgress(qint64, qint64)));
    //完成下载--每当应答处理结束时
    connect(m_Reply, SIGNAL(finished()), this, SLOT(slotHttpFinished()));

/*
    m_timer->start(m_timeout);
    if(!m_EventLoop->isRunning()){
       m_EventLoop->exec();
        if(m_Reply->error() == QNetworkReply::NoError){
            qDebug() << "HttpRequest Succ!!";
            //正产显示中文
            QTextCodec *codec = QTextCodec::codecForName("utf8");
            m_requestData = codec->toUnicode(m_Reply->readAll());

            resdata = m_requestData;
        }else{
            qDebug() << "HttpRequest Failure!!";
        }
    }
*/
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
        emit SigDownFinished(m_requestData);
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
        emit SigDownFinished(m_requestData);
        m_Reply->deleteLater();
    }
}

void NetworkOpe::slotHttpFinished()
{
    qDebug() << "slotHttpFinished";
    m_file->flush();
    m_file->close();
    m_Reply->deleteLater();
    m_Reply = 0;
    delete m_file;
    m_file = 0;
    emit SigDownFinished(m_requestData);
}

void NetworkOpe::slotHttpReadyRead()
{
    qDebug() << "slotHttpReadyRead";
    if (m_file){
        QTextCodec *codec = QTextCodec::codecForName("utf8");
        m_requestData = codec->toUnicode(m_Reply->readAll());
        m_file->write(m_requestData.toLatin1());
    }
}

void NetworkOpe::slotUpdateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    qDebug() << "down load:" << bytesRead<<"/"<<totalBytes;
}
