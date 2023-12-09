#include <atomic>
#include <mutex>
#include <vector>
#include <string>
#include <thread>

using namespace std;

extern std::atomic<int> commonTimer;
extern std::mutex printMutex;

class Order
{
	public:
    int start;
    int end;
    string name;
    char type;
    string stock;
    int price;
    int quantity;
};

class client
{
    public:
    string name;
    int id;
    vector<Order> orderBook;
};

class Trade
{
    int id;
    Order order;
};

class candidate
{
	public:
	Order order;
	int index;
};

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
