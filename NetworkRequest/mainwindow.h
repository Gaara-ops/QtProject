#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "networkope.h"

#include "udpserver.h"
#include "udpclient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void slotGetRequestData(QString resdata);
private slots:
    void on_StartBtn_clicked();
    void on_ClearBtn_clicked();
    void on_actionUDPClient_triggered();

    void on_actionUDPServer_triggered();

private:
    Ui::MainWindow *ui;
    NetworkOpe* m_netOpe;
    UDPClient* m_udpClient;
    UDPServer* m_udpServer;
};

#endif // MAINWINDOW_H
