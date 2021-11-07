#define _CRT_SECURE_NO_WARNINGS
#include"header4.h"

using namespace std;


vector<int> P;			//所有机场的位置集合
int N = 2;				//每个航班总共需要的机组人数
int Nc = 1;				//每个航班总共需要的正机长人数
int Nf = 1;				//每个航班总共共需要的副机长人数
int FltNum;				//全部的航班数
int crwNum;				//全部的机组人员数

double MinCT = 40;		//航段之间最小的连接时间40min
double MaxBlk = 600;	//一次执勤飞行时长最多不超过600min
double MaxDP = 720;		//执勤时长最多不超过720min
double MinRest = 660;	//相邻执勤之间的休息时间不少于660min
int MaxDH = 5;			//每趟航班最多的乘机人数 5人
double MaxTAFB = 14400;	//排班周期单个机组人员的任务环总时长不超过14400
int MaxSuccOn = 4;		//连续执勤天数不超过 4天
int MinVacDay = 2;		//相邻两个任务环之间至少休息 2天


vector<Flight> SetF;	//所有航班的集合
vector<Crew> SetC;		//所有员工的集合
unordered_map<string, int> g_FltNum;	//每个机场到其他机场之间的航班数量
clock_t sTime = clock();

ofstream output("E:/桌面文件/研究生/华为数学建模竞赛/F/newOut/B-3-out.txt");

//读取数据
void InputData()
{
	ifstream input1("E:/桌面文件/研究生/华为数学建模竞赛/F/B-Flight.txt");
	ifstream input2("E:/桌面文件/研究生/华为数学建模竞赛/F/B-Crew.txt");
	if (!input1 || !input2 || !output)
	{
		cout << "打开输入/输出文件失败！" << endl;
		exit(-1);
	}

	input1 >> FltNum;
	input2 >> crwNum;
	SetF.resize(FltNum, Flight());
	SetC.resize(crwNum, Crew());

	for (int i = 0; i < FltNum; ++i)
	{//读入机场数据
		tm tempT;
		input1 >> tempT.tm_mon >> tempT.tm_mday >> tempT.tm_year >> tempT.tm_hour >> tempT.tm_min;
		SetF[i].Dt = tempT;
		SetF[i].Dt.tm_sec = 0;
		SetF[i].Dt.tm_year -= 1900;
		SetF[i].Dt.tm_mon -= 1;
		SetF[i].Dt.tm_isdst = -1;
		input1 >> SetF[i].Ds;
		input1 >> tempT.tm_mon >> tempT.tm_mday >> tempT.tm_year >> tempT.tm_hour >> tempT.tm_min;
		SetF[i].At = tempT;
		SetF[i].At.tm_sec = 0;
		SetF[i].At.tm_year -= 1900;
		SetF[i].At.tm_mon -= 1;
		SetF[i].At.tm_isdst = -1;
		input1 >> SetF[i].As;
		SetF[i].fIdx = i;
	}

	for (int i = 0; i < crwNum; ++i)
	{
		input2 >> SetC[i].Cp >> SetC[i].Fo >> SetC[i].Dh >> SetC[i].Bs >> SetC[i].Dc >> SetC[i].Pc;
		SetC[i].cIdx = i;
	}

	input1.close();
	input2.close();
}

//计算每个机场到其他机场之间的航班数量
void FillFltNum()
{
	for (int i = 0; i < FltNum; ++i)
	{
		g_FltNum[SetF[i].Ds] += 1;
		g_FltNum[SetF[i].As] += 1;

	}
}

//判断两个航班的离开时间是否在同一天
bool JgeSameDay(Flight &f1, Flight &f2)
{
	if (f1.At.tm_year != f2.At.tm_year)
		return false;
	else if (f1.At.tm_mon != f2.At.tm_mon)
		return false;
	else if (f1.At.tm_mday != f2.At.tm_mday)
		return false;
	else
		return true;//同一年，同一月，同一天
}

//计算相邻两个航班直接的出发时间和离开时间之差,f1为前一个航班，f2为后一个航班
double CalDiffTime(Flight &f1, Flight &f2)
{
	//时间加减
	time_t t2 = mktime(&f2.Dt);
	time_t t1 = mktime(&f1.At);

	double t = difftime(t2, t1);//double difftime( time_t timeEnd, time_t timeStart );
	return t/60;
}

