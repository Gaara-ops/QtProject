#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyFunction.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	/**
	  set window to max
	this->setWindowState(Qt::WindowMaximized);*/
	m_timer = new QTimer(this);
    m_currentRow = 0;
    m_scrollIndex = 0;
	connect(ui->textEdit->verticalScrollBar(),SIGNAL(sliderMoved(int)),
			this,SLOT(slotSlideBarMoved(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UpdateListWidget()
{
    ui->listWidget->setCurrentRow(m_currentRow);
    ui->textEdit->setText(m_data.at(m_currentRow));
}

void MainWindow::SaveStatus()
{
    QString fontname = m_font.family();
    int pointsize = m_font.pointSize();
    int weight = m_font.weight();

    m_scrollIndex = ui->textEdit->verticalScrollBar()->sliderPosition();

	QJsonObject json;
	json.insert("fontname",QString(fontname));
    json.insert("pointsize",pointsize);
    json.insert("weight",weight);
    json.insert("gindex",m_currentRow);
    json.insert("scrollindex",m_scrollIndex);
	QJsonObject jsonwindow;
	jsonwindow.insert("width",this->frameGeometry().width());
	jsonwindow.insert("height",this->frameGeometry().height());
	QJsonArray jsonarr;
	jsonarr.push_back(json);
	jsonarr.push_back(jsonwindow);
	QJsonObject jsonobj;
	jsonobj.insert("configure",jsonarr);


    QFile file("./configure.txt");
    file.open( QIODevice::WriteOnly);
    QTextStream out(&file);
	out.setCodec(QTextCodec::codecForName("UTF-8"));
    QJsonDocument document;
	document.setObject(jsonobj);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
	QString strJson(byteArray);
    out << strJson;
    file.close();
}

void MainWindow::SetStatus()
{
    QFile file("./configure.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
         return;
    }
    QString fontname;
    int pointsize = 0;
    int weight = 0;

    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName("UTF-8"));
        QString allData = in.readAll();
		QByteArray byteArray = allData.toUtf8();
        QJsonParseError jsonError;
        QJsonDocument doucment = QJsonDocument::fromJson(byteArray,&jsonError);
		if(!doucment.isNull()&&(jsonError.error == QJsonParseError::NoError)){
            if (doucment.isObject()) {
                QJsonObject object = doucment.object();
				if (object.contains("fontname")) {
                    QJsonValue value = object.value("fontname");
                    if (value.isString()){
                        fontname = value.toString();
                    }
                }
                if (object.contains("pointsize")) {
                    QJsonValue value = object.value("pointsize");
                    pointsize = value.toInt();
                }
                if (object.contains("weight")) {
                    QJsonValue value = object.value("weight");
                    weight = value.toInt();
                }
                if (object.contains("gindex")) {
                    QJsonValue value = object.value("gindex");
                    m_currentRow = value.toInt();
                }
                if (object.contains("scrollindex")) {
                    QJsonValue value = object.value("scrollindex");
                    m_scrollIndex = value.toInt();
                }
            }
            m_font.setFamily(fontname.toUtf8());
            m_font.setPointSize(pointsize);
			m_font.setWeight(weight);
        }
    file.close();

    ui->listWidget->setCurrentRow(m_currentRow);
    ui->textEdit->setText(m_data.at(m_currentRow));
    ui->textEdit->setFont(m_font);
	ui->textEdit->update();
	connect(m_timer,SIGNAL(timeout()),this,SLOT(slotUpdateSlide()));
	m_timer->start(500);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	if( QMessageBox::question(this,
							 tr("Quit"),
							 tr("Are you sure to Quit?"),
							 QMessageBox::Yes, QMessageBox::No )
				   == QMessageBox::Yes){
		SaveStatus();
		e->accept();
	}
	else
	  e->ignore();
}

void MainWindow::on_upBtn_clicked()
{
    m_currentRow--;
    UpdateListWidget();
}

void MainWindow::on_downBtn_clicked()
{
    m_currentRow++;
    UpdateListWidget();
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    m_currentRow = currentRow;
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    UpdateListWidget();
}

void MainWindow::on_actionFont_triggered()
{
    //打开字体设置对话框，返回设置的字体
    bool ok;
    m_font = QFontDialog::getFont(&ok,m_font);
    if (ok)
    {
        ui->textEdit->setFont(m_font);
        ui->textEdit->update();
    }
}

void MainWindow::on_actionSaveStatus_triggered()
{
	SaveStatus();
}

void MainWindow::slotUpdateSlide()
{
	qDebug() << "update slide";
	if(ui->textEdit->verticalScrollBar()->maximum() == 0){
		return;
	}
	int maxindex = ui->textEdit->verticalScrollBar()->maximum();
	if(maxindex<=m_scrollIndex){
		m_scrollIndex = maxindex/2;
	}
	ui->textEdit->verticalScrollBar()->setSliderPosition(m_scrollIndex);
	ui->textEdit->verticalScrollBar()->update();
	m_timer->stop();
	disconnect(m_timer,SIGNAL(timeout()),this,SLOT(slotUpdateSlide()));
}

void MainWindow::slotPlay()
{
	m_scrollIndex++;
	int maxindex = ui->textEdit->verticalScrollBar()->maximum();
	if(m_scrollIndex > maxindex){
		m_scrollIndex=0;
		on_downBtn_clicked();
	}else{
		ui->textEdit->verticalScrollBar()->setSliderPosition(m_scrollIndex);
		ui->textEdit->verticalScrollBar()->update();
	}
}

void MainWindow::slotSlideBarMoved(int pos)
{
	m_scrollIndex = pos;
}

void MainWindow::on_playBtn_clicked(bool checked)
{
	if(checked){
		connect(m_timer,SIGNAL(timeout()),this,SLOT(slotPlay()));
		m_timer->start(200);
	}else{
		m_timer->stop();
		disconnect(m_timer,SIGNAL(timeout()),this,SLOT(slotPlay()));
	}
}

void MainWindow::on_actionOpen_triggered()
{
	//open file--farther dialog，name，path，filter
	QString s = QFileDialog::getOpenFileName(this,
											 "open file dialog",
											 "./",
											 "files(*)");
	ReadFile(s,m_headData,m_data);
	for(int i=0;i<m_headData.size();i++){
		ui->listWidget->addItem(m_headData.at(i));
	}
	//SetStatus();
}
