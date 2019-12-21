#ifndef CMESSAGE_H
#define CMESSAGE_H
struct cMessage
{
	/* 源节点位置信息 */
	 double src_x,src_y;
	/* 目的节点位置信息 */
	 double dest_x,dest_y;
	/* 节点广播时间间隔  */
	 double BPIT;
	/* 节点移动速度 */
	 double speed;
	/* 节点通信范围 */
	 double radius;
	/* 节点名称 */
	char name[50];
	/* 位置信息 */
	double posx,posy;

};
#endif // CMESSAGE_H
