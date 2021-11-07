#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<unordered_map>
#include<algorithm>
#include<cmath>
#include<fstream>
#include<ctime>
#include <iomanip>


//航班结构体
class Flight
{
public:
	struct tm Dt;			//航班的出发时间
	char Ds[3];				//航班的出发机场
	struct tm At;			//航班的到达时间
	char As[3];				//航班的到达机场
	int fIdx;				//航班编号

	struct tm dutyBeg;		//当前航班所属的执勤的开始时间
	double dutyTime;		//当前航班所属的执勤的总时长
	double dutyFlyTime;		//当前航班所属的执勤的飞行时长
	double ringTime;		//当前航班所属的单个任务环时长
	double tolRingTime;		//当前航班所属的所有任务环时长
};

//机组人员结构体
class Crew
{
public:
	int Cp;				//员工是否可以担任正机长，0-1
	int Fo;				//员工是否可以担任副机长，0-1
	int Dh;				//员工是否允许乘机，0-1
	char Bs[3];			//员工的基地
	double Dc;			//员工的单位小时执勤成本
	double Pc;			//员工的单位小时任务环成本
	int cIdx;			//员工编号
};

//执勤结构体
class Duty
{
public:
	std::vector<Flight> dutys;

};

//航班环结构体
class FltRing
{
public:
	std::vector<Flight> flts;					//航班集合
	std::vector<std::vector<Crew>> crws;		//机组人员集合	
	std::vector<bool> svRec;					//记录机组人员在每个航班是否服务1，还是乘机0
	int dutyNum;								//记录总的执勤次数
	double tolDutyTime;							//总执勤时长
	double tolFlyTime;							//总飞行时长
	std::vector<double> dutyTimeRec;					//记录所有次执勤的时长
	std::vector<double> flyTimeRec;					//记录所有次执勤的飞行时长
	int ringNum;								//记录总的任务环数量

	double AccuRingTime;						//记录所有任务环的时间
	std::vector<double> betweenRings;			//记录所有任务环之间的休假时间
	std::unordered_map<int, int> datRing;			//记录不同天的任务环数量


	int lastPortIdx = -1;						//记录最后一个可以返回基地的航班在flts中的索引
	Flight lastFlight;							//记录最后一个可以返回基地的航班
};

//解结构体
class Solution
{
public:
	std::vector<FltRing> fRings;	//所有的航班环
	std::vector<Flight> unDone;	//所有未服务的航班集合
	std::vector<Flight> Done;	//所有已经服务的航班集合

	Solution(std::vector<Flight>& SetF)
	{
		unDone = SetF;
	}
};

extern std::vector<Flight> SetF;	//所有航班的集合
extern std::vector<Crew> SetC;		//所有员工的集合


//读取数据
void InputData();

//贪婪构造初始解
void InitCrt(Solution &initSol);

//计算每个机场到其他机场之间的航班数量
void FillFltNum();

//输出结果
void Output(Solution & Sol);