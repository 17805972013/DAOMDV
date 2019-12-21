#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),running(0),Source(NULL),Dest(NULL),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setGeometry(400, 100, 600, 610); // 1120
	/* 产生随机粒子 */
	srand(time(NULL));
	/* 初始化界面 */
	initplot();
	/* SLOT槽中的函数必须被定义在private slots:下才能调用 */
	QMenu *editMenu = ui->menuBar->addMenu(tr("edit"));
	QAction *pauseAction = editMenu->addAction(tr("Pause"), this, SLOT(pauseEvent()));
	QAction *continueAction = editMenu->addAction(tr("Continue"), this, SLOT(continueEvent()));
	ui->mainToolBar->addAction(pauseAction);
	ui->mainToolBar->addAction(continueAction);
	connect(&eventTimer, SIGNAL(timeout()), this, SLOT(UpdatePosition()));
	eventTimer.start(200);

}
void MainWindow::initplot()
{

	/* 对点参数的设置，实线蓝色 */
	QPen pen(Qt::SolidLine);
	pen.setColor(Qt::blue);
	/* 将点添加到画板上去 */
	QCPGraph *graph0 = ui->customPlot->addGraph();
	graph0->setPen(pen);
	graph0->setLineStyle(QCPGraph::lsNone);
#ifdef SimTest
	graph0->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));
#endif
#ifdef SimReal
	graph0->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
#endif

	/* car and uav初始位置信息 */
	QVector<double> xCoord,yCoord;
#ifdef SimReal
	for(int i = 0;i < 150;++i){
		Node T;
		T.SetCarID(i+1);
		T.SetX(0);
		T.SetY(0);
		T.SetspeedX(0);
		T.SetspeedY(0);
		SumNode.push_back(T);
		xCoord.push_back(0);
		yCoord.push_back(0);
	}
#endif
#ifdef SimTest
	for(int i = 0;i < 1500;++i){
		double x = rand()%(5000+1);
		double y = rand()%(5000+1);
		Node T;
		T.SetCarID(i+1);
		T.SetX(x);
		T.SetY(y);
		T.SetspeedX(5);
		T.SetspeedY(5);
		SumNode.push_back(T);
		xCoord.push_back(x);
		yCoord.push_back(y);
	}
#endif

#ifdef UAVMethod
	Source = &SumNode[1];
	Dest = &SumNode[2];
#endif
#ifdef UAVMethod2
	for(int i = 0;i < 3;++i){
	_Source[i] = &SumNode[3+i];
	_Dest[i] = &SumNode[6+i];
	}
#endif
	/* set UAV Parameter */
	Node _UAV[3];
	int MAXCarID = SumNode.size();
	//First Address
	for(int i = 0;i < 3;++i){
		_UAV[i].SetUavID(i+1);
		_UAV[i].SetCarID(MAXCarID+i);
		_UAV[i].SetRadius(500);
		_UAV[i].SetX(1250);
		_UAV[i].SetY(1250);
		SumNode.push_back(_UAV[i]);
		xCoord.push_back(_UAV[i].GetX());
		yCoord.push_back(_UAV[i].GetY());

	}
	UAV = &SumNode[MAXCarID];
	char Uavname[2];
	for(int i = 0;i < 3;++i){
	Uavname[0] = 'U';
	Uavname[1] = '0'+i;
	UAV[i].SetRadius(500);
	Text_UAV[i] = new QCPItemText(ui->customPlot);
	Text_UAV[i]->setPositionAlignment(Qt::AlignmentFlag::AlignCenter);
	Text_UAV[i]->setText(Uavname);
	Text_UAV[i]->position->setCoords(UAV[i].GetX(), UAV[i].GetY());
	Text_UAV[i]->setColor(Qt::darkGreen);
	Circle_UAV[i] = new QCPItemEllipse(ui->customPlot);
	Circle_UAV[i]->topLeft->setCoords(UAV[i].GetX() - UAV[i].GetRadius(), UAV[i].GetY() + UAV[i].GetRadius());
	Circle_UAV[i]->bottomRight->setCoords(UAV[i].GetX() + UAV[i].GetRadius(), UAV[i].GetY() - UAV[i].GetRadius());
	Circle_UAV[i]->setPen(solidPen);
	SetUavColor(i);
	}

	/* Set Source and Destination Node */
