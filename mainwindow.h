#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QCustomPlot/qcustomplot.h>
#include <time.h>
#include "node.h"
//#define SimTest
#define SimReal
//#define UAVMethod //One transfer route
#define UAVMethod2 //Many transfer route
namespace Ui {
class MainWindow;
class UAVSchedule;
class DataIn;
static double SimTime = 1;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	friend class UAVSchedule;
	friend class DataIn;
	//static unsigned long long SimTime;
	void initplot();
	void SetUavColor(int sel)
	{
		switch(sel)
		{
		case 0:
			solidPen.setColor(Qt::darkCyan);
			break;
		case 1:
			solidPen.setColor(Qt::darkRed);
			break;
		case 2:
			solidPen.setColor(Qt::darkGreen);
			break;
		default:
			solidPen.setColor(Qt::cyan);
		}
	}

	~MainWindow();
private slots:
	void UpdatePosition();
	void pauseEvent();
	void continueEvent();
private:
	Ui::MainWindow *ui;
	QTimer eventTimer;
	QPen solidPen;
	std::vector<Node> SumNode;
	/* UAV  */
	int running;
	Node *UAV;
	Node *Source;
	Node *Dest;
	/* Judge Source or Dest is exist whethere or not? */
	bool SD_flags;
	QCPItemText *Text_UAV[3];
	QCPItemEllipse *Circle_UAV[3];
	/* Source and Destination Node */
	QCPItemText *Text_S;
	QCPItemText *Text_D;
	/* Many Source and Destination */
	Node *_Source[3];
	Node *_Dest[3];
	QCPItemText *_Text_S[3];
	QCPItemText *_Text_D[3];


};
class UAVSchedule
{
public:
/*  One transfer route Schedule */
void Schedule(MainWindow *ui,QVector<double> &xCoord,QVector<double> &yCoord);
/* Many transfer route Schedule */
void MRSchedule(MainWindow *ui,QVector<double> &xCoord,QVector<double> &yCoord);
protected:
private:
};
class DataIn
{
public:
	static unsigned int fd;
	DataIn(){
		//qDebug().nospace()<<"Data Inputing...";
	}
	void GetData(MainWindow *ui,QVector<double> &xCoord,QVector<double> &yCoord);
private:
protected:
};
#endif // MAINWINDOW_H
