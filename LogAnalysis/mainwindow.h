#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void DrawLine();

	void GetConrolPoint(QVector<QPoint>& oriPoslist,
						QVector<QPointF> &resPoslist);
	void DrawCurve();

	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
private slots:
	void on_actionCurve_triggered();

private:
    Ui::MainWindow *ui;

	bool m_drawCurve;
	QPoint m_clickPos;
	QPoint m_movePos;
	QVector<QPoint > m_clickPosVec;

    QString m_allPos;
    QVector<QLineF> m_lines;
    QVector<QPointF> m_poss;
};

#endif // MAINWINDOW_H
