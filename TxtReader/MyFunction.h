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