#ifdef UAVMethod
	Text_S = new QCPItemText(ui->customPlot);
	Text_D = new QCPItemText(ui->customPlot);
	Text_S->setText("S");
	Text_S->setColor(Qt::darkRed);
	Text_S->position->setCoords(Source->GetX(),Source->GetY());
	Text_D->setText("D");
	Text_D->setColor(Qt::darkRed);
	Text_D->position->setCoords(Dest->GetX(),Dest->GetY());
#endif
#ifdef UAVMethod2
	char Name[2];
	for(int i = 0;i < 3;++i){
		Name[0] = 'S';
		Name[1] = '0'+i;
		_Text_S[i] = new QCPItemText(ui->customPlot);
		_Text_D[i] = new QCPItemText(ui->customPlot);
		_Text_S[i]->setText(Name);
		_Text_S[i]->setColor(Qt::darkRed);
		_Text_S[i]->position->setCoords(_Source[i]->GetX(),_Source[i]->GetY());
		Name[0] = 'D';
		_Text_D[i]->setText(Name);
		_Text_D[i]->setColor(Qt::darkRed);
		_Text_D[i]->position->setCoords(_Dest[i]->GetX(),_Dest[i]->GetY());

	}
#endif
	/* 用于设置x轴与y轴的刻度标签 */
	ui->customPlot->xAxis2->setVisible(true);
	ui->customPlot->xAxis2->setTickLabels(false);
	ui->customPlot->yAxis2->setVisible(true);
	ui->customPlot->yAxis2->setTickLabels(false);

	/* 不加这两句话则当鼠标滚轮时,x轴上侧与y轴右侧范围不会变化 */
	connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

	/* 设置x,y轴的范围 */
#ifdef SimTest
	ui->customPlot->xAxis->setRange(0, 5000);
	ui->customPlot->yAxis->setRange(0, 5000);
#endif
#ifdef SimReal
	DataIn _datain;
   _datain.GetData(this,xCoord,yCoord);
	ui->customPlot->xAxis->setRange(0, 2000);
	ui->customPlot->yAxis->setRange(0, 2000);
#endif

	/* 利用鼠标滚轮自动调整大小 */
	ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	graph0->setData(xCoord,yCoord);
	ui->customPlot->replot();
}
void MainWindow::UpdatePosition()
{
	//qDebug().nospace()<<Source->GetX()<<"    "<<Source->GetY();
	//qDebug().nospace()<<Dest->GetX()<<"    "<<Dest->GetY();
	if(running == 1)
		return;
#ifdef SimReal
#ifdef UAVMethod
	if(!SD_flags)
	{
		Source->SetX(1000);
		Source->SetY(1000);
		Source->SetRadius(1000);
		Dest->SetX(1000);
		Dest->SetY(1000);
		Dest->SetRadius(1000);
	}
#endif
#ifdef UAVMethod2
	if(!SD_flags)
	{
		for(int i = 0;i < 3;++i)
		{
			_Source[i]->SetX(1000);
			_Source[i]->SetY(1000);
			_Source[i]->SetRadius(1000);
			_Dest[i]->SetX(1000);
			_Dest[i]->SetY(1000);
			_Dest[i]->SetRadius(1000);
		}
	}
#endif
#endif
	ui->customPlot->clearGraphs();
	ui->customPlot->clearItems();
	/* 对点参数的设置，实线蓝色 */
	QPen pen(Qt::SolidLine);
	pen.setColor(Qt::blue);
	/* 将点添加到画板上去 */
	QCPGraph *graph0 = ui->customPlot->addGraph();
	graph0->setPen(pen);
	graph0->setLineStyle(QCPGraph::lsNone);
#ifdef SimTest
	graph0->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));
#endif
#ifdef SimReal
	graph0->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
#endif
	/*
		UAV schedule
	*/
	QVector<double> xCoord,yCoord;
	UAVSchedule Sch;
