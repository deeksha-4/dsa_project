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
                    // if (stocks[i].waiting_sell_price!=-1 && stocks[i].waiting_sell_price == integer_price)
                    // {
                    //     stocks[i].waiting_sell_price = -1;
                    //     cout<<"No Trade\r"<<endl;
                    //     if (stocks[i].waiting_buy_price < integer_price)
                    //     {
                    //         stocks[i].waiting_buy_price = -1;
                    //     }
                    //     break;
                    // }
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

                    // if (stocks[i].waiting_buy_price!=-1 && stocks[i].waiting_buy_price == integer_price)
                    // {
                    //     stocks[i].waiting_buy_price = -1;
                    //     cout<<"No Trade\r"<<endl;
                    //     if (stocks[i].waiting_sell_price > integer_price)
                    //     {
                    //         stocks[i].waiting_sell_price = -1;
                    //     }
                    //     break;
                    // }
                    
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

vector<string>cname;

class arbitorder{
    public:
    vector<int> quantity;
    int netprice;
    char otype;
    int waiting_sellprice;
    bool waitingsellp;
    int waiting_buyprice;
    bool waitingbuyp;
};

vector<arbitorder> arbitrage;

void calcSubset(int& A, vector<vector<int> >& res, vector<int>& subset, int index)
{
    res.push_back(subset);
    for (int i = index; i < A; i++) 
    {
        subset.push_back(i);
        calcSubset(A, res, subset, i + 1);
        subset.pop_back();
    }
}

vector<vector<int>> subsets(int A)
{
    vector<int> subset;
    vector<vector<int>> res;
    int index = 0;
    calcSubset(A, res, subset, index);
    return res;
}

class possible_arbitage{
    public:
    vector<int> indices;
    int price;
};

void processtwo(string message)
{
    vector<string> orders = extract_orders(message);
    orders[0] = previo + orders[0];
    previo = preprevio;
    bool profit0 = 1;
    for (auto order : orders)
    {
        vector<possible_arbitage> arbitrage_array;
        string name, netprice;
        vector<int> quantity(cname.size(),0);
        char type;
        bool arbitchance=1;
        auto it = order.begin();
        auto ite = order.end()-2;
        type = *ite;
        ite = ite-2;
        string temp;

        while(*ite != ' ')
        {
            temp.push_back(*ite);
            ite--;
        }

        for(int i = temp.size(); i > 0; i--)
        {
            netprice.push_back(temp[i-1]);
        }
        int nprice = stoi(netprice);
        
        while(it < ite)
        {   
            string tempname;
            while(*it != ' ')
            {
                tempname.push_back(*it);
                it++;
            }
            int namefound = -1;
            for(int i = 0; i < cname.size(); i++)
            {
                if(cname[i] == tempname)
                {
                    namefound = i;
                    break;
                }
            }
            it++;
            string tempq;
            while(*it != ' ')
            {   
                tempq.push_back(*it);
                it++;
            }
            if (namefound == -1)
            {
                cname.push_back(tempname);
                arbitchance = 0;
                quantity.push_back(stoi(tempq));
            }
            else
            { 
                quantity[namefound] = stoi(tempq);
            }
            it++;
        }
        
        if(arbitchance == 0)
        {   
            arbitorder newa;
            newa.quantity = quantity;
            newa.netprice = nprice;
            newa.otype = type;
            arbitrage.push_back(newa);
            cout<<"No Trade"<<"\r"<<endl;
            for(int l = 0; l < arbitrage.size(); l++)
            {   
                while(arbitrage[l].quantity.size()<cname.size()){
                    arbitrage[l].quantity.push_back(0);
                }
            }
        }
        else
        {   
            vector<vector<int>> res = subsets(arbitrage.size());
            bool sumbool2 =0; 

            for (int i = 0; i < res.size(); i++) 
            {
                vector<int> sum(cname.size(),0);
                bool arbitadded = 0;
                bool sumbool = 0;
                int profit = 0;
                for(int k = 0 ; k < cname.size(); k++)
                {
                    for (int j = 0; j < res[i].size(); j++)
                    {
                        sum[k] += arbitrage[res[i][j]].quantity[k];
                    }
                    if(sum[k] + quantity[k]!=0)
                    {
                        sumbool = 1; 
                        break;
                    }
                }
                if (sumbool) continue;

                for (int j = 0; j < res[i].size(); j++)
                {
                    if (arbitrage[res[i][j]].otype == 'b') profit += arbitrage[res[i][j]].netprice;
                    else if (arbitrage[res[i][j]].otype == 's') profit -= arbitrage[res[i][j]].netprice;
                }

                if(type == 'b') profit += nprice;
                else if(type == 's') profit -= nprice;
                if (profit > 0) 
                {
                    sumbool2 = 1; 
                    possible_arbitage newpa;
                    newpa.indices = res[i];
                    newpa.price = profit;
                    arbitrage_array.push_back(newpa);
                }
            }

            if (sumbool2 == 0)
            {
                arbitorder newa;
                newa.quantity = quantity;
                newa.netprice = nprice;
                newa.otype = type;
                arbitrage.push_back(newa);
                cout<<"No Trade"<<"\r"<<endl;
            }
        

        int maxprofit = 0;
        int index;

        if (arbitrage_array.size()==0) continue;
        else{profit0 = 0;}

        for (int l = 0; l < arbitrage_array.size(); l++)
        {
            if(arbitrage_array[l].price>maxprofit)
            {
                maxprofit=arbitrage_array[l].price;
                index=l;
            }
        }

        for(int p = 0; p < cname.size(); p++)
        {
            if(quantity[p] != 0)
            {
                cout<<cname[p]<<" "<<quantity[p]<<" ";
            }
        }
        if(type == 's'){
            type='b';
        }
        else{
            type='s';
        }
        cout<<nprice<<" "<<type<<"\r"<<endl;

        for (int k = arbitrage_array[index].indices.size()-1; k >= 0; k--)
        {
            for (int p = 0; p < cname.size(); p++)
            {
                if (arbitrage[k].quantity[p] != 0) {
                    cout<<cname[p]<<" "<<arbitrage[k].quantity[p]<<" ";
                } 
            }
            if(arbitrage[k].otype == 's'){
                arbitrage[k].otype='b';
            }
            else{
                arbitrage[k].otype='s';
            }
            cout<<arbitrage[k].netprice<<" "<<arbitrage[k].otype<<"\r"<<endl;
            arbitrage.erase(arbitrage.begin() + arbitrage_array[index].indices[k]);
        }
        cout<<maxprofit<<"\r"<<endl;
    }
    }
    if(profit0){cout<<0;}
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
<<<<<<< HEAD
            while (true)
=======
            while(true)
>>>>>>> 95aee57f0586b9053335b96934e99be38abee67e
            {
                string message = rcv.readIML();
                auto endmarker = message.end();
                --endmarker;
                processtwo(message);
                if (*endmarker == '$') break;
            }
            break;

        case 3:
            // part3 
            break;
    }
    
    return 0;
}
