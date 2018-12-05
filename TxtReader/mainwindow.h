#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <QTextEdit>
#include <QWheelEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void UpdateListWidget();
    void SaveStatus();
    void SetStatus();
	QString getPath();
	void closeEvent(QCloseEvent *e);
private slots:
    void on_upBtn_clicked();

    void on_downBtn_clicked();

    void on_listWidget_currentRowChanged(int currentRow);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_actionFont_triggered();

    void on_actionSaveStatus_triggered();

	void on_playBtn_clicked(bool checked);

	void on_actionOpen_triggered();

	void on_SpeedUpBtn_clicked();

	void on_SpeedDownBtn_clicked();

public slots:
	void slotUpdateSlide();
	void slotPlay();
	void slotSlideBarMoved(int pos);
	void slotScrollPosChange(int pos);
private:
    Ui::MainWindow *ui;
	QTimer* m_timer;
	int m_timeSpace;
    int m_currentRow;
    int m_scrollIndex;
	QString m_fileName;
    QList<QString> m_data;
	QList<QString> m_headData;
    QFont m_font;
};

#endif // MAINWINDOW_H
