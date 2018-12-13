#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyFunction.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	SetWidgetVisible(0);
	/**
	  set window to max
	this->setWindowState(Qt::WindowMaximized);*/
	m_timer = new QTimer(this);
	m_timeSpace = 200;
    m_currentRow = 0;
    m_scrollIndex = 0;
	m_fileName = "";
	connect(ui->textEdit->verticalScrollBar(),SIGNAL(sliderMoved(int)),
			this,SLOT(slotSlideBarMoved(int)));
	connect(ui->textEdit->verticalScrollBar(),SIGNAL(valueChanged(int)),
			this,SLOT(slotScrollPosChange(int)));
}

MainWindow::~MainWindow()
{
	if(m_timer != NULL){
		m_timer->stop();
		delete m_timer;
		m_timer = NULL;
	}
	delete ui;
}

void MainWindow::SetWidgetVisible(bool isvisible)
{
	ui->widget->setVisible(isvisible);
	ui->listWidget->setVisible(isvisible);
	ui->textEdit->setVisible(isvisible);
}

void MainWindow::UpdateListWidget()
{
    ui->listWidget->setCurrentRow(m_currentRow);
    ui->textEdit->setText(m_data.at(m_currentRow));
}

void MainWindow::SaveStatus()
{
	if(m_fileName == ""){
		return;
	}
    QString fontname = m_font.family();
    int pointsize = m_font.pointSize();
	int weight = m_font.weight();
	int posx = this->geometry().x();
	int posy = this->geometry().y();
	int width = this->geometry().width();
	int height = this->geometry().height();
	m_scrollIndex = ui->textEdit->verticalScrollBar()->sliderPosition();
	QString configname = getPath();
	SaveJosn(configname,fontname,pointsize,weight,posx,posy,
			 width,height,m_currentRow,m_scrollIndex);
}

void MainWindow::SetStatus()
{
	QString configname = getPath();
    QString fontname;
    int pointsize = 0;
    int weight = 0;
	int posx=0,posy=0,width=0,height=0;
	bool succeed = ParseJson(configname,fontname,pointsize,weight,posx,posy,
			  width,height,m_currentRow,m_scrollIndex);
	if(!succeed)
		return;
	m_font.setFamily(fontname.toUtf8());
	m_font.setPointSize(pointsize);
	m_font.setWeight(weight);

	this->setGeometry(posx,posy,width,height);
    ui->listWidget->setCurrentRow(m_currentRow);
    ui->textEdit->setText(m_data.at(m_currentRow));
    ui->textEdit->setFont(m_font);
	ui->textEdit->update();
	connect(m_timer,SIGNAL(timeout()),this,SLOT(slotUpdateSlide()));
	m_timer->start(500);
}

QString MainWindow::getPath()
{
	QString filename = m_fileName.split("/").last();
	QString prefixpath = "./Configure/";
	CreateDir(prefixpath);
	QString configname = prefixpath +filename.split(".").first() +
											"configure.txt";
	return configname;
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
	if(m_currentRow <=0){
		m_currentRow = 0;
	}
	m_scrollIndex = 0;
    UpdateListWidget();
}

void MainWindow::on_downBtn_clicked()
{
    m_currentRow++;
	if(m_currentRow >= m_headData.size()){
		m_currentRow = m_headData.size()-1;
	}
	m_scrollIndex = 0;
    UpdateListWidget();
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
	m_currentRow = currentRow;
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
	m_scrollIndex = 0;
    UpdateListWidget();
}

void MainWindow::on_actionFont_triggered()
{
    //打开字体设置对话框，返回设置的字体
    bool ok;
    m_font = QFontDialog::getFont(&ok,m_font);
    if (ok)
    {
		if(ui->textEdit->isVisible()){
			ui->textEdit->setFont(m_font);
			ui->textEdit->update();
		}
    }
}

void MainWindow::on_actionSaveStatus_triggered()
{
	SaveStatus();
}

void MainWindow::slotUpdateSlide()
{
	if(ui->textEdit->verticalScrollBar()->maximum() == 0){
		return;
	}
	int maxindex = ui->textEdit->verticalScrollBar()->maximum();
	if(maxindex<m_scrollIndex){
		return;
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

void MainWindow::slotScrollPosChange(int pos)
{
	m_scrollIndex = pos;
}

void MainWindow::on_playBtn_clicked(bool checked)
{
	if(checked){
		connect(m_timer,SIGNAL(timeout()),this,SLOT(slotPlay()));
		m_timer->start(m_timeSpace);
	}else{
		m_timer->stop();
		disconnect(m_timer,SIGNAL(timeout()),this,SLOT(slotPlay()));
	}
}

void MainWindow::on_actionOpen_triggered()
{
	//open file--farther dialog，name，path，filter
	m_fileName = QFileDialog::getOpenFileName(this,
											 "open file dialog",
											 "./",
											 "files(*)");
	if(m_fileName == ""){

		return;
	}
	ReadFile(m_fileName,m_headData,m_data);
	for(int i=0;i<m_headData.size();i++){
		ui->listWidget->addItem(m_headData.at(i));
	}
	SetWidgetVisible(1);
	SetStatus();
}

void MainWindow::on_SpeedUpBtn_clicked()
{
	if(!m_timer->isActive()){
		return;
	}
	m_timer->stop();
	m_timeSpace /= 2;
	if(m_timeSpace <= 10)
		m_timeSpace = 10;
	m_timer->start(m_timeSpace);
}

void MainWindow::on_SpeedDownBtn_clicked()
{
	if(!m_timer->isActive()){
		return;
	}
	m_timer->stop();
	m_timeSpace *= 2;
	if(m_timeSpace >= 800)
		m_timeSpace = 800;
	m_timer->start(m_timeSpace);
}
