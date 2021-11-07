#define _CRT_SECURE_NO_WARNINGS
#include"header4.h"

using namespace std;


vector<int> P;			//���л�����λ�ü���
int N = 2;				//ÿ�������ܹ���Ҫ�Ļ�������
int Nc = 1;				//ÿ�������ܹ���Ҫ������������
int Nf = 1;				//ÿ�������ܹ�����Ҫ�ĸ���������
int FltNum;				//ȫ���ĺ�����
int crwNum;				//ȫ���Ļ�����Ա��

double MinCT = 40;		//����֮����С������ʱ��40min
double MaxBlk = 600;	//һ��ִ�ڷ���ʱ����಻����600min
double MaxDP = 720;		//ִ��ʱ����಻����720min
double MinRest = 660;	//����ִ��֮�����Ϣʱ�䲻����660min
int MaxDH = 5;			//ÿ�˺������ĳ˻����� 5��
double MaxTAFB = 14400;	//�Ű����ڵ���������Ա��������ʱ��������14400
int MaxSuccOn = 4;		//����ִ������������ 4��
int MinVacDay = 2;		//������������֮��������Ϣ 2��


vector<Flight> SetF;	//���к���ļ���
vector<Crew> SetC;		//����Ա���ļ���
unordered_map<string, int> g_FltNum;	//ÿ����������������֮��ĺ�������
clock_t sTime = clock();

ofstream output("E:/�����ļ�/�о���/��Ϊ��ѧ��ģ����/F/newOut/B-3-out.txt");

