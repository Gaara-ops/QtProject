#ifndef MYFUNCTION_H
#define MYFUNCTION_H

#include <QMainWindow>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QListWidgetItem>
#include <QList>
#include <QFontDialog>
#include <QScrollBar>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonParseError>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTimer>
#include <QFileDialog>
#include <QDir>
void CreateDir(QString dirname){
	QDir tempDir(dirname);
	if(!tempDir.exists()){
		qDebug() << dirname;
		tempDir.mkpath(dirname);
	}
}

void ParseJson(QString path,QString& fontname,int& pointsize,int& weight,
			   int& posx,int& posy,int& width,int& height,
			   int& currentRow,int& scrollIndex){
	QFile file(path);
	if(!file.open(QIODevice::ReadOnly))
	{
		 return;
	}
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
					currentRow = value.toInt();
				}
				if (object.contains("scrollindex")) {
					QJsonValue value = object.value("scrollindex");
					scrollIndex = value.toInt();
				}
				if (object.contains("posx")) {
					QJsonValue value = object.value("posx");
					posx = value.toInt();
				}
				if (object.contains("posy")) {
					QJsonValue value = object.value("posy");
					posy = value.toInt();
				}
				if (object.contains("width")) {
					QJsonValue value = object.value("width");
					width = value.toInt();
				}
				if (object.contains("height")) {
					QJsonValue value = object.value("height");
					height = value.toInt();
				}
			}
		}
	file.close();
}

void ReadFile(QString filename,QList<QString>& headdata,QList<QString>& data){
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly))
	{
		 return;
	}
	QTextStream in(&file);
	in.setCodec(QTextCodec::codecForName("UTF-8"));
	QString aGra="";
	while(!in.atEnd()){
		QString oneline = in.readLine();
		if(oneline.isEmpty() || oneline.isNull()){
			continue;
		}
		if(!oneline.at(0).isSpace()){
			headdata.append(oneline);
			if(aGra != ""){
				data.append(aGra);
				aGra = "";
			}
		}else{
			aGra += oneline + "\n";
		}
	}
	file.close();
}

#endif // MYFUNCTION_H