//计算某个航班的飞行时间
double CalFltTime(Flight &f)
{
	//时间加减
	time_t t2 = mktime(&f.At);
	time_t t1 = mktime(&f.Dt);

	double t = difftime(t2, t1);//double difftime( time_t timeEnd, time_t timeStart );
	return t/60;
}

//寻找最低资格配置机组人员组合
bool FindCrewPair(vector<Crew> &qulified, vector<Crew> &remainCrews)
{
	if (remainCrews.size() < N) return false;

	bool CpFlag = false,	//标记是否有正机长
		FoFlag = false;		//标记是否有副机长
	for (int i = 0; i <(int)remainCrews.size(); ++i)
	{
		if (remainCrews[i].Cp && !remainCrews[i].Fo && !CpFlag)
		{//先找一个没有副机长替补资格的正机长
			CpFlag = true;
			qulified.push_back(remainCrews[i]);
			remainCrews.erase(remainCrews.begin() + i);
		}
		else if (!remainCrews[i].Cp && !FoFlag)
		{//找一个副机长，不使用替补资格
			FoFlag = true;
			qulified.push_back(remainCrews[i]);
			remainCrews.erase(remainCrews.begin() + i);
		}
		if (CpFlag && FoFlag)
			break;
	}
	if (!CpFlag)
	{//找有副机长替补资格的正机长
		for (int i = 0; i < (int)remainCrews.size(); ++i)
		{
			if (remainCrews[i].Cp && !CpFlag)
			{
				CpFlag = true;
				qulified.push_back(remainCrews[i]);
				remainCrews.erase(remainCrews.begin() + i);
			}
		}
	}
	if (!CpFlag) return false;

	//判断是否需要使用正机长的替补资格
	if (!FoFlag)
	{//需要使用正机长的替补资格
		for (int i = 0; i < (int)remainCrews.size(); ++i)
		{
			if (remainCrews[i].Cp && remainCrews[i].Fo)
			{
				FoFlag = true;
				qulified.push_back(remainCrews[i]);
				remainCrews.erase(remainCrews.begin() + i);
			}
			if (FoFlag)
				break;
		}
	}
	if (!FoFlag) return false;

	return true;
}

//判断某个航班相对于上个航班是不是可行航班，满足执勤时间要求, 满足任务环时间要求
bool JgeFlt(Flight &lastFlt, Flight &preFlt, bool base, bool newDuty, bool newRing)
{
	if (base) return true;

	//判断 连接时间 是否>=MinCT
	double diffTime1 = INT_MAX,		//判断连接时间
		diffTime2 = INT_MAX,		//判断执勤时间
		diffTime3 = INT_MAX;		//判断任务环时间

	if (!base)//若不是从基地出发的第一个航班，则有前一个航班laseFlt，不是新的执勤
		diffTime1 = CalDiffTime(lastFlt, preFlt);
	else if (!base && newDuty)
		diffTime2 = CalDiffTime(lastFlt, preFlt);
	else if(!base && newRing)
		diffTime3 = CalDiffTime(lastFlt, preFlt);

	if (diffTime1 < 0 || diffTime2 < 0 || diffTime3 < 0)
		return false;

	if (!newDuty && !newRing && diffTime1 < MinCT)			//连接时间满足要求
		return false;
	if (newDuty && diffTime2 < MinRest)						//执勤时间满足要求
		return false;
	if (newRing && diffTime3 < MinVacDay * 24 * 60)			//任务环时间满足要求
		return false;

	return true;
}

//计算某个机场的航班数量
int CalFltNum(char* port, vector<Flight> &remainFlight)
{
	//int res = 0;
	//for (int i = 0; i < (int)remainFlight.size(); ++i)
	//{
	//	if (!strcmp(port, remainFlight[i].Ds))
	//		++res;
	//}
	//return res;
	return g_FltNum[port];
}

