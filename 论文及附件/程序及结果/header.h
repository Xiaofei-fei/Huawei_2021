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


//����ṹ��
class Flight
{
public:
	struct tm Dt;			//����ĳ���ʱ��
	char Ds[3];				//����ĳ�������
	struct tm At;			//����ĵ���ʱ��
	char As[3];				//����ĵ������
	int fIdx;				//������

	struct tm dutyBeg;		//��ǰ����������ִ�ڵĿ�ʼʱ��
	double dutyTime;		//��ǰ����������ִ�ڵ���ʱ��
	double dutyFlyTime;		//��ǰ����������ִ�ڵķ���ʱ��
	double ringTime;		//��ǰ���������ĵ�������ʱ��
	double tolRingTime;		//��ǰ������������������ʱ��
};

//������Ա�ṹ��
class Crew
{
public:
	int Cp;				//Ա���Ƿ���Ե�����������0-1
	int Fo;				//Ա���Ƿ���Ե��θ�������0-1
	int Dh;				//Ա���Ƿ�����˻���0-1
	char Bs[3];			//Ա���Ļ���
	double Dc;			//Ա���ĵ�λСʱִ�ڳɱ�
	double Pc;			//Ա���ĵ�λСʱ���񻷳ɱ�
	int cIdx;			//Ա�����
};

//ִ�ڽṹ��
class Duty
{
public:
	std::vector<Flight> dutys;

};

//���໷�ṹ��
class FltRing
{
public:
	std::vector<Flight> flts;					//���༯��
	std::vector<std::vector<Crew>> crws;		//������Ա����	
	std::vector<bool> svRec;					//��¼������Ա��ÿ�������Ƿ����1�����ǳ˻�0
	int dutyNum;								//��¼�ܵ�ִ�ڴ���
	double tolDutyTime;							//��ִ��ʱ��
	double tolFlyTime;							//�ܷ���ʱ��
	std::vector<double> dutyTimeRec;					//��¼���д�ִ�ڵ�ʱ��
	std::vector<double> flyTimeRec;					//��¼���д�ִ�ڵķ���ʱ��
	int ringNum;								//��¼�ܵ���������

	double AccuRingTime;						//��¼�������񻷵�ʱ��
	std::vector<double> betweenRings;			//��¼��������֮����ݼ�ʱ��
	std::unordered_map<int, int> datRing;			//��¼��ͬ�����������


	int lastPortIdx = -1;						//��¼���һ�����Է��ػ��صĺ�����flts�е�����
	Flight lastFlight;							//��¼���һ�����Է��ػ��صĺ���
};

//��ṹ��
class Solution
{
public:
	std::vector<FltRing> fRings;	//���еĺ��໷
	std::vector<Flight> unDone;	//����δ����ĺ��༯��
	std::vector<Flight> Done;	//�����Ѿ�����ĺ��༯��

	Solution(std::vector<Flight>& SetF)
	{
		unDone = SetF;
	}
};

extern std::vector<Flight> SetF;	//���к���ļ���
extern std::vector<Crew> SetC;		//����Ա���ļ���


//��ȡ����
void InputData();

//̰�������ʼ��
void InitCrt(Solution &initSol);

//����ÿ����������������֮��ĺ�������
void FillFltNum();

//������
void Output(Solution & Sol);