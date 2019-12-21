#include "utils.h"
double Dis(double x1,double y1,double x2,double y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}
double Dir_Theta(double X,double Y,double x,double y)
{
	double theta;
	if(X == x)
	{
		theta = atan((Y-y)/0.001);
		return theta;
	}
	theta = atan((Y-y)/(X-x));
	return theta;
}