//寻找飞行时间最长的航班			航班数量最多的机场
int FindFlight(char* prePort, vector<Flight> &remainFlight, Flight & lastFlt, bool base, bool newDuty, bool newRing)//最后3个参数表示是否从基地出发,是否新的执勤，是否新的任务环
{
	//double minTime = 0;		//最大航行时间
	double maxFNum = 0;	//最大航班数量
	int resFlt = -1;
	for (int i = 0; i < (int)remainFlight.size(); ++i)
	{
		if (!strcmp(prePort, remainFlight[i].Ds))
		{
			bool fltFlag = JgeFlt(lastFlt, remainFlight[i], base, newDuty, newRing);//判断相邻两个航班之间的连接时间是否>=MinCT
			
			if (fltFlag)				
			{//计算航班数量
				//double fltTime = CalFltTime(remainFlight[i]);
				int fltNum = CalFltNum(remainFlight[i].Ds, remainFlight);
				//if (fltTime > minTime)
				
				//添加随机因素
				int randR = rand() % 101;
				if (fltNum > maxFNum )
				{
					/*if (base)
					{
						resFlt = i;
						maxFNum = fltNum;
					}*/
					//else if(strcmp(remainFlight[i].As, lastFlt.Ds) != 0)
					//{
					if (resFlt == -1)
					{
						resFlt = i;
						maxFNum = fltNum;
					}
					else
					{
						if (randR > 50)
						{
							resFlt = i;
							maxFNum = fltNum;
						}
					}
					//}
				}
			}
		
		}
	}
	return resFlt;
}

//判断某个机场是否有可行航班可以使得当前航班环返回基地
bool JgeToBase(char *prePort, char *base, Flight &lastFlt, vector<Flight> &remainFlight, Flight &resFlt, bool newDuty, bool newRing)
{
	for (int i = 0; i < (int)remainFlight.size(); ++i)
	{
		if (!strcmp(prePort, remainFlight[i].Ds) && !strcmp(base, remainFlight[i].As)
			&& JgeFlt(lastFlt,remainFlight[i],0, newDuty, newRing))
		{//若有，则选择第一个可以返回基地的航班
			resFlt = remainFlight[i];
			return true;
		}
	}
	return false;
}

//在已经构建的航班环中选择一个可以在某机场接纳机组人员的航班环，假设乘机也需要满足最小连接时间约束
int BackFltRing(char *prePort, char *preBase, Flight &preFlt, FltRing &preRing, bool newDuty, bool newRing)
{
	for (int i = 0; i < (int)preRing.flts.size(); ++i)
	{
		if (!strcmp(preRing.flts[i].Ds, prePort) && JgeFlt(preFlt, preRing.flts[i], 0, newDuty, newRing)
			&& !strcmp(preBase, preRing.flts[0].Ds))//基地相同
			return i;
	}
	return -1;
}

//判断从前一个航班当当前选定的航班是不是新的执勤
bool JgeNewDuty(bool base, Flight &lastFlt, Flight &preFlt)
{
	if (base) return true;

	int newDuty = false;
	
	if (lastFlt.dutyBeg.tm_mday != preFlt.dutyBeg.tm_mday)//若不是同一天出发的，则是newDuty
		newDuty = true;
	else if (lastFlt.dutyTime + CalDiffTime(lastFlt, preFlt) + CalFltTime(preFlt) > MaxDP)//执勤时间判断
		newDuty = true;
	else if (lastFlt.dutyFlyTime + CalFltTime(preFlt) > MaxBlk)//执勤飞行时间判断
		newDuty = true;
	else
		newDuty = false;
	return newDuty;
}

//判断从前一个航班当当前选定的航班是不是新的任务环
bool JgeNewRing(bool base, Flight &lastFlt, Flight &preFlt)
{
	if (base) return true;

	int newRing = false;

	if (lastFlt.ringTime + CalDiffTime(lastFlt, preFlt) + CalFltTime(preFlt) > MaxSuccOn*24*60)//连续执勤天数判断
		newRing = true;


	return newRing;
}


