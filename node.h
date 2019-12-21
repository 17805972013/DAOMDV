#ifndef _NODE_
#define _NODE_
#include <vector>
#include <sstream>
#include "utils.h"
typedef std::vector<int> VInt;
typedef std::vector<double> VDouble;
typedef std::stringstream Int2Str;
class Table
{
public:
	friend class Node;
	friend class MainWindow;
	VInt NeighberID;
	VDouble NeighberX;
	VDouble NeighberY;
	VDouble NeighberSpeedx;
	VDouble NeighberSpeedy;

};
class Node
{
public:
	Node():UavID(0),CarID(0),x(0),y(0),speedx(0),speedy(0),radius(300){
	}
	~Node()
	{

	}
	virtual void SendBeacon();
	void SetUavID(int _UavID){UavID = _UavID;}
	void SetCarID(int _CarID){CarID = _CarID;}
	void SetX(double _X){x = _X;}
	void SetY(double _Y){y = _Y;}
	void SetspeedX(double sx){speedx = sx;}
	void SetspeedY(double sy){speedy = sy;}
	void SetRadius(double _radius){radius = _radius;}
	void SetNeighber(Table _Neighber){Neighber = _Neighber;}
	int GetUavID(){return UavID;}
	int GetCarID(){return CarID;}
	double GetX(){return x;}
	double GetY(){return y;}
	double GetSpeedx(){return speedx;}
	double GetSpeedy(){return speedy;}
	double GetRadius(){return radius;}
	Table& GetNeighber(){return Neighber;}
private:
	int UavID;
	int CarID;
	double x;
	double y;
	double speedx;
	double speedy;
	double radius;
	Table Neighber;
};
#endif
