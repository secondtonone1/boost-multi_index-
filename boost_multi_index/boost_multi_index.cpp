// boost_multi_index.cpp : �������̨Ӧ�ó������ڵ㡣
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

	//�������
	friend std::ostream& operator<<(std::ostream& os,const boost::shared_ptr<Book> & e)
	{
		//��ȡ���ü���
		//cout << e.use_count();
		//��ȡԭʼָ��
		//e.get()
		os<<e->id<<" "<<e->date<<" "<<e->name<<" "<<e->author<<std::endl;
		//os<<e.id<<" "<<e.date<<" "<<e.name<<" "<<e.author<<std::endl;
		return os;
	}

};

//Compare�࣬�Զ���ȽϹ���
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
	boost::shared_ptr<Book>, //��������壬��Ȼ������Book����
	indexed_by<
		//Ψһ�����������ظ�, std::greater<int> Ϊid����Ӵ�С��Ĭ�ϴ�С����
		//tag<by_id>Ϊ��ǩ������д�ɲ�д
		ordered_unique<tag<by_id> , member<Book,int,&Book::id>, std::greater<int> >,
		
		//�����дtag�����Զ�������
		//ordered_unique<member<Book,int,&Book::id>, std::less<int> >,

		//���ظ�������
		ordered_non_unique<tag<by_date>, member<Book, int, &Book::date> >,
		ordered_non_unique<tag<by_name>, member<Book, string, &Book::name> >,
		
		//��������˱ȽϹ��򣬵��Ƚ�����Ϊ�������ͻ����Զ���ȽϹ���ʱ����Ҫ�Լ�д�ȽϺ�����
		//�����ض���ȽϹ���ʵ�ֽ�������
		ordered_non_unique<tag<by_author>, const_mem_fun<Book, const string, &Book::getAuthor>,Compare<string>  >
		
		//Ҳ����ȥ���ȽϹ���,��������
		//ordered_non_unique< const_mem_fun<Book, const string, &Book::getAuthor>  >

	>
> BookContainer;

// �޸�ѧ������ĺ�������  
class CModifyBookName  
{  
public:  
	CModifyBookName(std::string name) : m_name(name){}  
	
	void operator()(boost::shared_ptr<Book>& bookptr)  
	{  
		// �޸�����
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
	
	//���ַ�ʽ
	//1 ͨ��get<��������>��ȡĳһ�����µ�����, ����ֵһ��Ҫ�����ý��գ����򱨴�
	typedef BookContainer::nth_index<0>::type Id_Index;
	Id_Index& id_idx = con.get<0>();

	for (auto iter = id_idx.begin(); iter != id_idx.end(); iter++)
	{
			 cout << iter->get()->id << "  "
			        << iter->get()->date << "  "
				    << iter->get()->name << "  "
				    << iter->get()->author << endl;
	  }

	//2 ͨ��get<by_id> ��ȡĳһ�����µ�����, by_idΪ֮ǰ����õı�ǩ

	//Id_Index& id_idx2 = con.get<by_id>(); ������auto�﷨
	//����ֵһ��Ҫ�����ý��գ����򱨴�
	auto &id_idx2 = con.get<by_id>();
	for (auto iter = id_idx2.begin(); iter != id_idx2.end(); iter++)
	{
		cout << iter->get()->id << "  "
			<< iter->get()->date << "  "
			<< iter->get()->name << "  "
			<< iter->get()->author << endl;
	}

	//���������ص�<< ���
	auto & authors = con.get<by_author>();
	copy(authors.begin(), authors.end(), ostream_iterator<boost::shared_ptr<Book> >(cout));
	cout << endl;

	//ɾ��Ψһ����Ϊ2�ļ�¼
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
	//�޸�modify��replace�Կ���
	//replace ʧ�ܲ���ɾ����Ŀ�����Ƕ���copy���Ч�ʵ��� 
	if(itname != names.end()){
		cout << "found name : "<< itname->get()->name <<endl;
		auto newvalue = boost::make_shared<Book>(itname->get()->id,itname->get()->date,"music book",itname->get()->author);
		names.replace(itname, newvalue);
		copy(names.begin(), names.end(), ostream_iterator<boost::shared_ptr<Book> >(cout));
	}

	//modifyʧ�ܻ�ɾ����Ӧ��Ŀ�����ױ�����ɾ��������Ч�ʸ�
	//modifyҪ��ȷ���ҵ�����Ŀ���޸�
	
	itname = names.find("music book");
	if(itname != names.end()){
		cout << "found name : "<< itname->get()->name <<endl;
		names.modify(itname, CModifyBookName("english book"));
		copy(names.begin(), names.end(), ostream_iterator<boost::shared_ptr<Book> >(cout));
	}

	//modify �ڶ���д��
	itname = names.find("music book");
	if(itname != names.end()){
		cout << "found name : "<< itname->get()->name <<endl;
		auto &newname = "english book";
		names.modify(itname, [&](boost::shared_ptr<Book> & bookptr)->void{
			bookptr->name = newname;
		});
		copy(names.begin(), names.end(), ostream_iterator<boost::shared_ptr<Book> >(cout));
	}

	//������������Ϊ"english book"��Ԫ��
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

