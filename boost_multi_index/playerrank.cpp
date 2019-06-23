// multi_index.cpp : 定义控制台应用程序的入口点。
//
#include <iostream>
using namespace std;
#include <string>
#include <iostream>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost;
using namespace boost::multi_index;

using namespace std;


//玩家基本信息
class PlayrInfo{
public:
	PlayrInfo(int id, int power, int charge):u_ind(id),u_charge(charge),u_power(power){}
	int getId()const{
		return u_ind;
	}

	int getPower()const{
		return u_power;
	}

	int getCharge()const{
		return u_charge;
	}

	void setPower(int power){
		u_power = power;
	}

private:
	int u_ind; //玩家唯一id
	int u_power; //玩家战力
	int u_charge; //玩家充值数量

	//重载输出
	friend std::ostream& operator<<(std::ostream& os,const boost::shared_ptr<PlayrInfo> & e)
	{
		//获取引用计数
		//cout << e.use_count();
		//获取原始指针
		//e.get()
		os<<e->u_ind<<" "<<e->u_power<<" "<<e->u_charge<<std::endl;
		return os;
	}
};

struct powerOperator
{
	bool operator()(int a, int b) const
	{
		return a > b;
	}
};

struct by_id;
struct by_power;
struct by_charge;
typedef multi_index_container<
	boost::shared_ptr<PlayrInfo>, //插入的主体，当然可以是PlayrInfo本身
	indexed_by<
	//唯一主键，不可重复, std::greater<int> 为id规则从大到小，默认从小到大
	//tag<by_id>为标签名，可写可不写
	ordered_unique<tag<by_id> , const_mem_fun<PlayrInfo,int,&PlayrInfo::getId>>,

	//可重复的主键
	ordered_non_unique<tag<by_power>, const_mem_fun<PlayrInfo, int, &PlayrInfo::getPower>, powerOperator >,
	ordered_non_unique<tag<by_charge>, const_mem_fun<PlayrInfo, int, &PlayrInfo::getCharge>, greater<int>  >
	>
> PlayerContainer;




int main(int argc, char* argv[])
{
	PlayerContainer con;
	con.insert(boost::make_shared<PlayrInfo>(1, 1231, 10000));
	con.insert(boost::make_shared<PlayrInfo>(2, 22222, 2000));
	con.insert(boost::make_shared<PlayrInfo>(3, 19999, 222222));

	
	auto& ids = con.get<by_id>();
	
	auto& powers = con.get<by_power>();
	
	//删除唯一主键为2的记录
	
	auto &itfind = ids.find(2);
	
	if (itfind != ids.end()){
		ids.erase(itfind);
		cout << "after erase: "<<endl;
		copy(con.begin(), con.end(), ostream_iterator<boost::shared_ptr<PlayrInfo> >(cout));
	}

	con.insert(boost::make_shared<PlayrInfo>(4, 1231, 10000));
	auto &beginit = powers.lower_bound(1231);
	auto & endit = powers.upper_bound(1231);
	for( ; beginit != powers.end()&& beginit != endit; ){
		cout << "...................." <<endl;
		cout << *beginit;
		beginit = powers.erase(beginit);
	}
	
	copy(powers.begin(), powers.end(), ostream_iterator<boost::shared_ptr<PlayrInfo> >(cout));
	auto piter = powers.find(19999);
	if(piter != powers.end() ){
		auto newvalue = boost::make_shared<PlayrInfo>(100,200,300);
		powers.replace(piter, newvalue);
		copy(powers.begin(), powers.end(), ostream_iterator<boost::shared_ptr<PlayrInfo> >(cout));
	}

	piter = powers.find(200);
	if(piter != powers.end()){
		auto newp = 1024;
		powers.modify(piter, [&](boost::shared_ptr<PlayrInfo> & playptr)->void{
			playptr->setPower(newp) ;
		});
		copy(powers.begin(), powers.end(), ostream_iterator<boost::shared_ptr<PlayrInfo> >(cout));
	}

	getchar();
	return 0;
}