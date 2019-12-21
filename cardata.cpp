#include "cardata.h"
unsigned int DataIn::fd = 0;
void DataIn::GetData(MainWindow *ui,QVector<double> &xCoord,QVector<double> &yCoord)
{
	unsigned int time;
	int CarID;
	float x,y;
	float speedx,speedy;
	ui->SD_flags = false;
	unsigned int count(0);
	FILE *fp = fopen("/home/zqw/Qt/Qt_project/FLDB/CarData.txt","r");
	rewind(fp);
	while(count < fd)
	{
		fscanf(fp,"%d %d %f %f %f %f",&time,&CarID,&x,&y,&speedx,&speedy);
		++count;
	}
	while(fscanf(fp,"%d %d %f %f %f %f",&time,&CarID,&x,&y,&speedx,&speedy) != EOF)
	{
#ifdef UAVMethod
		if(ui->Source != NULL)
			if(ui->Source->GetCarID() == CarID)
				ui->SD_flags = true;
		if(ui->Dest != NULL)
			if(ui->Dest->GetCarID() == CarID)
				ui->SD_flags = true;
#endif
#ifdef UAVMethod2
		for(int i = 0;i < 3;++i){
			if(ui->_Source[i]->GetCarID() == CarID){
				ui->SD_flags = true;
				break;
			}
			if(ui->_Dest[i]->GetCarID() == CarID){
				ui->SD_flags = true;
				break;
			}
		}
#endif
		if(time > Ui::SimTime)
			break;
		if(static_cast<double>(time) == Ui::SimTime){
			for(std::vector<Node>::iterator iter = ui->SumNode.begin();iter != ui->SumNode.end();++iter){
				if(CarID == iter->GetCarID())
				{
					iter->SetX(x);
					iter->SetY(y);
					iter->SetspeedX(speedx);
					iter->SetspeedY(speedy);
					xCoord.push_back(x);
					yCoord.push_back(y);
				}
			}
			++fd;//Mark File Position
		}

	}
	fclose(fp);
	Ui::SimTime += 1;

}
