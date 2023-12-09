// This is your trader. Place your orders from here
#include <sstream>
#include <fstream> 
#include <string>
#include <iostream>
#include <atomic>
#include <vector>
#include <mutex>
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

vector<vector<int>> ind_quantity;

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

vector<int>netquantity;
vector<string> dname;

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

class median
{
    public:
    vector<Stock> items;
    vector<int> prev_trades;
};

extern std::atomic<int> commonTimer;
extern std::mutex printMutex; 

int n = 0;
// no of lines already read, so that i consider only new orders

Order to_order(string line)
{
    Order new_order;
    string s;
    stringstream ss(line);

    vector<string> v;
    while (getline(ss, s, ' ')) {
        v.push_back(s);
    }

    new_order.start = stoi(v[0]);
    if (*(--v.end()) == "-1") new_order.end = -1;
    else new_order.end = stoi(*(--v.end())) + new_order.start;
    new_order.name = v[1];
    int n = v.size();
    v[n-2].erase(v[n-2].begin());
    new_order.quantity = stoi(v[n-2]);
    v[n-3].erase(v[n-3].begin());
    new_order.price = stoi(v[n-3]);
    new_order.type = v[2][0];

    int m = n-6; // size of lincomb

    if (m==1)
    {
        Stock s;
        s.name = v[3];
        s.quantity = 1;
        new_order.items.push_back(s);
    }

    else
    {
        int x = 3;
        while(x<m+3)
        {
            Stock s;
            s.name = v[x];
            s.quantity = stoi(v[x+1]);
            x=x+2;
            new_order.items.push_back(s);
        }            
    }
    return new_order;
}

vector<Order> orderBook;
vector<median> prices;

