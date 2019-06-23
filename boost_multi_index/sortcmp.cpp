// multi_index.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct PowerCmp;
struct ChargeCmp;
//��һ�����Ϣ
class PlayrInfo{
public:
	int getPower()const{
		return u_power;
	}

	int getCharge()const{
		return u_charge;
	}
private:
	int u_ind; //���Ψһid
	int u_power; //���ս��
	int u_charge; //��ҳ�ֵ����
};

bool chargeCmp(PlayrInfo *a, PlayrInfo *b) 
{
	return a->getCharge() > b->getCharge();
}

bool powerCmp(PlayrInfo *a, PlayrInfo *b) 
{
	return a->getPower() > b->getPower();
}
/*
int main(int argc, char* argv[])
{
	//�ֱ�ʵ������
	vector<PlayrInfo*> vecPlayer;
	
	sort(vecPlayer.begin(),vecPlayer.end(),powerCmp);
	sort(vecPlayer.begin(),vecPlayer.end(),chargeCmp);
	getchar();
	return 0;
}*/