//贪婪构造初始解
void InitCrt(Solution &initSol)
{
	//遍历所有机组人员，划分不同基地的机组人员
	vector<vector<Crew>> partCrews(1);

	char *preBs = SetC[0].Bs;
	partCrews.back().push_back(SetC[0]);
	for (int i = 1; i < crwNum; ++i)
	{
		if (!strcmp(SetC[i].Bs, preBs))
			partCrews.back().push_back(SetC[i]);
		else		//假设机组人员的基地顺序排列
		{
			partCrews.push_back(vector<Crew>());
			preBs = SetC[i].Bs;
			partCrews.back().push_back(SetC[i]);
		}
	}
	if (partCrews.size() > 1)
	{
		vector<Crew> t = partCrews[0];
		partCrews[0] = partCrews[1];
		partCrews[1] = t;
	}

	//对于每一个基地的机组人员新建航班环
	for (int i = 0; i < (int)partCrews.size(); ++i)
	{
		//判断当前剩余机组人员是否能够达到最低资格配置；
		//若能，则找到用于新建航班环的机组人员；若不能将剩余机组人员加到上一个航班环中
		vector<Crew> preCrews,
			notUsedCrws;				//记录最终没有用上的机组人员
		bool flag = FindCrewPair(preCrews, partCrews[i]);
		char *base = preCrews[0].Bs;
		while (flag)
		{//若找到合适配置的机组人员，新开辟一个航班环
			initSol.fRings.push_back(FltRing());
			//贪心往每一个航班环后插入航班，直到不能插入为止
			Flight tf;
			int preFlt = FindFlight(preCrews[0].Bs, initSol.unDone, tf, 1,1,1);	//寻找到达机场航班数最大的航班
			
			while (preFlt != -1)
			{
				Flight tFlt = initSol.unDone[preFlt], lastFlt;
				//判断从前一个航班到当前选定的航班是不是新的任务环
				bool newRing = false, newDuty = false;
				if (initSol.fRings.back().flts.empty())
				{
					Flight tf;
					newRing = JgeNewRing(1, tf, tFlt);
				}
				else
					newRing = JgeNewRing(0, initSol.fRings.back().flts.back(), tFlt);
				if (!newRing)
				{
					//判断从前一个航班到当前选定的航班是不是新的执勤
					if (initSol.fRings.back().flts.empty())
					{
						Flight tf;
						newDuty = JgeNewDuty(1, tf, tFlt);
					}
					else
						newDuty = JgeNewDuty(0, initSol.fRings.back().flts.back(), tFlt);
				}
				else
					newDuty = true;//新任务环肯定对应新的执勤

				tFlt.dutyBeg = newDuty ? tFlt.Dt : lastFlt.Dt;
				double flyTime = CalFltTime(tFlt);
				tFlt.dutyFlyTime += newDuty ? flyTime : initSol.fRings.back().flts.back().dutyFlyTime + flyTime;
				tFlt.dutyTime += newDuty ? flyTime : initSol.fRings.back().flts.back().dutyTime + flyTime + CalDiffTime(lastFlt, tFlt);
				tFlt.ringTime += newRing ? flyTime : initSol.fRings.back().flts.back().dutyTime + flyTime + CalDiffTime(lastFlt, tFlt);
				tFlt.tolRingTime += initSol.fRings.back().flts.empty() ? flyTime : lastFlt.dutyTime + flyTime + CalDiffTime(lastFlt, tFlt);
				if (tFlt.ringTime > MaxTAFB || tFlt.dutyFlyTime > MaxBlk || tFlt.dutyTime > MaxDP)
				{//判断最大航班环的时间
					preFlt = -1;
					initSol.fRings.back().AccuRingTime += initSol.fRings.back().flts.back().ringTime;
					goto LABEL;
				}
				else
				{
					if (!initSol.fRings.back().flts.empty())
					{
						initSol.fRings.back().dutyNum += newDuty ? 1 : 0;
						initSol.fRings.back().tolDutyTime += newDuty ? initSol.fRings.back().flts.back().dutyTime : 0;
						initSol.fRings.back().tolFlyTime += newDuty ? initSol.fRings.back().flts.back().dutyFlyTime : 0;
						if (newDuty)
						{
							initSol.fRings.back().dutyTimeRec.push_back(initSol.fRings.back().flts.back().dutyTime);
							initSol.fRings.back().flyTimeRec.push_back(initSol.fRings.back().flts.back().dutyFlyTime);
						}
						initSol.fRings.back().ringNum += newRing ? 1 : 0;
						if(newRing)
							initSol.fRings.back().betweenRings.push_back(CalDiffTime(initSol.fRings.back().flts.back(),tFlt));
						initSol.fRings.back().AccuRingTime += newRing ? initSol.fRings.back().flts.back().ringTime : 0;

						if (tFlt.Dt.tm_mday != initSol.fRings.back().flts.back().Dt.tm_mday)
							initSol.fRings.back().datRing[initSol.fRings.back().flts.back().Dt.tm_mday] += 1;
					}
					initSol.fRings.back().flts.push_back(tFlt);
					initSol.fRings.back().svRec.push_back(1);
					//initSol.Done.push_back(tFlt);
					initSol.unDone.erase(initSol.unDone.begin() + preFlt);
					--g_FltNum[tFlt.Ds];
					initSol.fRings.back().crws.push_back(preCrews);				//当前航班的机组人员配置

					lastFlt = initSol.fRings.back().flts.back();

					if (JgeToBase(tFlt.As, base, lastFlt, initSol.unDone, initSol.fRings.back().lastFlight, newDuty, newRing))
					{//更新最后一个可以返回基地的航班lastPort, lastFlight
						initSol.fRings.back().lastPortIdx = (int)initSol.fRings.back().flts.size() - 1;
						initSol.fRings.back().lastFlight.dutyFlyTime += initSol.fRings.back().flts.back().dutyFlyTime + flyTime;
						initSol.fRings.back().lastFlight.dutyTime += initSol.fRings.back().flts.back().dutyTime + flyTime + CalDiffTime(initSol.fRings.back().flts.back(), tFlt);
						initSol.fRings.back().lastFlight.ringTime +=  initSol.fRings.back().flts.back().dutyTime + flyTime + CalDiffTime(initSol.fRings.back().flts.back(), tFlt);
						initSol.fRings.back().lastFlight.tolRingTime += initSol.fRings.back().flts.back().dutyTime + flyTime + CalDiffTime(initSol.fRings.back().flts.back(), tFlt);

					}

					preFlt = FindFlight(lastFlt.As, initSol.unDone, lastFlt, 0, newDuty, newRing);//寻找航行时间最大的航班
				}

				LABEL:
				if (preFlt == -1)
				{//航班环构造结束，判断机组人员是否在基地
					int lastPos = initSol.fRings.back().lastPortIdx;
					if (strcmp(initSol.fRings.back().flts.back().As, base) != 0 && lastPos != -1)
					{//若不在，则选择最后一个可以返回基地的航班让机组人员返回基地，更新initSol.unDone和initSol.Done
						Flight lastFlt = initSol.fRings.back().lastFlight;
						if (lastPos == initSol.fRings.back().flts.size() - 1)
						{//直接回基地就行
							initSol.fRings.back().flts.push_back(lastFlt);
							initSol.fRings.back().crws.push_back(initSol.fRings.back().crws.back());
							initSol.fRings.back().svRec.push_back(1);

						}
						else
						{
							vector<Flight> delFlts, recFlts;
							vector<vector<Crew>> recCrws;
							vector<bool> recSv;
							delFlts.assign(initSol.fRings.back().flts.begin() + lastPos + 1, initSol.fRings.back().flts.end());
							initSol.unDone.insert(initSol.unDone.end(), delFlts.begin(), delFlts.end());
							//更新航班环、员工环和服务环
							recFlts.assign(initSol.fRings.back().flts.begin(), initSol.fRings.back().flts.begin() + lastPos + 1);
							initSol.fRings.back().flts = recFlts;
							recCrws.assign(initSol.fRings.back().crws.begin(), initSol.fRings.back().crws.begin() + lastPos + 2);
							initSol.fRings.back().crws = recCrws;
							recSv.assign(initSol.fRings.back().svRec.begin(), initSol.fRings.back().svRec.begin() + lastPos + 2);
							initSol.fRings.back().svRec = recSv;

							initSol.fRings.back().flts.push_back(lastFlt);

						}
					}
					else if (lastPos == -1)
					{//若没有航班可以让当前机组人员做任务回基地，则选择其他航班乘机回基地
						int j = 0;
						for (; j < initSol.fRings.size() - 1; ++j)
						{
							char *preBase = initSol.fRings.back().flts[0].Ds;
							int pos = BackFltRing(initSol.fRings.back().flts.back().As, preBase, initSol.fRings.back().flts.back(), initSol.fRings[j], newDuty, newRing);
							if (pos != -1)
							{//让当前机组人员在pos位置之后乘坐initSol.fRings[j]的飞机返回基地
								vector<Flight> recFlts;
								vector<vector<Crew>> recCrws;
								vector<bool> recSv;
								recFlts.assign(initSol.fRings[j].flts.begin()+pos, initSol.fRings[j].flts.end());

								initSol.fRings.back().flts.insert(initSol.fRings.back().flts.end(), recFlts.begin(), recFlts.end());
								int tNum = (int)initSol.fRings[j].flts.size() - pos;
								initSol.fRings.back().crws.insert(initSol.fRings.back().crws.end(), tNum, initSol.fRings.back().crws[0]);
								initSol.fRings.back().svRec.insert(initSol.fRings.back().svRec.end(), tNum, 0);
								

								break;
							}
						}
						if (j == initSol.fRings.size() - 1)
						{//当前航班环直接判定无效
							initSol.unDone.insert(initSol.unDone.end(), initSol.fRings.back().flts.begin(), initSol.fRings.back().flts.end());
							notUsedCrws.insert(notUsedCrws.end(), initSol.fRings.back().crws[0].begin(), initSol.fRings.back().crws[0].end());
							initSol.fRings.pop_back();
						}

					}
					initSol.fRings.back().AccuRingTime += initSol.fRings.back().flts.back().ringTime;

				}
			}
			if (initSol.fRings.back().flts.empty())
			{
				initSol.fRings.pop_back();
				break;
			}
			
			preCrews.clear();
			flag = FindCrewPair(preCrews, partCrews[i]);
		}
		
		//若剩余机组人员不能满足最低配置，将剩余机组人员全部加入到最后一个航班环中
		if (!partCrews[i].empty())
		{
			for (int j = 0; j < (int)initSol.fRings.back().crws.size(); ++j)
				initSol.fRings.back().crws[j].insert(initSol.fRings.back().crws[j].end(), partCrews[i].begin(), partCrews[i].end());
		}
		//若有notUsedCrews,则将他们分组分别随着开始航班环依次乘机
		if (!notUsedCrws.empty())
		{
			//分组
			vector<vector<Crew>> grp(1);
			for (int j = 0; j < notUsedCrws.size(); ++j)
			{
				if (grp.back().size() < MaxDH)
					grp.back().push_back(notUsedCrws[j]);
					
				else
				{
					grp.push_back(vector<Crew>());
					grp.back().push_back(notUsedCrws[j]);
				}
			}
			//乘机
			for (int j = 0; j < (int)grp.size(); ++j)
			{
				for (int k = 0; k < (int)initSol.fRings[j].crws.size(); ++k)
					initSol.fRings[j].crws[k].insert(initSol.fRings[j].crws[k].end(), grp[j].begin(), grp[j].end());
			}
		}
		
	}


}

