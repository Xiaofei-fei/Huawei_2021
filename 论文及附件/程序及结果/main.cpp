#include"header4.h"

using namespace std;


int main()
{
	//读取数据
	InputData();
	FillFltNum();

	Solution initSol(SetF);	//初始解

	//贪婪加随机构造解
	InitCrt(initSol);
	
	//输出结果
	Output(initSol);

	return 0;
}