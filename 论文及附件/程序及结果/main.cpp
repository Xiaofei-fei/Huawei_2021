#include"header4.h"

using namespace std;


int main()
{
	//��ȡ����
	InputData();
	FillFltNum();

	Solution initSol(SetF);	//��ʼ��

	//̰������������
	InitCrt(initSol);
	
	//������
	Output(initSol);

	return 0;
}