#ifdef UAVMethod
	Sch.Schedule(this,xCoord,yCoord);
	for(int i = 0;i < 3;++i)
		qDebug().nospace()<<"UAV["<<i+1<<"]:"<<UAV[i].GetX()<<","<<UAV[i].GetY();
#endif
#ifdef UAVMethod2
	Sch.MRSchedule(this,xCoord,yCoord);
	for(int i = 0;i < 3;++i)
		qDebug().nospace()<<"UAV["<<i+1<<"]:"<<UAV[i].GetX()<<","<<UAV[i].GetY();
#endif
#ifdef SimReal
		DataIn _datain;
		_datain.GetData(this,xCoord,yCoord);
#endif
	for(std::vector<Node>::iterator iter = SumNode.begin();iter != SumNode.end();++iter)
	{
#ifdef SimTest
		double x = iter->GetX();
		double y = iter->GetY();
		double speedx = iter->GetSpeedx();
		double speedy = iter->GetSpeedy();

		if(!iter->GetUavID()){

		/*
			Dest:X,Y
			X = x + rand()%21 - 10;
			Y = y + rand()%21 - 10;
		*/
		//beyond area
		if(x>5000){
			x = 0;
			y = rand()%(5000+1);
		}
		if(y>5000){
			x = rand()%(5000+1);
			y = 0;
		}
		if(x<0){
			x = 5000;
			y = rand()%(5000+1);
		}
		if(y<0){
			x = rand()%(5000+1);
			y = 5000;
		}
		iter->SetX(x+speedx);
		iter->SetY(y+speedy);
		double X = x+rand()%10-10;
		double Y = y+rand()%10-10;
		iter->SetspeedX((X-x)*3);
		iter->SetspeedY((Y-y)*3);
		xCoord.push_back(x);
		yCoord.push_back(y);
		}
#endif

	/************///Set Neighber Node
	Table &_t = iter->GetNeighber();
	_t.NeighberID.clear();
	_t.NeighberSpeedx.clear();
	_t.NeighberSpeedy.clear();
	_t.NeighberX.clear();
	_t.NeighberY.clear();
	for(std::vector<Node>::iterator _iter = SumNode.begin();_iter!=SumNode.end();++_iter )
	{
		if(Dis(iter->GetX(),iter->GetY(),_iter->GetX(),_iter->GetY()) <= iter->GetRadius() && iter!=_iter)
		{
			_t.NeighberID.push_back(_iter->GetCarID());
			_t.NeighberSpeedx.push_back(_iter->GetSpeedx());
			_t.NeighberSpeedy.push_back(_iter->GetSpeedy());
			_t.NeighberX.push_back(_iter->GetX());
			_t.NeighberY.push_back(_iter->GetY());

		}
	}
	/********************************/
	}

	/*for(int i = 0;i < 3;++i)
	{
		qDebug().nospace()<<"UAV"<<i+1<<"Neighber:";
		VInt T(UAV[i].GetNeighber().NeighberID);
		for(std::vector<int>::iterator iter = T.begin();iter!=T.end();++iter)
			qDebug().nospace()<<*iter;
	}
	/*
		Neighber Nodde
		qDebug().nospace()<<"-----------------";
		for(int i = 0;i < SumNode.size();++i){
			VInt NID(SumNode[i].GetNeighber()->NeighberID);
			qDebug().nospace()<<"Node["<<i<<"]Neighber:";
			for(int k = 0;k < NID.size();++k)
				qDebug().nospace()<<NID[k];
		}

	*/
	/* UAV apart */
	char Uavname[2];
	for(int i = 0;i < 3;++i){
	Uavname[0] = 'U';
	Uavname[1] = '0'+i;
	UAV[i].SetRadius(500);
	Text_UAV[i] = new QCPItemText(ui->customPlot);
	Text_UAV[i]->setPositionAlignment(Qt::AlignmentFlag::AlignCenter);
	Text_UAV[i]->setText(Uavname);
	Text_UAV[i]->position->setCoords(UAV[i].GetX(), UAV[i].GetY());
	Text_UAV[i]->setColor(Qt::darkGreen);
	Circle_UAV[i] = new QCPItemEllipse(ui->customPlot);
	Circle_UAV[i]->topLeft->setCoords(UAV[i].GetX() - UAV[i].GetRadius(), UAV[i].GetY() + UAV[i].GetRadius());
	Circle_UAV[i]->bottomRight->setCoords(UAV[i].GetX() + UAV[i].GetRadius(), UAV[i].GetY() - UAV[i].GetRadius());
	Circle_UAV[i]->setPen(solidPen);
	SetUavColor(i);
	}
	/* Set Source and Destination Node */