int binarySearch(vector<int> arr, int item, int low, int high) {
    while (low < high) {
        int mid = low + (high - low) / 2;

        if (item == arr[mid]) {
            return mid + 1;
        } else if (item > arr[mid]) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return (item > arr[low]) ? (low + 1) : low;
}


vector<vector<int>> createsubsets(const vector<int>& netq) {
    vector<vector<int>> new_possible_arbitrage;
    for (int i = 0; i < netq.size(); i++) {
        for (int j = i + 1; j < netq.size(); j++) {
            for (int m = 1; m <= netq[i]; m++) {
                for (int n = 1; n <= netq[j]; n++) {
                    vector<int> subset(netq.size(), 0);
                    subset[i] = m;
                    subset[j] = n;
                    new_possible_arbitrage.push_back(subset);
                }
            }
        }
    }
    return new_possible_arbitrage;
}


int projected_profit = 0;

void update(int n, vector<string> v)
{
    // now i consider orders in orderbook + v[n:]
    for (int z = n; z<v.size(); ++z)
    {
        int potato = -1;
        Order new_order = to_order(v[z]);
        vector<int> quantity(dname.size(),0);
        vector<candidate> possible_matches;

        for (int i = 0; i<orderBook.size(); ++i)
        {
            if (orderBook[i].end >= 0 && orderBook[i].end < new_order.start) continue;
            if (orderBook[i].type != new_order.type)
            {
                if (new_order.items == orderBook[i].items)
                {
                    if (new_order.type == 'B')
                    {        
                        if (orderBook[i].price <= new_order.price)
                        {
                            candidate c;
                            c.index = i;
                            c.order = orderBook[i];
                            possible_matches.push_back(c);                     
                        }
                    }
                    else
                    {                        
                        if (orderBook[i].price >= new_order.price)
                        {
                            candidate c;
                            c.index = i;
                            c.order = orderBook[i];
                            possible_matches.push_back(c);                     
                        }
                    }
                }
            }
        }
        // now possible_matches has all the orders that could cancel mine

        if (new_order.type =='B') sort(possible_matches.begin(), possible_matches.end(), buy_key());
        else if (new_order.type =='S') sort(possible_matches.begin(), possible_matches.end(), sell_key());

        bool flag = 0;
        vector<int> indices_to_delete;

        for (int i = 0; i<possible_matches.size(); ++i)
        {
            if (flag) break;
            if (possible_matches[i].order.quantity >= new_order.quantity)
            {            
                flag = 1;             
                orderBook[possible_matches[i].index].quantity -= new_order.quantity;             
                new_order.quantity = 0;
            }
            else
            {                
                new_order.quantity -= possible_matches[i].order.quantity;
                if (new_order.quantity == 0) flag = 1;
                orderBook[possible_matches[i].index].quantity = 0;
            }
            bool fg = 0;
            for (int k = 0; k<prices.size(); ++k)
            {
                if (prices[k].items == possible_matches[i].order.items)
                {
                    fg = 1;
                    prices[k].prev_trades.insert(prices[k].prev_trades.begin()+binarySearch(prices[k].prev_trades, possible_matches[i].order.price, 0, prices[k].prev_trades.size()-1), possible_matches[i].order.price);
                    break;
                }
            }
            if (!fg)
            {
                median m;
                m.items = possible_matches[i].order.items;
                m.prev_trades = {possible_matches[i].order.price};
                prices.push_back(m);
            }
        }

        if (!new_order.quantity) continue;
        bool traded = 0;
        for (int k = 0; k<prices.size(); ++k)
        {
            if (new_order.items.size()==1)
            {
                if (prices[k].items == new_order.items)
                {     
                    potato = k;                  
                    int med = prices[k].prev_trades[prices[k].prev_trades.size()/2];
                    if (new_order.type == 'B')
                    {
                        if (new_order.price > med)
                        {                        
                            prices[k].prev_trades.insert(prices[k].prev_trades.begin()+binarySearch(prices[k].prev_trades, new_order.price, 0, prices[k].prev_trades.size()-1), new_order.price);
                            int currentTime;
                            {
                                currentTime = commonTimer.load();
                            }
                            {std::lock_guard<std::mutex> lock(printMutex);
                            projected_profit += new_order.quantity*(new_order.price - med);
                            std::cout << currentTime << " " << "22B0943_22B0988 SELL ";
                            traded = 1;
                            if (new_order.items[0].quantity == 1)  cout<<new_order.items[0].name<<" ";
                            else
                            {
                                for (auto g: new_order.items)
                                {
                                    cout<<g.name<<" "<<g.quantity<<" ";
                                }
                            }
                            cout<<"$"<<new_order.price<<" #"<<new_order.quantity<<" ";
                            if (new_order.end == -1) cout<<-1<<endl;
                            else cout<<new_order.end - currentTime<<endl;}                            
                        }
                    }
                    else
                    {
                        if (new_order.price < med)
                        {                        
                            prices[k].prev_trades.insert(prices[k].prev_trades.begin()+binarySearch(prices[k].prev_trades, new_order.price, 0, prices[k].prev_trades.size()-1), new_order.price);                       
                            
                            int currentTime;
                            {
                                currentTime = commonTimer.load();
                            }
                            {std::lock_guard<std::mutex> lock(printMutex);
                            projected_profit -= new_order.quantity*(new_order.price - med);
                            std::cout << currentTime << " " << "22B0943_22B0988 BUY ";
                            traded = 1;
                            if (new_order.items[0].quantity == 1)  cout<<new_order.items[0].name<<" ";
                            else
                            {
                                for (auto g: new_order.items)
                                {
                                    cout<<g.name<<" "<<g.quantity<<" ";
                                }
                            }
                            cout<<"$"<<new_order.price<<" #"<<new_order.quantity<<" ";
                            if (new_order.end == -1) cout<<-1<<endl;
                            else cout<<new_order.end - currentTime<<endl;}                          
                        }                        
                    }
                }
            }
        }
        if (!traded)
        {   
            for (int p =0; p< new_order.items.size(); p++)
            {
                int namefound = -1;
                for(int m = 0; m < dname.size(); m++)
                {
                    if(dname[m] == new_order.items[p].name)
                    {
                        namefound = m;
                        quantity[m] = new_order.items[p].quantity;
                        break;
                    }
                }
                if(namefound = -1)
                {
                    quantity.push_back(new_order.items[p].quantity);
                    dname.push_back(new_order.items[p].name);
                }
            }
            for(int l = 0; l < orderBook.size(); l++)
            {   
                while(ind_quantity[l].size() < dname.size())
                {
                   ind_quantity[l].push_back(0);
                }
            }
            if(netquantity.size() >= 2)
            {
                vector<vector<int>> res = createsubsets(netquantity);
                for (int i = 0; i < res.size(); i++)
                {
                    vector<int> sum(dname.size(),0);
                    bool sumbool = 0;
                    vector<int> nq;
                    for (int l=0; l < new_order.quantity; l++)
                    {
                        for (int j = 0; j < orderBook.size(); j++)
                        {   
                            if(orderBook[j].type =='B')
                            {
                                sum[0] += res[i][j]*ind_quantity[j][0];
                            }
                            else
                            {
                                sum[0] -= res[i][j]*ind_quantity[j][0];
                            }
                        }
                        if(new_order.type == 'B')
                        {
                            if(sum[0] == -quantity[0]*l){nq.push_back(l);}
                        }
                        if(new_order.type == 'S')
                        {
                            if(sum[0] == quantity[0]*l){nq.push_back(l);}
                        }
                    }
                    bool arbit_trade = 0;
                    int index = -1;
                    for(int l = nq.size()-1; l>=0 ; l--)
                    {
                        for(int k = 1; k<dname.size(); k++)
                        {
                            for (int j = 0; j < orderBook.size(); j++)
                            {   
                                if(orderBook[j].type =='B')
                                {
                                    sum[k] += res[i][j]*(ind_quantity[j][k]);
                                }
                                else
                                {
                                    sum[k] -= res[i][j]*(ind_quantity[j][k]);
                                }
                            }
                            if(new_order.type == 'B')
                            {
                                if(sum[k] == -quantity[k]*nq[l]){arbit_trade = 1;}
                                else{arbit_trade = 0; break;}
                            }
                            else if(new_order.type == 'S')
                            {
                                if(sum[k] == quantity[k]*nq[l]){arbit_trade = 1;}
                                else{arbit_trade = 0; break;}
                            }
                        }
                        if(arbit_trade == 1){index = l; break;}
                    }
                    if (index == -1) continue;
                    int profit = 0;
                    for(int j = 0; j < orderBook.size(); j++)
                    {
                        if (orderBook[j].type == 'B') profit += res[i][j]*orderBook[j].price;
                        else if (orderBook[j].type == 'S') profit -= res[i][j]*orderBook[j].price;
                    } 
                    if(new_order.type == 'B')
                    {
                        profit += new_order.price*index;
                    }
                    else if(new_order.type == 'S')
                    {
                        profit -= new_order.price*index;
                    }
                    if (profit > 0) 
                    {  
                        projected_profit += profit;
                        median new_trade;
                        vector<int> timevec;
                        timevec.push_back(new_order.end);
                        for (int j = 0; j < orderBook.size(); j++)
                        {
                            if(res[i][j] !=0)
                            {
                                timevec.push_back(orderBook[j].end);
                            }
                        }
                        int time = -1;
                        sort(timevec.begin(), timevec.end());
                        if (timevec[2]==-1){
                            time = -1;
                        }
                        else if (timevec[1]==-1){
                            time = timevec[2];
                        }
                        else if(timevec[0] ==-1){
                            time = timevec[1];
                        }
                        else{
                            time=timevec[0];
                        }
                        for (int j = 0; j < orderBook.size(); j++)
                        {   
                            if(res[i][j] !=0)
                            {
                                orderBook[j].quantity -= res[i][j];
                                int currentTime;
                                {
                                    currentTime = commonTimer.load();
                                }
                                std::lock_guard<std::mutex> lock(printMutex);
                                if(orderBook[j].type == 'B'){std::cout << currentTime << " " << "22B0943_22B0988 SELL ";}
                                else if(orderBook[j].type == 'S'){std::cout << currentTime << " " << "22B0943_22B0988 BUY ";}
                                if (orderBook[j].items[0].quantity == 1)  cout<<orderBook[j].items[0].name<<" ";
                                else
                                {
                                    for (auto g: orderBook[j].items)
                                    {
                                        cout<<g.name<<" "<<g.quantity<<" ";
                                    }
                                }
                                cout<<"$"<<orderBook[j].price<<" #"<<res[i][j]<<" ";
                                if(orderBook[j].quantity == 0)
                                {
                                    orderBook.erase(orderBook.begin()+j);
                                    netquantity.erase(netquantity.begin()+j);
                                }
                                if (time == -1) cout<<-1<<endl;
                                else cout<<time - currentTime<<endl;
                            }
                        }
                        if(new_order.items.size()==1)
                        {
                            if (potato == -1)
                            {
                                median m;
                                Stock s;
                                s.name = new_order.items[0].name;
                                s.quantity = 1;
                                m.items = {s};
                                m.prev_trades = {new_order.price};
                                prices.push_back(m);
                            }
                            else
                            {
                                prices[potato].prev_trades.insert(prices[potato].prev_trades.begin()+binarySearch(prices[potato].prev_trades, new_order.price, 0, prices[potato].prev_trades.size()-1), new_order.price);
                            }
                        }
                        int currentTime;
                        {
                            currentTime = commonTimer.load();
                        }
                        {std::lock_guard<std::mutex> lock(printMutex);
                        if(new_order.type == 'B'){std::cout << currentTime << " " << "22B0943_22B0988 SELL ";}
                        else if(new_order.type == 'S'){std::cout << currentTime << " " << "22B0943_22B0988 BUY ";}
                        std::cout << currentTime << " " << "22B0943_22B0988 BUY ";
                        if (new_order.items[0].quantity == 1)  cout<<new_order.items[0].name<<" ";
                        else
                        {
                            for (auto g: new_order.items)
                            {
                                cout<<g.name<<" "<<g.quantity<<" ";
                            }
                        }
                        cout<<"$"<<new_order.price<<" #"<<index<<" ";
                        new_order.quantity -= index;
                        if (time == -1) cout<<-1<<endl;
                        else cout<<time - currentTime<<endl;}     
                        break;
                    }
                }
            }
            if (new_order.quantity !=0)
            {
                ind_quantity.push_back(quantity);
                netquantity.push_back(new_order.quantity);
                orderBook.push_back(new_order);

            }
        }
    }
}

int reader(int time)
{
    const string ordersFileName = "output.txt";
    bool flag = true;
    while(flag)
    {
        sleep(1);
        ifstream inputFile(ordersFileName);
        string line;
    
        vector<string> v;
    
        while(getline(inputFile, line))
        {
            if (*(--line.end()) == '\r') line.pop_back();
            if (line == "!@") {flag = false; break;}
            if (line == "TL") {continue;}
            v.push_back(line);
        }
        inputFile.close();
        inputFile.clear();
        // now v has the set of all orders we have seen till now
        // relevant part is only from [n:]

        // process v
        update(n, v);

        n = v.size();

        int currentTime;
        {
            currentTime = commonTimer.load();
        }
    }
    return 1;
}

int trader(std::string *message)
{
    return 1;
}
