#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextCodec>



struct DataStream{
    quint64 m_textsize;
    quint64 m_datasize;
    QString m_type;
    QByteArray m_stream;

    DataStream(){
        m_textsize = 0;
        m_datasize = 0;
        m_type = "";
        m_stream = "";
    }
    DataStream(const DataStream& stream){
        m_textsize = stream.m_textsize;
        m_datasize = stream.m_datasize;
        m_type = stream.m_type;
        m_stream = stream.m_stream;
    }
    ~DataStream(){
        m_textsize = 0;
        m_datasize = 0;
        m_type.clear();
        m_stream.clear();
    }
    DataStream &operator =(const DataStream& stream){
        m_textsize = stream.m_textsize;
        m_datasize = stream.m_datasize;
        m_type = stream.m_type;
        m_stream = stream.m_stream;
        return *this;
    }
};

typedef DataStream StreamTest;

bool PackageStream(const QVector<QByteArray> &datas,const QVector<QString> &texts,
                   const QVector<QString> &types, QByteArray &stream){
    QVector<StreamTest> vecdata;
    for(int i=0;i<datas.size();i++){
        QByteArray data = datas[i];
        QString    text = texts[i];
        QString    type = types[i];
        StreamTest s;
        QByteArray bytearrText = text.toLocal8Bit();
        s.m_textsize = bytearrText.size();
        s.m_stream.append(bytearrText);
        s.m_datasize = data.size();
        s.m_stream.append(data);
        s.m_type = type;
        vecdata.append(s);
    }
    //将vecdata打包为json
    QByteArray streamByteArr;
    QJsonArray streamTagArr;
    for(int i=0; i<vecdata.size(); i++)
    {
        QJsonObject item;
        item.insert("TextSize", QString::number(vecdata[i].m_textsize));
        item.insert("DataSize", QString::number(vecdata[i].m_datasize));
        item.insert("Type",     vecdata[i].m_type);
        streamTagArr.append(item);

        streamByteArr.append(vecdata[i].m_stream);
    }

    QJsonObject root;
    root.insert("TagInfo", streamTagArr);
    QJsonDocument doc(root);

    QByteArray outHeader(0, sizeof(quint64));
    outHeader.resize(sizeof(quint64));
    outHeader.append(doc.toJson());
    quint64 hLenth = outHeader.size() - sizeof(quint64);
    char *phLenth = outHeader.data();
    memcpy(phLenth, &hLenth, sizeof(quint64));

    QByteArray outDocument;
    outDocument.append(outHeader);
    outDocument.append(streamByteArr);

    QByteArray outStream(0, sizeof(quint64));
    outStream.resize(sizeof(quint64));
    outStream.append(outDocument);

    quint64 sLenth = outStream.size();
    char *psLenth = outStream.data();
    memcpy(psLenth, &sLenth, sizeof(quint64));

    stream = outStream;
}


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
#if 0
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
#endif
//测试文本数据与二进制数据组合发送
    QImage image;
    image.load("F:/opengl/ogldev-source/Content/test.png");
    qDebug() << image.size() << image.byteCount() <<image.text()<<image.textKeys();
    QByteArray binaryData;
    binaryData.resize(image.byteCount());
    memcpy(binaryData.data(), (char*)image.bits(), image.byteCount());
    //----------------------------------文本数据
    QVector<QByteArray> vecStream;
    QVector<QString> vecTag;
    QVector<QString> vecType;

    QJsonObject testTag;
    testTag.insert("user","gg");
    testTag.insert("filename","tag.json");
    testTag.insert("type","1");
    QJsonObject tagjson;
    tagjson.insert("name","g1");
    tagjson.insert("age",18);
    tagjson.insert("sex","F");

    testTag.insert("res",tagjson);
    QJsonDocument jsonDoc(testTag);
    QByteArray byteArrData = jsonDoc.toJson();

    vecTag.push_back(QString(byteArrData));
    vecStream.push_back("");
    vecType.push_back("1");
    //-----------------------------------二进制数据
    QJsonObject testStream;
    testStream.insert("user","gg");
    testStream.insert("filename","gg.png");
    testStream.insert("type","2");

    QJsonDocument jsonDoc2(testStream);
    QByteArray byteArrData2 = jsonDoc2.toJson();

    vecTag.push_back(QString(byteArrData2));
    vecStream.push_back(binaryData);
    vecType.push_back("2");
    //------------------------------进行组合
    QByteArray resByte;
    PackageStream(vecStream,vecTag,vecType,resByte);

    QString strurl = "http://192.168.1.68:8899/Saveinfo/";
    QString postdata = QString(resByte);
    QString resdata;
    m_netOpe->StartRequest(strurl,2,postdata,resdata);
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
