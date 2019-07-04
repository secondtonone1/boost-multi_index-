// boost_multi_index.cpp : 定义控制台应用程序的入口点。
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

struct Book{
	int id;
	int date;
	string name;
	string author;
	Book(int id_, int date_, string name_, string author_){
		id = id_;
		date = date_;
		name = name_;
		author = author_;
	}
	
	const string getAuthor() const{
		return author;
	}

	//重载输出
	friend std::ostream& operator<<(std::ostream& os,const boost::shared_ptr<Book> & e)
	{
		//获取引用计数
		//cout << e.use_count();
		//获取原始指针
		//e.get()
		os<<e->id<<" "<<e->date<<" "<<e->name<<" "<<e->author<<std::endl;
		//os<<e.id<<" "<<e.date<<" "<<e.name<<" "<<e.author<<std::endl;
		return os;
	}

};

//Compare类，自定义比较规则
template <class T>
struct Compare
{
	bool operator()(T a, T b) const
	{
		return a > b;
	}
};
struct by_id;
struct by_date;
struct by_name;
struct by_author;
typedef multi_index_container<
	boost::shared_ptr<Book>, //插入的主体，当然可以是Book本身
	indexed_by<
		//唯一主键，不可重复, std::greater<int> 为id规则从大到小，默认从小到大
		//tag<by_id>为标签名，可写可不写
		ordered_unique<tag<by_id> , member<Book,int,&Book::id>, std::greater<int> >,
		
		//如果不写tag，可以定义如下
		//ordered_unique<member<Book,int,&Book::id>, std::less<int> >,

		//可重复的主键
		ordered_non_unique<tag<by_date>, member<Book, int, &Book::date> >,
		ordered_non_unique<tag<by_name>, member<Book, string, &Book::name> >,
		
		//这里添加了比较规则，当比较类型为复合类型或者自定义比较规则时，需要自己写比较函数。
		//这里重定义比较规则实现降序排序
		ordered_non_unique<tag<by_author>, const_mem_fun<Book, const string, &Book::getAuthor>,Compare<string>  >
		
		//也可以去掉比较规则,升序排序
		//ordered_non_unique< const_mem_fun<Book, const string, &Book::getAuthor>  >

	>
> BookContainer;

// 修改学生年龄的函数对象  
class CModifyBookName  
{  
public:  
	CModifyBookName(std::string name) : m_name(name){}  
	
	void operator()(boost::shared_ptr<Book>& bookptr)  
	{  
		// 修改年龄
		bookptr->name = m_name;
	}  
	
private:  
	std::string  m_name;  
};  


int main(int argc, char* argv[])
{
	BookContainer con;
	con.insert(boost::make_shared<Book>(0, 2011, "math book", "jim"));
	con.insert(boost::make_shared<Book>(2, 2008, "chinese book", "jam"));
	con.insert(boost::make_shared<Book>(1, 2005, "english book", "roland"));
	con.insert(boost::make_shared<Book>(3, 2010, "music book", "rose"));
	
	//两种方式
	//1 通过get<数字索引>获取某一主键下的序列, 返回值一定要用引用接收，否则报错
	typedef BookContainer::nth_index<0>::type Id_Index;
	Id_Index& id_idx = con.get<0>();

	for (auto iter = id_idx.begin(); iter != id_idx.end(); iter++)
	{
			 cout << iter->get()->id << "  "
			        << iter->get()->date << "  "
				    << iter->get()->name << "  "
				    << iter->get()->author << endl;
	  }

	//2 通过get<by_id> 获取某一主键下的序列, by_id为之前定义好的标签

	//Id_Index& id_idx2 = con.get<by_id>(); 可以用auto语法
	//返回值一定要用引用接收，否则报错
	auto &id_idx2 = con.get<by_id>();
	for (auto iter = id_idx2.begin(); iter != id_idx2.end(); iter++)
	{
		cout << iter->get()->id << "  "
			<< iter->get()->date << "  "
			<< iter->get()->name << "  "
			<< iter->get()->author << endl;
	}

	//可以用重载的<< 输出
	auto & authors = con.get<by_author>();
	copy(authors.begin(), authors.end(), ostream_iterator<boost::shared_ptr<Book> >(cout));
	cout << endl;

	//删除唯一主键为2的记录
	BookContainer::index<by_id>::type & ids = con.get<by_id>();
	/*BookContainer::index<by_id>::type::iterator*/auto &itfind = ids.find(2);
	if (itfind != ids.end()){
		cout << "found id: "<<itfind->get()->id<< endl;	
		con.erase(itfind);
		cout << "after erase: "<<endl;
		copy(ids.begin(), ids.end(), ostream_iterator<boost::shared_ptr<Book> >(cout));
	}

	
	BookContainer::index<by_name>::type & names = con.get<by_name>();
	auto itname = names.find("english book");
	//修改modify和replace皆可以
	//replace 失败不会删除条目，但是二次copy造成效率低下 
	if(itname != names.end()){
		cout << "found name : "<< itname->get()->name <<endl;
		auto newvalue = boost::make_shared<Book>(itname->get()->id,itname->get()->date,"music book",itname->get()->author);
		names.replace(itname, newvalue);
		copy(names.begin(), names.end(), ostream_iterator<boost::shared_ptr<Book> >(cout));
	}

	//modify失败会删除对应条目，容易暴力误删除，但是效率高
	//modify要在确定找到该条目后修改
	
	itname = names.find("music book");
	if(itname != names.end()){
		cout << "found name : "<< itname->get()->name <<endl;
		names.modify(itname, CModifyBookName("english book"));
		copy(names.begin(), names.end(), ostream_iterator<boost::shared_ptr<Book> >(cout));
	}

	//modify 第二种写法
	itname = names.find("music book");
	if(itname != names.end()){
		cout << "found name : "<< itname->get()->name <<endl;
		auto &newname = "english book";
		names.modify(itname, [&](boost::shared_ptr<Book> & bookptr)->void{
			bookptr->name = newname;
		});
		copy(names.begin(), names.end(), ostream_iterator<boost::shared_ptr<Book> >(cout));
	}

	//查找所有名字为"english book"的元素
	auto beginit = names.lower_bound("english book");
	auto endit = names.upper_bound("english book");
	for( ; beginit != names.end()&& beginit != endit; ){
		cout << "...................." <<endl;
		cout << *beginit;
		beginit = names.erase(beginit);
	}
	copy(names.begin(), names.end(), ostream_iterator<boost::shared_ptr<Book> >(cout));
	cout << "????????????????????"<<endl;
	con.insert(boost::make_shared<Book>(7, 2011, "math book", "jim"));
	con.insert(boost::make_shared<Book>(8, 2011, "math book", "jim"));
	
	auto beginit2 = names.lower_bound("math book");
	auto endit2 = names.upper_bound("math book");
	for( ; beginit2 != names.end()&& beginit2 != endit2; beginit2++){
		cout << "...................." <<endl;
		cout << *beginit2;
		auto &newname = "english book";
		names.modify(beginit2, [&](boost::shared_ptr<Book> & bookptr)->void{
			bookptr->name = newname;
		});
	}
	copy(names.begin(), names.end(), ostream_iterator<boost::shared_ptr<Book> >(cout));
	getchar();
	return 0;
}

