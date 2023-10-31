#include "receiver.h"
#include <vector>
using namespace std;

class company
{   
    public:
    string name;
    int price;
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

        // for (company c: stocks)
        for (int i = 0; i<stocks.size(); ++i)
        {
            if (stocks[i].name == name)
            {
                found = 1;
                if (type == 'b')
                {
                    if (integer_price > stocks[i].price)
                    {
                        cout<<name<<" "<<integer_price<<" "<<'s'<<"\r"<<endl;
                        company c;
                        c.name = name;
                        c.price = integer_price;
                        stocks[i] = c;
                        // for(auto u: stocks){
                        // cout<<u.name<<" "<<u.price<<endl;}
                    }

                    else
                    {
                        cout<<"No Trade"<<"\r"<<endl;
                    }
                }
                else if (type == 's')
                {
                    if (integer_price < stocks[i].price)
                    {
                        cout<<name<<" "<<integer_price<<" "<<'b'<<"\r"<<endl;
                        company c;
                        c.name = name;
                        c.price = integer_price;
                        stocks[i] = c;
                        // for(auto u: stocks){
                        // cout<<u.name<<" "<<u.price<<endl;}
                    }
                    else
                    {
                        cout<<"No Trade"<<"\r"<<endl;
                    }
                }
                break;
            }
        }
        if (!found)
        {
            if (type == 'b')
            {
                cout<<name<<" "<<integer_price<<" "<<'s'<<"\r"<<endl;
                company c;
                c.name = name;
                c.price = integer_price;
                stocks.push_back(c);
            }
            else if (type == 's')
            {
                cout<<name<<" "<<integer_price<<" "<<'b'<<"\r"<<endl;
                company c;
                c.name = name;
                c.price = integer_price;
                stocks.push_back(c);
            }
        }
    }    
}

int main() {

    Receiver rcv;
    // sleep(5);

    while (true)
    {
        string message = rcv.readIML();
        auto endmarker = message.end();
        --endmarker;
        process(message);
        if (*endmarker == '$') break;
    }

    return 0;
}