#ifdef UAVMethod
	Text_S = new QCPItemText(ui->customPlot);
	Text_D = new QCPItemText(ui->customPlot);
	Text_S->setText("S");
	Text_S->setColor(Qt::darkRed);
	Text_S->position->setCoords(Source->GetX(),Source->GetY());
	Text_D->setText("D");
	Text_D->setColor(Qt::darkRed);
	Text_D->position->setCoords(Dest->GetX(),Dest->GetY());
#endif
#ifdef UAVMethod2
	char Name[2];
	for(int i = 0;i < 3;++i){
		Name[1] = '0'+i;
		Name[0] = 'S';
		_Text_S[i] = new QCPItemText(ui->customPlot);
		_Text_S[i]->setText(Name);
		_Text_S[i]->setColor(Qt::darkRed);
		_Text_S[i]->position->setCoords(_Source[i]->GetX(),_Source[i]->GetY());
		Name[0] = 'D';
		_Text_D[i] = new QCPItemText(ui->customPlot);
		_Text_D[i]->setText(Name);
		_Text_D[i]->setColor(Qt::darkRed);
		_Text_D[i]->position->setCoords(_Dest[i]->GetX(),_Dest[i]->GetY());
	}
#endif
	graph0->setData(xCoord,yCoord);
	ui->customPlot->replot();
}
void MainWindow::pauseEvent()
{
	running = 1;
}
void MainWindow::continueEvent()
{
	running = 0;
}
MainWindow::~MainWindow()
{
	delete ui;
}
void UAVSchedule::Schedule(MainWindow *ui,QVector<double> &xCoord,QVector<double> &yCoord)
{
	Node *Source = ui->Source;
	Node *Dest = ui->Dest;
	Node *UAV = ui->UAV;
	double _x[3];
	double _y[3];
	double _X[3],_Y[3];
	double dis[3];
	for(int i = 0;i < 3;++i){
		_x[i] = UAV[i].GetX();
		_y[i] = UAV[i].GetY();
	}
	_X[0] = (_x[0]+Source->GetX()+Dest->GetX())/3;
	_Y[0] = (_y[0]+Source->GetY()+Dest->GetY())/3;
	_X[1] = (_x[1]+Source->GetX()+_x[0])/3;
	_Y[1] = (_y[1]+Source->GetY()+_y[0])/3;
	_X[2] = (_x[2]+Dest->GetX()+_x[0])/3;
	_Y[2] = (_y[2]+Dest->GetY()+_y[0])/3;
	for(int i = 0;i < 3;++i)
	{
		dis[i] = Dis(_X[i],_Y[i],_x[i],_y[i]);
		if(dis[i] != 0){
		UAV[i].SetspeedX(45*(_X[i]-_x[i])/dis[i]);
		UAV[i].SetspeedY(45*(_Y[i]-_y[i])/dis[i]);
		}
		else
		{
			UAV[i].SetspeedX(0);
			UAV[i].SetspeedY(0);
		}
		double sx = UAV[i].GetSpeedx();
		double sy = UAV[i].GetSpeedy();
		if(fabs(_X[i]-_x[i]-sx) <= 45)
			UAV[i].SetX(_X[i]);
		else
			UAV[i].SetX(_x[i]+sx);
		if(fabs(_Y[i]-_y[i]-sy) <= 45)
			UAV[i].SetY(_Y[i]);
		else
			UAV[i].SetY(_y[i]+sy);
		xCoord.push_back(_x[i]);
		yCoord.push_back(_y[i]);
	}
}
void UAVSchedule::MRSchedule(MainWindow *ui, QVector<double> &xCoord, QVector<double> &yCoord)
{
	int RouteNum = 3;
	int UAVNum = 3;
	Node *Source[RouteNum];
	Node *Dest[RouteNum];
	Node *UAV = ui->UAV;
	for(int i = 0;i < RouteNum;++i){
		Source[i] = ui->_Source[i];
		Dest[i] = ui->_Dest[i];
	}
	double x[UAVNum];
	double y[UAVNum];
	std::vector<double> X[UAVNum];
	std::vector<double> Y[UAVNum];
	double SumX[UAVNum]={0.0};
	double SumY[UAVNum]={0.0};
	for(int i = 0;i < UAVNum;++i){
		x[i] = UAV[i].GetX();
		y[i] = UAV[i].GetY();
	}
	/*
		Set Route Weight
		0 <= Weight[i] <= 1 ,i = 1,2,3
	*/
	double Weight[RouteNum];
	double SumWeight[UAVNum] = {0.0};
	Weight[0] = 1.0/3;
	Weight[1] = 1.0/3;
	Weight[2] = 1.0/3;
	for(int j = 0;j < RouteNum;++j){
		X[0].push_back(Weight[j]*(Source[j]->GetX()+Dest[j]->GetX())/2);
		Y[0].push_back(Weight[j]*(Source[j]->GetY()+Dest[j]->GetY())/2);
		SumWeight[0] += Weight[j];
		if(Dis(Source[j]->GetX(),Source[j]->GetY(),x[1],y[1]) <= Dis(Source[j]->GetX(),Source[j]->GetY(),x[2],y[2]))
		{

			X[1].push_back(Weight[j]*Source[j]->GetX());
			Y[1].push_back(Weight[j]*Source[j]->GetY());
			SumWeight[1] += Weight[j];
		}
		else
		{
			X[2].push_back(Weight[j]*Source[j]->GetX());
			Y[2].push_back(Weight[j]*Source[j]->GetY());
			SumWeight[2] += Weight[j];
		}
		if(Dis(Dest[j]->GetX(),Dest[j]->GetY(),x[1],y[1]) <= Dis(Dest[j]->GetX(),Dest[j]->GetY(),x[2],y[2]))
		{
			X[1].push_back(Weight[j]*Dest[j]->GetX());
			Y[1].push_back(Weight[j]*Dest[j]->GetY());
			SumWeight[1] += Weight[j];
		}
		else
		{
			X[2].push_back(Weight[j]*Dest[j]->GetX());
			Y[2].push_back(Weight[j]*Dest[j]->GetY());
			SumWeight[2] += Weight[j];
		}
	}

	double CBpX[RouteNum] = {0};
	double CBpY[RouteNum] = {0};
	for(int j = 0;j < UAVNum;++j)
	{
		if(X[j].size()){
			int len = X[j].size();
		for(int k = 0;k < len;++k){
			CBpX[j] += X[j][k]/SumWeight[j];
			CBpY[j] += Y[j][k]/SumWeight[j];
		}
		}
		else
		{
			CBpX[j] = 1000;
			CBpY[j] = 1000;
		}
	}

	for(int i = 0;i < UAVNum;++i){
		double dis = Dis(CBpX[i],CBpY[i],x[i],y[i]);
		if(dis != 0)
		{
			SumX[i] = 32*(CBpX[i]-x[i])/dis;
			SumY[i] = 32*(CBpY[i]-y[i])/dis;
		}
		else
		{
			SumX[i] = 0;
			SumY[i] = 0;
		}
		UAV[i].SetspeedX(SumX[i]);
		UAV[i].SetspeedY(SumY[i]);
		double sx = UAV[i].GetSpeedx();
		double sy = UAV[i].GetSpeedy();
		if(fabs(CBpX[i]-x[i]-sx) <= 45)
			UAV[i].SetX(CBpX[i]);
		else
			UAV[i].SetX(x[i]+sx);
		if(fabs(CBpY[i]-y[i]-sy) <= 45)
			UAV[i].SetY(CBpY[i]);
		else
			UAV[i].SetY(y[i]+sy);
		xCoord.push_back(x[i]);
		yCoord.push_back(y[i]);
	}
}
