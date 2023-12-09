#ifndef MARKET_H
#define MARKET_H
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class Company
{
	public:
	string name;
	int buy;
	int sell;
	int net;
};

class Stock
{
	public:
    string name;
    int quantity;
};

class Order
{
	public:
    int start;
    int end;
    string name;
    char type;
    vector<Stock> items;
    int price;
    int quantity;
};

class candidate
{
	public:
	Order order;
	int index;
};

inline bool operator==(const Stock& s1, const Stock& s2)
{
	return (s1.name == s2.name && s1.quantity==s2.quantity);
}

class sell_key
{
	public:
    inline bool operator() (const candidate& order1, const candidate& order2)
    {
		if (order1.order.price != order2.order.price) return (order1.order.price > order2.order.price);
		if (order1.order.start != order2.order.start) return (order1.order.start < order2.order.start);
		int x = order1.order.name.compare(order2.order.name);
		if (x>=0) return 0;
		return 1;
    }
};

class buy_key
{
	public:
    inline bool operator() (const candidate& order1, const candidate& order2)
    {
		if (order1.order.price != order2.order.price) return (order1.order.price < order2.order.price);
		if (order1.order.start != order2.order.start) return (order1.order.start < order2.order.start);
		int x = order1.order.name.compare(order2.order.name);
		if (x>=0) return 0;
		return 1;
    }
};

class market
{
public:
	market(int argc, char** argv);
	void start();
	vector<Order> orderBook;
};
#endif