//��ȡ����
void InputData()
{
	ifstream input1("E:/�����ļ�/�о���/��Ϊ��ѧ��ģ����/F/B-Flight.txt");
	ifstream input2("E:/�����ļ�/�о���/��Ϊ��ѧ��ģ����/F/B-Crew.txt");
	if (!input1 || !input2 || !output)
	{
		cout << "������/����ļ�ʧ�ܣ�" << endl;
		exit(-1);
	}

	input1 >> FltNum;
	input2 >> crwNum;
	SetF.resize(FltNum, Flight());
	SetC.resize(crwNum, Crew());

	for (int i = 0; i < FltNum; ++i)
	{//�����������
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

//����ÿ����������������֮��ĺ�������
void FillFltNum()
{
	for (int i = 0; i < FltNum; ++i)
	{
		g_FltNum[SetF[i].Ds] += 1;
		g_FltNum[SetF[i].As] += 1;

	}
}

//�ж�����������뿪ʱ���Ƿ���ͬһ��
bool JgeSameDay(Flight &f1, Flight &f2)
{
	if (f1.At.tm_year != f2.At.tm_year)
		return false;
	else if (f1.At.tm_mon != f2.At.tm_mon)
		return false;
	else if (f1.At.tm_mday != f2.At.tm_mday)
		return false;
	else
		return true;//ͬһ�꣬ͬһ�£�ͬһ��
}

//����������������ֱ�ӵĳ���ʱ����뿪ʱ��֮��,f1Ϊǰһ�����࣬f2Ϊ��һ������
double CalDiffTime(Flight &f1, Flight &f2)
{
	//ʱ��Ӽ�
	time_t t2 = mktime(&f2.Dt);
	time_t t1 = mktime(&f1.At);

	double t = difftime(t2, t1);//double difftime( time_t timeEnd, time_t timeStart );
	return t/60;
}

//����ĳ������ķ���ʱ��
double CalFltTime(Flight &f)
{
	//ʱ��Ӽ�
	time_t t2 = mktime(&f.At);
	time_t t1 = mktime(&f.Dt);

	double t = difftime(t2, t1);//double difftime( time_t timeEnd, time_t timeStart );
	return t/60;
}

//Ѱ������ʸ����û�����Ա���
bool FindCrewPair(vector<Crew> &qulified, vector<Crew> &remainCrews)
{
	if (remainCrews.size() < N) return false;

	bool CpFlag = false,	//����Ƿ���������
		FoFlag = false;		//����Ƿ��и�����
	for (int i = 0; i <(int)remainCrews.size(); ++i)
	{
		if (remainCrews[i].Cp && !remainCrews[i].Fo && !CpFlag)
		{//����һ��û�и������油�ʸ��������
			CpFlag = true;
			qulified.push_back(remainCrews[i]);
			remainCrews.erase(remainCrews.begin() + i);
		}
		else if (!remainCrews[i].Cp && !FoFlag)
		{//��һ������������ʹ���油�ʸ�
			FoFlag = true;
			qulified.push_back(remainCrews[i]);
			remainCrews.erase(remainCrews.begin() + i);
		}
		if (CpFlag && FoFlag)
			break;
	}
	if (!CpFlag)
	{//���и������油�ʸ��������
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

	//�ж��Ƿ���Ҫʹ�����������油�ʸ�
	if (!FoFlag)
	{//��Ҫʹ�����������油�ʸ�
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

//�ж�ĳ������������ϸ������ǲ��ǿ��к��࣬����ִ��ʱ��Ҫ��, ��������ʱ��Ҫ��
bool JgeFlt(Flight &lastFlt, Flight &preFlt, bool base, bool newDuty, bool newRing)
{
	if (base) return true;

	//�ж� ����ʱ�� �Ƿ�>=MinCT
	double diffTime1 = INT_MAX,		//�ж�����ʱ��
		diffTime2 = INT_MAX,		//�ж�ִ��ʱ��
		diffTime3 = INT_MAX;		//�ж�����ʱ��

	if (!base)//�����Ǵӻ��س����ĵ�һ�����࣬����ǰһ������laseFlt�������µ�ִ��
		diffTime1 = CalDiffTime(lastFlt, preFlt);
	else if (!base && newDuty)
		diffTime2 = CalDiffTime(lastFlt, preFlt);
	else if(!base && newRing)
		diffTime3 = CalDiffTime(lastFlt, preFlt);

	if (diffTime1 < 0 || diffTime2 < 0 || diffTime3 < 0)
		return false;

	if (!newDuty && !newRing && diffTime1 < MinCT)			//����ʱ������Ҫ��
		return false;
	if (newDuty && diffTime2 < MinRest)						//ִ��ʱ������Ҫ��
		return false;
	if (newRing && diffTime3 < MinVacDay * 24 * 60)			//����ʱ������Ҫ��
		return false;

	return true;
}

//����ĳ�������ĺ�������
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

//Ѱ�ҷ���ʱ����ĺ���			�����������Ļ���
int FindFlight(char* prePort, vector<Flight> &remainFlight, Flight & lastFlt, bool base, bool newDuty, bool newRing)//���3��������ʾ�Ƿ�ӻ��س���,�Ƿ��µ�ִ�ڣ��Ƿ��µ�����
{
	//double minTime = 0;		//�����ʱ��
	double maxFNum = 0;	//��󺽰�����
	int resFlt = -1;
	for (int i = 0; i < (int)remainFlight.size(); ++i)
	{
		if (!strcmp(prePort, remainFlight[i].Ds))
		{
			bool fltFlag = JgeFlt(lastFlt, remainFlight[i], base, newDuty, newRing);//�ж�������������֮�������ʱ���Ƿ�>=MinCT
			
			if (fltFlag)				
			{//���㺽������
				//double fltTime = CalFltTime(remainFlight[i]);
				int fltNum = CalFltNum(remainFlight[i].Ds, remainFlight);
				//if (fltTime > minTime)
				
				//����������
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

//�ж�ĳ�������Ƿ��п��к������ʹ�õ�ǰ���໷���ػ���
bool JgeToBase(char *prePort, char *base, Flight &lastFlt, vector<Flight> &remainFlight, Flight &resFlt, bool newDuty, bool newRing)
{
	for (int i = 0; i < (int)remainFlight.size(); ++i)
	{
		if (!strcmp(prePort, remainFlight[i].Ds) && !strcmp(base, remainFlight[i].As)
			&& JgeFlt(lastFlt,remainFlight[i],0, newDuty, newRing))
		{//���У���ѡ���һ�����Է��ػ��صĺ���
			resFlt = remainFlight[i];
			return true;
		}
	}
	return false;
}

//���Ѿ������ĺ��໷��ѡ��һ��������ĳ�������ɻ�����Ա�ĺ��໷������˻�Ҳ��Ҫ������С����ʱ��Լ��
int BackFltRing(char *prePort, char *preBase, Flight &preFlt, FltRing &preRing, bool newDuty, bool newRing)
{
	for (int i = 0; i < (int)preRing.flts.size(); ++i)
	{
		if (!strcmp(preRing.flts[i].Ds, prePort) && JgeFlt(preFlt, preRing.flts[i], 0, newDuty, newRing)
			&& !strcmp(preBase, preRing.flts[0].Ds))//������ͬ
			return i;
	}
	return -1;
}

//�жϴ�ǰһ�����൱��ǰѡ���ĺ����ǲ����µ�ִ��
bool JgeNewDuty(bool base, Flight &lastFlt, Flight &preFlt)
{
	if (base) return true;

	int newDuty = false;
	
	if (lastFlt.dutyBeg.tm_mday != preFlt.dutyBeg.tm_mday)//������ͬһ������ģ�����newDuty
		newDuty = true;
	else if (lastFlt.dutyTime + CalDiffTime(lastFlt, preFlt) + CalFltTime(preFlt) > MaxDP)//ִ��ʱ���ж�
		newDuty = true;
	else if (lastFlt.dutyFlyTime + CalFltTime(preFlt) > MaxBlk)//ִ�ڷ���ʱ���ж�
		newDuty = true;
	else
		newDuty = false;
	return newDuty;
}

//�жϴ�ǰһ�����൱��ǰѡ���ĺ����ǲ����µ�����
bool JgeNewRing(bool base, Flight &lastFlt, Flight &preFlt)
{
	if (base) return true;

	int newRing = false;

	if (lastFlt.ringTime + CalDiffTime(lastFlt, preFlt) + CalFltTime(preFlt) > MaxSuccOn*24*60)//����ִ�������ж�
		newRing = true;


	return newRing;
}


//̰�������ʼ��
void InitCrt(Solution &initSol)
{
	//�������л�����Ա�����ֲ�ͬ���صĻ�����Ա
	vector<vector<Crew>> partCrews(1);

	char *preBs = SetC[0].Bs;
	partCrews.back().push_back(SetC[0]);
	for (int i = 1; i < crwNum; ++i)
	{
		if (!strcmp(SetC[i].Bs, preBs))
			partCrews.back().push_back(SetC[i]);
		else		//���������Ա�Ļ���˳������
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

	//����ÿһ�����صĻ�����Ա�½����໷
	for (int i = 0; i < (int)partCrews.size(); ++i)
	{
		//�жϵ�ǰʣ�������Ա�Ƿ��ܹ��ﵽ����ʸ����ã�
		//���ܣ����ҵ������½����໷�Ļ�����Ա�������ܽ�ʣ�������Ա�ӵ���һ�����໷��
		vector<Crew> preCrews,
			notUsedCrws;				//��¼����û�����ϵĻ�����Ա
		bool flag = FindCrewPair(preCrews, partCrews[i]);
		char *base = preCrews[0].Bs;
		while (flag)
		{//���ҵ��������õĻ�����Ա���¿���һ�����໷
			initSol.fRings.push_back(FltRing());
			//̰����ÿһ�����໷����뺽�ֱ࣬�����ܲ���Ϊֹ
			Flight tf;
			int preFlt = FindFlight(preCrews[0].Bs, initSol.unDone, tf, 1,1,1);	//Ѱ�ҵ���������������ĺ���
			
			while (preFlt != -1)
			{
				Flight tFlt = initSol.unDone[preFlt], lastFlt;
				//�жϴ�ǰһ�����ൽ��ǰѡ���ĺ����ǲ����µ�����
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
					//�жϴ�ǰһ�����ൽ��ǰѡ���ĺ����ǲ����µ�ִ��
					if (initSol.fRings.back().flts.empty())
					{
						Flight tf;
						newDuty = JgeNewDuty(1, tf, tFlt);
					}
					else
						newDuty = JgeNewDuty(0, initSol.fRings.back().flts.back(), tFlt);
				}
				else
					newDuty = true;//�����񻷿϶���Ӧ�µ�ִ��

				tFlt.dutyBeg = newDuty ? tFlt.Dt : lastFlt.Dt;
				double flyTime = CalFltTime(tFlt);
				tFlt.dutyFlyTime += newDuty ? flyTime : initSol.fRings.back().flts.back().dutyFlyTime + flyTime;
				tFlt.dutyTime += newDuty ? flyTime : initSol.fRings.back().flts.back().dutyTime + flyTime + CalDiffTime(lastFlt, tFlt);
				tFlt.ringTime += newRing ? flyTime : initSol.fRings.back().flts.back().dutyTime + flyTime + CalDiffTime(lastFlt, tFlt);
				tFlt.tolRingTime += initSol.fRings.back().flts.empty() ? flyTime : lastFlt.dutyTime + flyTime + CalDiffTime(lastFlt, tFlt);
				if (tFlt.ringTime > MaxTAFB || tFlt.dutyFlyTime > MaxBlk || tFlt.dutyTime > MaxDP)
				{//�ж���󺽰໷��ʱ��
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
					initSol.fRings.back().crws.push_back(preCrews);				//��ǰ����Ļ�����Ա����

					lastFlt = initSol.fRings.back().flts.back();

					if (JgeToBase(tFlt.As, base, lastFlt, initSol.unDone, initSol.fRings.back().lastFlight, newDuty, newRing))
					{//�������һ�����Է��ػ��صĺ���lastPort, lastFlight
						initSol.fRings.back().lastPortIdx = (int)initSol.fRings.back().flts.size() - 1;
						initSol.fRings.back().lastFlight.dutyFlyTime += initSol.fRings.back().flts.back().dutyFlyTime + flyTime;
						initSol.fRings.back().lastFlight.dutyTime += initSol.fRings.back().flts.back().dutyTime + flyTime + CalDiffTime(initSol.fRings.back().flts.back(), tFlt);
						initSol.fRings.back().lastFlight.ringTime +=  initSol.fRings.back().flts.back().dutyTime + flyTime + CalDiffTime(initSol.fRings.back().flts.back(), tFlt);
						initSol.fRings.back().lastFlight.tolRingTime += initSol.fRings.back().flts.back().dutyTime + flyTime + CalDiffTime(initSol.fRings.back().flts.back(), tFlt);

					}

					preFlt = FindFlight(lastFlt.As, initSol.unDone, lastFlt, 0, newDuty, newRing);//Ѱ�Һ���ʱ�����ĺ���
				}

				LABEL:
				if (preFlt == -1)
				{//���໷����������жϻ�����Ա�Ƿ��ڻ���
					int lastPos = initSol.fRings.back().lastPortIdx;
					if (strcmp(initSol.fRings.back().flts.back().As, base) != 0 && lastPos != -1)
					{//�����ڣ���ѡ�����һ�����Է��ػ��صĺ����û�����Ա���ػ��أ�����initSol.unDone��initSol.Done
						Flight lastFlt = initSol.fRings.back().lastFlight;
						if (lastPos == initSol.fRings.back().flts.size() - 1)
						{//ֱ�ӻػ��ؾ���
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
							//���º��໷��Ա�����ͷ���
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
					{//��û�к�������õ�ǰ������Ա������ػ��أ���ѡ����������˻��ػ���
						int j = 0;
						for (; j < initSol.fRings.size() - 1; ++j)
						{
							char *preBase = initSol.fRings.back().flts[0].Ds;
							int pos = BackFltRing(initSol.fRings.back().flts.back().As, preBase, initSol.fRings.back().flts.back(), initSol.fRings[j], newDuty, newRing);
							if (pos != -1)
							{//�õ�ǰ������Ա��posλ��֮�����initSol.fRings[j]�ķɻ����ػ���
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
						{//��ǰ���໷ֱ���ж���Ч
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
		
		//��ʣ�������Ա��������������ã���ʣ�������Աȫ�����뵽���һ�����໷��
		if (!partCrews[i].empty())
		{
			for (int j = 0; j < (int)initSol.fRings.back().crws.size(); ++j)
				initSol.fRings.back().crws[j].insert(initSol.fRings.back().crws[j].end(), partCrews[i].begin(), partCrews[i].end());
		}
		//����notUsedCrews,�����Ƿ���ֱ����ſ�ʼ���໷���γ˻�
		if (!notUsedCrws.empty())
		{
			//����
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
			//�˻�
			for (int j = 0; j < (int)grp.size(); ++j)
			{
				for (int k = 0; k < (int)initSol.fRings[j].crws.size(); ++k)
					initSol.fRings[j].crws[k].insert(initSol.fRings[j].crws[k].end(), grp[j].begin(), grp[j].end());
			}
		}
		
	}


}

//����ĳ�����໷�ĺ��������
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

//һ�ν�������Ż�
void OneOpt(Solution & Sol)
{
	//���ѡ���������໷
	vector<int> tSet;
	for (int i = 0; i < Sol.fRings.size(); ++i)
		tSet.push_back(i);
	random_shuffle(tSet.begin(), tSet.end());
	int ring1 = tSet[0], ring2 = tSet[1];

	int fNum1 = CalFltNum(Sol.fRings[ring1].svRec),
		fNum2 = CalFltNum(Sol.fRings[ring2].svRec);
	//�ж�����


}

//������
void Output(Solution & Sol)
{
	cout << "������������õĺ�����Ϊ��" << (int)Sol.unDone.size() << endl;
	cout << "����������õĺ�����Ϊ��" << FltNum - (int)Sol.unDone.size() << endl;
	output << "������������õĺ�����Ϊ��" << (int)Sol.unDone.size() << endl;
	output << "����������õĺ�����Ϊ��" << FltNum - (int)Sol.unDone.size() << endl;
	int followNum = 0;
	for (int i = 0; i < (int)Sol.fRings.size(); ++i)
	{
		for (int j = 0; j < (int)Sol.fRings[i].svRec.size(); ++j)
		{
			if (Sol.fRings[i].svRec[j] == 0)
				++followNum;
		}
	}
	cout << "����˻�����Ϊ��" << followNum << endl;
	output << "����˻�����Ϊ��" << followNum << endl;

	int rep = 0;
	for (int i = 0; i < (int)Sol.fRings.size(); ++i)
	{
		for (int j = 0; j < (int)Sol.fRings[i].crws.size(); ++j)
		{
			if (Sol.fRings[i].crws[j][1].Cp == 1)
				++rep;
		}
	}
	cout << "�����油����Ϊ��" << rep << endl;
	output << "�����油����Ϊ��" << rep << endl;

	double totalDutyCost = 0;
	for (int i = 0; i < Sol.fRings.size(); ++i)
	{
		totalDutyCost += Sol.fRings[i].tolDutyTime/60 *Sol.fRings[i].crws[0][0].Dc;
		totalDutyCost += Sol.fRings[i].tolDutyTime / 60 *Sol.fRings[i].crws[0][1].Dc;
		
	}
	cout << "����ִ�ڳɱ�Ϊ��" << totalDutyCost/10000 << endl;
	output << "����ִ�ڳɱ�Ϊ��" << totalDutyCost/10000 << endl;

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

	//�����������񻷵�ʱ��
	double tolRingTime = 0;
	for (int i = 0; i < Sol.fRings.size(); ++i)
		tolRingTime += Sol.fRings[i].AccuRingTime;


	cout << fixed << setprecision(2) << "���һ��ִ�ڷ���ʱ��Ϊ��" << maxDutyTime/60 << endl;
	cout << fixed << setprecision(2) << "��Сһ��ִ�ڷ���ʱ��Ϊ��" << minDutyTime/60 << endl;
	cout << fixed << setprecision(2) << "ƽ��һ��ִ�ڷ���ʱ��Ϊ��" << tolDutyTime/ tolNum /60 << endl;
	cout << fixed << setprecision(2) << "��������������Ϊ��" << (tolFlyTime) * 100 / (tolDutyTime) << "%" << endl;
	cout << fixed << setprecision(2) << "�������񻷳ɱ�Ϊ��" << tolRingTime * 20 / 60 / 10000 << endl;

	output << fixed << setprecision(2) << "���һ��ִ�ڷ���ʱ��Ϊ��" << maxDutyTime/60 << endl;
	output << fixed << setprecision(2) << "��Сһ��ִ�ڷ���ʱ��Ϊ��" << minDutyTime/60 << endl;
	output << fixed << setprecision(2) << "ƽ��һ��ִ�ڷ���ʱ��Ϊ��" << tolDutyTime/ tolNum /60 << endl;
	output << fixed << setprecision(2) << "��������������Ϊ��" << (tolFlyTime) * 100 / (tolDutyTime) << "%" << endl;
	output << fixed << setprecision(2) << "�������񻷳ɱ�Ϊ��" << tolRingTime * 20 / 60 / 10000 << endl;

	clock_t eTime = clock();
	cout << "��������ʱ��Ϊ��" << fixed << setprecision(2) << double((eTime - sTime) / CLOCKS_PER_SEC)/60.0 << endl;
	output << "��������ʱ��Ϊ��" << fixed << setprecision(2) << double((eTime - sTime) / CLOCKS_PER_SEC)/60.0 << endl;


	//�������񻷷ֲ�
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
	cout << "��ͬ���������񻷵������ֲ�:" << endl;
	output << "��ͬ���������񻷵������ֲ�:" << endl;
	unordered_map<int, int>::iterator ite = rec.begin();
	while(ite != rec.end())
	{
		cout << "�� " << ite->first << " �����������Ϊ��" << ite->second << endl;
		output << "�� " << ite->first << " �����������Ϊ��" << ite->second << endl;
		++ite;
	}
	
	//���û����ɵĺ���
	output << "\n\nʱ��ĸ�ʽΪ��-��-��-ʱ-��" << endl;
	for (int i = 0; i < Sol.unDone.size(); ++i)
	{
		output << Sol.unDone[i].Dt.tm_mon+1 << "\t" << Sol.unDone[i].Dt.tm_mday << "\t" << Sol.unDone[i].Dt.tm_year+1900 << "\t" << Sol.unDone[i].Dt.tm_hour << "\t" << Sol.unDone[i].Dt.tm_min << "\t";
		output << Sol.unDone[i].At.tm_mon+1 << "\t" << Sol.unDone[i].At.tm_mday << "\t" << Sol.unDone[i].At.tm_year+1900 << "\t" << Sol.unDone[i].At.tm_hour << "\t" << Sol.unDone[i].At.tm_min << "\t";
		output << Sol.unDone[i].Ds << "\t";
		output << Sol.unDone[i].As << "\t" << endl;
	}
	//������к�����Ϣ
	output << "\n\n�����Ѿ���ɵĺ�����Ϣ" << endl;
	output << "������Ա��Ϣ������������Ա�������Ƿ�������Ƿ񸱻������Ƿ��油" << endl;
	for (int i = 0; i < Sol.fRings.size(); ++i)
	{
		output << Sol.fRings[i].crws[0].size() << endl;
		for (int j = 0; j < Sol.fRings[i].crws[0].size(); ++j)
		{
			output << Sol.fRings[i].crws[0][j].Cp << "\t" << Sol.fRings[i].crws[0][j].Fo << "\t" << (j == Sol.fRings[i].crws[0].size()-1 ? Sol.fRings[i].crws[0][j].Cp:0) << endl;
		}
	}
	output << "\n\n������Ϣ�������������ʱ�䣬��ɻ���������ʱ�䡢�������" << endl;
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