//计算某个航班环的航班段数量
int CalFltNum(vector<bool> &svRec)
{
	int rec = 0;
	for (const auto & c : svRec)
	{
		if (c == 1)
			++rec;
	}
	return rec;
}

//一次解编和组编优化
void OneOpt(Solution & Sol)
{
	//随机选择两个航班环
	vector<int> tSet;
	for (int i = 0; i < Sol.fRings.size(); ++i)
		tSet.push_back(i);
	random_shuffle(tSet.begin(), tSet.end());
	int ring1 = tSet[0], ring2 = tSet[1];

	int fNum1 = CalFltNum(Sol.fRings[ring1].svRec),
		fNum2 = CalFltNum(Sol.fRings[ring2].svRec);
	//判断两个


}

//输出结果
void Output(Solution & Sol)
{
	cout << "不满足机组配置的航班数为：" << (int)Sol.unDone.size() << endl;
	cout << "满足机组配置的航班数为：" << FltNum - (int)Sol.unDone.size() << endl;
	output << "不满足机组配置的航班数为：" << (int)Sol.unDone.size() << endl;
	output << "满足机组配置的航班数为：" << FltNum - (int)Sol.unDone.size() << endl;
	int followNum = 0;
	for (int i = 0; i < (int)Sol.fRings.size(); ++i)
	{
		for (int j = 0; j < (int)Sol.fRings[i].svRec.size(); ++j)
		{
			if (Sol.fRings[i].svRec[j] == 0)
				++followNum;
		}
	}
	cout << "整体乘机次数为：" << followNum << endl;
	output << "整体乘机次数为：" << followNum << endl;

	int rep = 0;
	for (int i = 0; i < (int)Sol.fRings.size(); ++i)
	{
		for (int j = 0; j < (int)Sol.fRings[i].crws.size(); ++j)
		{
			if (Sol.fRings[i].crws[j][1].Cp == 1)
				++rep;
		}
	}
	cout << "整体替补次数为：" << rep << endl;
	output << "整体替补次数为：" << rep << endl;

	double totalDutyCost = 0;
	for (int i = 0; i < Sol.fRings.size(); ++i)
	{
		totalDutyCost += Sol.fRings[i].tolDutyTime/60 *Sol.fRings[i].crws[0][0].Dc;
		totalDutyCost += Sol.fRings[i].tolDutyTime / 60 *Sol.fRings[i].crws[0][1].Dc;
		
	}
	cout << "总体执勤成本为：" << totalDutyCost/10000 << endl;
	output << "总体执勤成本为：" << totalDutyCost/10000 << endl;

	double maxDutyTime = 0, minDutyTime = INT_MAX, tolDutyTime = 0, tolFlyTime = 0, tolNum = 0, aveDutyTime = 0;
	for (int i = 0; i < Sol.fRings.size(); ++i)
	{
		double preMax = *max_element(Sol.fRings[i].dutyTimeRec.begin(), Sol.fRings[i].dutyTimeRec.end());
		if (preMax > maxDutyTime)
			maxDutyTime = preMax;

		double preMin = *min_element(Sol.fRings[i].flyTimeRec.begin(), Sol.fRings[i].flyTimeRec.end());
		if (preMin < minDutyTime)
			minDutyTime = preMin;

		for (int j = 0; j < Sol.fRings[i].dutyTimeRec.size(); ++j)
			tolDutyTime += Sol.fRings[i].dutyTimeRec[j];

		tolNum += Sol.fRings[i].dutyNum;
	}
	for (int i = 0; i < Sol.fRings.size(); ++i)
	{
		for (int j = 0; j < Sol.fRings[i].flts.size()-1; ++j)
			tolFlyTime += Sol.fRings[i].svRec[j] * CalFltTime(Sol.fRings[i].flts[j]);
	}

	double vacTime = 0;
	for (int i = 0; i < Sol.fRings.size(); ++i)
	{
		for (int j = 0; j < Sol.fRings[i].betweenRings.size(); ++j)
			vacTime += Sol.fRings[i].betweenRings[j];
	}

	//计算所有任务环的时间
	double tolRingTime = 0;
	for (int i = 0; i < Sol.fRings.size(); ++i)
		tolRingTime += Sol.fRings[i].AccuRingTime;


	cout << fixed << setprecision(2) << "最大一次执勤飞行时长为：" << maxDutyTime/60 << endl;
	cout << fixed << setprecision(2) << "最小一次执勤飞行时长为：" << minDutyTime/60 << endl;
	cout << fixed << setprecision(2) << "平均一次执勤飞行时长为：" << tolDutyTime/ tolNum /60 << endl;
	cout << fixed << setprecision(2) << "机组总体利用率为：" << (tolFlyTime) * 100 / (tolDutyTime) << "%" << endl;
	cout << fixed << setprecision(2) << "总体任务环成本为：" << tolRingTime * 20 / 60 / 10000 << endl;

	output << fixed << setprecision(2) << "最大一次执勤飞行时长为：" << maxDutyTime/60 << endl;
	output << fixed << setprecision(2) << "最小一次执勤飞行时长为：" << minDutyTime/60 << endl;
	output << fixed << setprecision(2) << "平均一次执勤飞行时长为：" << tolDutyTime/ tolNum /60 << endl;
	output << fixed << setprecision(2) << "机组总体利用率为：" << (tolFlyTime) * 100 / (tolDutyTime) << "%" << endl;
	output << fixed << setprecision(2) << "总体任务环成本为：" << tolRingTime * 20 / 60 / 10000 << endl;

	clock_t eTime = clock();
	cout << "程序运行时间为：" << fixed << setprecision(2) << double((eTime - sTime) / CLOCKS_PER_SEC)/60.0 << endl;
	output << "程序运行时间为：" << fixed << setprecision(2) << double((eTime - sTime) / CLOCKS_PER_SEC)/60.0 << endl;


	//处理任务环分布
	unordered_map<int, int> rec;
	for (int i = 0; i < Sol.fRings.size(); ++i)
	{
		unordered_map<int, int>::iterator ite = Sol.fRings[i].datRing.begin();
		for (int j = 0; j < Sol.fRings[i].datRing.size(); ++j)
		{
			rec[ite->first] += 1;
			++ite;
		}
	}
	cout << "不同天数的任务环的数量分布:" << endl;
	output << "不同天数的任务环的数量分布:" << endl;
	unordered_map<int, int>::iterator ite = rec.begin();
	while(ite != rec.end())
	{
		cout << "第 " << ite->first << " 天的任务环数量为：" << ite->second << endl;
		output << "第 " << ite->first << " 天的任务环数量为：" << ite->second << endl;
		++ite;
	}
	
	//输出没有完成的航班
	output << "\n\n时间的格式为月-日-年-时-分" << endl;
	for (int i = 0; i < Sol.unDone.size(); ++i)
	{
		output << Sol.unDone[i].Dt.tm_mon+1 << "\t" << Sol.unDone[i].Dt.tm_mday << "\t" << Sol.unDone[i].Dt.tm_year+1900 << "\t" << Sol.unDone[i].Dt.tm_hour << "\t" << Sol.unDone[i].Dt.tm_min << "\t";
		output << Sol.unDone[i].At.tm_mon+1 << "\t" << Sol.unDone[i].At.tm_mday << "\t" << Sol.unDone[i].At.tm_year+1900 << "\t" << Sol.unDone[i].At.tm_hour << "\t" << Sol.unDone[i].At.tm_min << "\t";
		output << Sol.unDone[i].Ds << "\t";
		output << Sol.unDone[i].As << "\t" << endl;
	}
	//输出所有航班信息
	output << "\n\n所有已经完成的航班信息" << endl;
	output << "机组人员信息包括：机组人员数量、是否机长，是否副机长，是否替补" << endl;
	for (int i = 0; i < Sol.fRings.size(); ++i)
	{
		output << Sol.fRings[i].crws[0].size() << endl;
		for (int j = 0; j < Sol.fRings[i].crws[0].size(); ++j)
		{
			output << Sol.fRings[i].crws[0][j].Cp << "\t" << Sol.fRings[i].crws[0][j].Fo << "\t" << (j == Sol.fRings[i].crws[0].size()-1 ? Sol.fRings[i].crws[0][j].Cp:0) << endl;
		}
	}
	output << "\n\n航班信息包括：航班起飞时间，起飞机场、到达时间、到达机场" << endl;
	for (int i = 0; i < Sol.fRings.size(); ++i)
	{
		for (int j = 0; j < Sol.fRings[i].flts.size(); ++j)
		{
			output << Sol.fRings[i].flts[j].Dt.tm_mon+1 << "\t" << Sol.fRings[i].flts[j].Dt.tm_mday
				<< "\t" << Sol.fRings[i].flts[j].Dt.tm_year+1900 << "\t" << Sol.fRings[i].flts[j].Dt.tm_hour << "\t"
				<< Sol.fRings[i].flts[j].Dt.tm_min << "\t" << Sol.fRings[i].flts[j].Ds << "\t" << 

				Sol.fRings[i].flts[j].At.tm_mon+1 << "\t" << Sol.fRings[i].flts[j].At.tm_mday
				<< "\t" << Sol.fRings[i].flts[j].At.tm_year+1900 << "\t" << Sol.fRings[i].flts[j].At.tm_hour << "\t"
				<< Sol.fRings[i].flts[j].At.tm_min << "\t" << Sol.fRings[i].flts[j].As << endl;
		}
		output << endl << endl;

	}
}