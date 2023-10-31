#include "receiver.h"
#include <vector>
using namespace std;

class company
{   
    public:
    string name;
    int price;
    int waiting_sell_price; // -1 indicates no waiitng order
    int waiting_buy_price;
};

string previo, preprevio;
vector<company> stocks;

vector<string> extract_orders(string message)
{
    vector<string> orders;
    int start = 0;
    while(start < message.length())
    {
        int end = message.find('#', start);
        if (end >= message.length()-1)
        {
            orders.push_back(message.substr(start));
            break;
        }
        else
        {
            orders.push_back(message.substr(start, end - start + 1));
            start = end + 2;
        }
    }
    string last = orders.back();
    if (*(--last.end())!='#')
    {
        preprevio = last;
        orders.pop_back();
    }
    else
    {
        preprevio = "";
    }
    return orders;
}

void process(string message)
{
    vector<string> orders = extract_orders(message);
    orders[0] = previo + orders[0];
    previo = preprevio;

    for (auto order : orders)
    {   
    //     for(auto u: stocks){
    //     cout<<u.name<<endl<<u.waiting_buy_price<<endl<<u.waiting_sell_price<<endl<<u.price<<endl;
    // }
        string name, price;
        char type;
        auto it = order.begin();
        while(*it != ' ')
        {
            name.push_back(*it);
            it++;
        }
        it++;
        while(*it != ' ')
        {
            price.push_back(*it);
            it++;
        }
        it++;
        type = *it;

        int integer_price = stoi(price);
        bool found = 0;

        
        for (int i = 0; i < stocks.size(); ++i)
        {
            if (stocks[i].name == name)
            {
                found = 1;
                if (type == 'b')
                {

                    if (stocks[i].waiting_buy_price!=-1 && (stocks[i].waiting_buy_price >= integer_price))
                    {
                        cout<<"No Trade\r"<<endl;
                        break;
                    }
                    else if (stocks[i].waiting_buy_price!=-1 && (stocks[i].waiting_buy_price < integer_price))
                    {
                        stocks[i].waiting_buy_price = integer_price;

                    }
                    if (stocks[i].waiting_sell_price!=-1 && stocks[i].waiting_sell_price == integer_price)
                    {
                        stocks[i].waiting_buy_price = -1;
                        stocks[i].waiting_sell_price = -1;
                        cout<<"No Trade\r"<<endl;
                        break;
                    }
                    if (stocks[i].price < integer_price)
                    {
                        cout<<name<<" "<<integer_price<<" "<<'s'<<"\r"<<endl;
                        stocks[i].price = integer_price;
                        stocks[i].waiting_buy_price = -1;
                        break;
                    }
                    else if (stocks[i].price >= integer_price)
                    {
                        stocks[i].waiting_buy_price = integer_price;
                        cout<<"No Trade"<<"\r"<<endl;
                        break;
                    }
                    
                }

                else if (type == 's')
                {

                    if (stocks[i].waiting_sell_price!=-1 && (stocks[i].waiting_sell_price <= integer_price))
                    {
                        cout<<"No Trade\r"<<endl;
                        break;
                    }
                    else if (stocks[i].waiting_sell_price!=-1 && (stocks[i].waiting_sell_price > integer_price))
                    {
                        stocks[i].waiting_sell_price = integer_price;

                    }
                    if (stocks[i].waiting_buy_price!=-1 && stocks[i].waiting_buy_price == integer_price)
                    {
                        stocks[i].waiting_buy_price = -1;
                        stocks[i].waiting_sell_price = -1;
                        cout<<"No Trade\r"<<endl;
                        break;
                    }
                    if (stocks[i].price > integer_price)
                    {
                        cout<<name<<" "<<integer_price<<" "<<'b'<<"\r"<<endl;
                        stocks[i].price = integer_price;
                        stocks[i].waiting_sell_price = -1;
                        break;
                    }
                    else if (stocks[i].price <= integer_price)
                    {   
                        stocks[i].waiting_sell_price = integer_price;
                        cout<<"No Trade"<<"\r"<<endl;
                        break;
                    }
                    
                }
            }
        }

        if (!found)
        {
            if (type == 'b')
            {
                cout<<name<<" "<<integer_price<<" "<<'s'<<"\r"<<endl;
            }
            else if (type == 's')
            {
                cout<<name<<" "<<integer_price<<" "<<'b'<<"\r"<<endl;
            }
            company c;
            c.name = name;
            c.price = integer_price;
            c.waiting_buy_price = -1;
            c.waiting_sell_price = -1;
            stocks.push_back(c);            
        }
    }    
}

int main(int argc, char* argv[]) {

    Receiver rcv;
    // sleep(5);

    int choice = stoi(argv[1]);

    switch (choice){

        case 1:
            while (true)
            {
                string message = rcv.readIML();
                auto endmarker = message.end();
                --endmarker;
                process(message);
                if (*endmarker == '$') break;
            }
            break;
        

        case 2:
            // arbitrage
            break;

        case 3:
            // part3 
            break;
    }
    

    return 0;
}