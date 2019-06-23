// multi_index.cpp : 定义控制台应用程序的入口点。
//
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct PowerCmp;
struct ChargeCmp;
//玩家基本信息
class PlayrInfo{
public:
	int getPower()const{
		return u_power;
	}

	int getCharge()const{
		return u_charge;
	}
private:
	int u_ind; //玩家唯一id
	int u_power; //玩家战力
	int u_charge; //玩家充值数量
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
	//分别实现排序
	vector<PlayrInfo*> vecPlayer;
	
	sort(vecPlayer.begin(),vecPlayer.end(),powerCmp);
	sort(vecPlayer.begin(),vecPlayer.end(),chargeCmp);
	getchar();
	return 0;
}*/

