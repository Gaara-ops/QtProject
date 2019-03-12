#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "globeinclude.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_pushButton_clicked();
    void slotTimeOut();
private:
    Ui::MainWindow *ui;
	QTimer* m_timer;
};

#endif // MAINWINDOW_H
