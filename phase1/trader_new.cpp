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
    bool sell_flag = 0; // 1 indicates waiting order
    bool buy_flag = 0;
};

string previo, preprevio;
vector<company> stocks;
vector<pair<int, char>> setRepresentation;
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

                    if (stocks[i].buy_flag && (stocks[i].waiting_buy_price >= integer_price))
                    {
                        cout<<"No Trade\r"<<endl;
                        break;
                    }
                    else if (stocks[i].buy_flag && (stocks[i].waiting_buy_price < integer_price))
                    {
                        stocks[i].waiting_buy_price = integer_price;
                        stocks[i].buy_flag = 1;
                    }
                    if (stocks[i].sell_flag && stocks[i].waiting_sell_price == integer_price)
                    {
                        stocks[i].buy_flag = 0;
                        stocks[i].sell_flag = 0;
                        cout<<"No Trade\r"<<endl;
                        break;
                    }

                    if (stocks[i].price < integer_price)
                    {
                        cout<<name<<" "<<integer_price<<" "<<'s'<<"\r"<<endl;
                        stocks[i].price = integer_price;
                        stocks[i].buy_flag = 0;
                        break;
                    }
                    else if (stocks[i].price >= integer_price)
                    {
                        stocks[i].waiting_buy_price = integer_price;
                        stocks[i].buy_flag = 1;
                        cout<<"No Trade"<<"\r"<<endl;
                        break;
                    }
                    
                }

                else if (type == 's')
                {
                    if (stocks[i].sell_flag && (stocks[i].waiting_sell_price <= integer_price))
                    {
                        cout<<"No Trade\r"<<endl;
                        break;
                    }
                    else if (stocks[i].sell_flag && (stocks[i].waiting_sell_price > integer_price))
                    {
                        stocks[i].waiting_sell_price = integer_price;
                        stocks[i].sell_flag = 1;
                    }
                    if (stocks[i].buy_flag && stocks[i].waiting_buy_price == integer_price)
                    {
                        stocks[i].buy_flag = 0;
                        stocks[i].sell_flag = 0;
                        cout<<"No Trade\r"<<endl;
                        break;
                    }
                    
                    if (stocks[i].price > integer_price)
                    {
                        cout<<name<<" "<<integer_price<<" "<<'b'<<"\r"<<endl;
                        stocks[i].price = integer_price;
                        stocks[i].sell_flag = 0;
                        break;
                    }
                    else if (stocks[i].price <= integer_price)
                    {   
                        stocks[i].waiting_sell_price = integer_price;
                        stocks[i].sell_flag = 1;
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

int processtwo(string message)
{
    vector<string> orders = extract_orders(message);
    orders[0] = previo + orders[0];
    previo = preprevio;
    int totalprofit=0;
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
            // since new company appeared, matching is also not possible
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
            bool done = 0, ch = 1;
            int buyfound = -1, sellfound = -1;
            // have to check for match first
            for (int i = 0; i < arbitrage.size(); ++i)
            {
                if (arbitrage[i].quantity == quantity)
                {                    
                    if (arbitrage[i].otype == 'b')
                    {
                        buyfound = i;
                    }
                    if (arbitrage[i].otype == 's')
                    {
                        sellfound = i;
                    }
                }
                if (buyfound != -1 && sellfound != -1) break;
            }
            if (type == 'b')
            {
                if (buyfound != -1)
                {
                    if (arbitrage[buyfound].netprice >= nprice)
                    {
                        // incoming order dies
                        cout<<"No Trade\r"<<endl;
                        done = 1;
                        ch = 0;
                    }
                    else
                    {
                        arbitrage.erase(arbitrage.begin() + buyfound);
                        // deleting the old one so that i consider the new one for profit calc
                        ch = 0;
                    }
                }
                if (ch && sellfound != -1)
                {
                    if (arbitrage[sellfound].netprice == nprice)
                    {
                        // new one cancels out old one. both die
                        arbitrage.erase(arbitrage.begin() + sellfound);
                        cout<<"No Trade\r"<<endl;
                        done = 1;                        
                    }
                }
            }
            else if (type == 's')
            {
                if (sellfound != -1)
                {
                    if (arbitrage[sellfound].netprice <= nprice)
                    {
                        // incoming order dies
                        cout<<"No Trade\r"<<endl;
                        done = 1;
                        ch = 0;
                    }
                    else
                    {
                        arbitrage.erase(arbitrage.begin() + sellfound);
                        // deleting the old one so that i consider the new one for profit calc
                        ch = 0;
                    }
                }
                if (ch && buyfound != -1)
                {
                    if (arbitrage[buyfound].netprice == nprice)
                    {
                        // new one cancels out old one. both die
                        arbitrage.erase(arbitrage.begin() + buyfound);
                        cout<<"No Trade\r"<<endl;
                        done = 1;                        
                    }
                }
            }

            if (done) continue;

            vector<vector<int>> res = subsets(arbitrage.size());
            bool sumbool2 =0; 

            for (int i = 0; i < res.size(); i++) 
            {
                vector<int> sum(cname.size(),0);
                bool sumbool = 0;
                int profit = 0;
                for(int k = 0 ; k < cname.size(); k++)
                {
                    for (int j = 0; j < res[i].size(); j++)
                    {   
                        if(arbitrage[res[i][j]].otype=='b')
                        {
                            sum[k] += arbitrage[res[i][j]].quantity[k];
                        }
                        else
                        {
                            sum[k] -= arbitrage[res[i][j]].quantity[k];
                        }
                    }
                    if (type=='b')
                    {
                        sum[k] += quantity[k];
                    }
                    else
                    {
                        sum[k] -= quantity[k];
                    }
                    if(sum[k]!=0)
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
        if(type == 's') type = 'b';
        else type = 's';
        
        cout<<nprice<<" "<<type<<"\r"<<endl;

        for (int k = arbitrage_array[index].indices.size()-1; k >= 0; k--)
        {
            for (int p = 0; p < cname.size(); p++)
            {
                if (arbitrage[arbitrage_array[index].indices[k]].quantity[p] != 0) 
                {
                    cout<<cname[p]<<" "<<arbitrage[arbitrage_array[index].indices[k]].quantity[p]<<" ";
                } 
            }
            if(arbitrage[arbitrage_array[index].indices[k]].otype == 's')
            {
                arbitrage[arbitrage_array[index].indices[k]].otype='b';
            }
            else
            {
                arbitrage[arbitrage_array[index].indices[k]].otype='s';
            }
            cout<<arbitrage[arbitrage_array[index].indices[k]].netprice<<" "<<arbitrage[arbitrage_array[index].indices[k]].otype<<"\r"<<endl;
            arbitrage.erase(arbitrage.begin() + arbitrage_array[index].indices[k]);
        }
        totalprofit += maxprofit;
    }
    }
    return totalprofit;
}

class newarbitorder{
    public:
    vector<int> quantity;
    int netprice;
    char otype;
};

vector<newarbitorder> orderbook;
vector<int>netquantity;
vector<string> dname;

int countDigitsInBinary(int num) {
    if (num <= 0) {
        return 0;
    }
    int count = 0;
    while (num > 0) {
        num = num >> 1;
        count++;
    }
    return count;
}

vector<vector<int>>createsubsets(const vector<int>& netq)
{
    vector<vector<int>>new_possible_arbitrage;
    int totalSubsets = 1;
    int n = netq.size();
    // Calculate the total number of subsets
    int totalSubsets = 1;
    for (int i = 0; i < n; ++i) {
        totalSubsets *= (netq[i] + 1);
    }
    // Generate all subsets using counts of each character
   for (int i = 0; i < totalSubsets; ++i) {
        vector<int> subset;
        int temp = i;
        for (int j = 0; j < n; ++j)
        {
            subset.push_back(temp % (netq[j] + 1));
            temp /= (netq[j] + 1);
        }
        new_possible_arbitrage.push_back(subset);
    }

    return new_possible_arbitrage;
}

int processthree(string message)
{
    vector<string> orders = extract_orders(message);
    orders[0] = previo + orders[0];
    previo = preprevio;
    int totalprofit=0;
    for (auto order : orders)
    {
        string name, netprice, netquantity;
        vector<int> quantity(dname.size(),0);
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

        ite--;

        for(int i = temp.size(); i > 0; i--)
        {
            netprice.push_back(temp[i-1]);
        }

        int nquantity = stoi(netprice);

        temp = ""; netprice = "";

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
            for(int i = 0; i < dname.size(); i++)
            {
                if(dname[i] == tempname)
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
                dname.push_back(tempname);
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
            newarbitorder newa;
            newa.quantity = quantity;
            newa.netprice = nprice;
            netquantity.push_back(nquantity);
            newa.otype = type;
            orderbook.push_back(newa); 
            cout<<"No Trade"<<"\r"<<endl;
            for(int l = 0; l < orderbook.size(); l++)
            {   
                while(orderbook[l].quantity.size() < dname.size())
                {
                    orderbook[l].quantity.push_back(0);
                }
            }
        }
        else
        {  
            bool done = 0, ch = 1;
            int i = 0;
            while(i < orderbook.size())
            {
                if(type == 'b')
                {  
                    if( orderbook[i].netprice == nprice)
                    {
                        if (orderbook[i].quantity == quantity)
                        {     
                            if (type == 'b')
                            {
                                for(int u= 0; u <netquantity[i] ; u++)
                                {
                                    netquantity[i] += nquantity;
                                }
                                for(int u= 0; u<nquantity; u++)
                                {
                                    orderbook.insert(orderbook.begin() + i + netquantity[i] - nquantity + u , orderbook[i]);
                                }
                                done = 1;
                                break;
                            }
                            else
                            {
                                if(nquantity < orderbook[i].netquantity)
                                {
                                    for(int u= 0; u <orderbook[i].netquantity ; u++)
                                    {
                                        orderbook[i+u].netquantity -= nquantity;
                                    }
                                    orderbook.erase(orderbook.begin() + i, orderbook.begin() + i + nquantity);
                                    cout << "No Trade"<<"\r"<<endl;
                                    done = 1;
                                    ch = 0;
                                }
                                else if(nquantity == orderbook[i].netquantity)
                                {
                                    orderbook.erase(orderbook.begin() + i, orderbook.begin() + i + nquantity);
                                    cout << "No Trade"<<"\r"<<endl;
                                    done = 1;
                                    ch = 0;
                                }
                                else
                                {
                                    nquantity = nquantity - orderbook[i].netquantity;
                                    orderbook.erase(orderbook.begin() + i, orderbook.begin() + i + orderbook[i].netquantity);
                                }
                            }
                            break;
                        }               
                    }
                }
                else if(type == 's')
                {  
                    if( orderbook[i].netprice == nprice)
                    {
                        if (orderbook[i].quantity == quantity)
                        {     
                            if (type == 's')
                            {
                                for(int u= 0; u <orderbook[i].netquantity ; u++)
                                {
                                    orderbook[i+u].netquantity += nquantity;
                                }
                                for(int u= 0; u<nquantity; u++)
                                {
                                    orderbook.insert(orderbook.begin() + i + orderbook[i].netquantity - nquantity + u , orderbook[i]);
                                }
                                done = 1;
                                break;
                            }
                            else
                            {
                                if(nquantity < orderbook[i].netquantity)
                                {
                                    for(int u= 0; u <orderbook[i].netquantity ; u++)
                                    {
                                        orderbook[i+u].netquantity -= nquantity;
                                    }
                                    orderbook.erase(orderbook.begin() + i, orderbook.begin() + i + nquantity);
                                    cout << "No Trade"<<"\r"<<endl;
                                    done = 1;
                                    ch = 0;
                                }
                                else if(nquantity == orderbook[i].netquantity)
                                {
                                    orderbook.erase(orderbook.begin() + i, orderbook.begin() + i + nquantity);
                                    cout << "No Trade"<<"\r"<<endl;
                                    done = 1;
                                    ch = 0;
                                }
                                else
                                {
                                    nquantity = nquantity - orderbook[i].netquantity;
                                    orderbook.erase(orderbook.begin() + i, orderbook.begin() + i + orderbook[i].netquantity);
                                }
                            }
                        break;
                        }               
                    }
                }
                i += orderbook[i].netquantity;
            }
            if (!done)
            {
                newarbitorder newao;
                newao.otype = type;
                newao.netprice = nprice;
                newao.netquantity = nquantity;
                newao.quantity = quantity;
                for (int z=0; z<nquantity; z++)
                {
                    orderbook.push_back(newao);
                }
            }
            if (!ch) continue;

            vector<vector<int>> res = subsets(orderbook.size());
            bool sumbool2 =0; 

            for (int i = 0; i < res.size(); i++) 
            {
                vector<int> sum(dname.size(),0);
                bool sumbool = 0;
                int profit = 0;
                for(int k = 0 ; k < dname.size(); k++)
                {
                    for (int j = 0; j < res[i].size(); j++)
                    {   
                        if(orderbook[res[i][j]].otype=='b'){
                            sum[k] += orderbook[res[i][j]].quantity[k];
                        }
                        else{
                            sum[k] -= orderbook[res[i][j]].quantity[k];
                        }
                    }
                    if(sum[k]!=0)
                    {
                        sumbool = 1;
                        break;
                    }
                }
                if (sumbool) continue;
                for (int j = 0; j < res[i].size(); j++)
                {
                    if (orderbook[res[i][j]].otype == 'b') profit += orderbook[res[i][j]].netprice;
                    else if (orderbook[res[i][j]].otype == 's') profit -= orderbook[res[i][j]].netprice;
                } 
                if (profit > 0) 
                {  
                    sumbool2 = 1; 
                    new_possible_arbitage newpa;
                    for(int y=0; y<res[i].size(); y++)
                    {
                        if(y!=0 && orderbook[res[i][y]].quantity==orderbook[res[i][y-1]].quantity && orderbook[res[i][y]].netprice==orderbook[res[i][y-1]].netprice)
                        {
                            newpa.nq[newpa.nq.size()-1]+=1;
                        }
                        else
                        {
                            newpa.indices.push_back(res[i][y]);
                            newpa.nq.push_back(1);
                        }
                    }
                    newpa.price = profit;
                    new_arbitrage_array.push_back(newpa);
                }
            }
            if (sumbool2 == 0)
            {
                cout<<"No Trade"<<"\r"<<endl;
                continue;
            }

        int maxprofit = 0;
        int index;

        if (new_arbitrage_array.size()==0) continue;

        for (int l = 0; l < new_arbitrage_array.size(); l++)
        {
            if(new_arbitrage_array[l].price > maxprofit)
            {
                maxprofit = new_arbitrage_array[l].price;
                index = l;
            }
        }
        for (int k = new_arbitrage_array[index].indices.size()-1; k >= 0; k--)
        {
            for (int p = 0; p < dname.size(); p++)
            {   
                if (orderbook[new_arbitrage_array[index].indices[k]].quantity[p] != 0) 
                {
                    cout<<dname[p]<<" "<<orderbook[new_arbitrage_array[index].indices[k]].quantity[p]<<" ";
                } 
            }
            if(orderbook[new_arbitrage_array[index].indices[k]].otype == 's')
            {
                orderbook[new_arbitrage_array[index].indices[k]].otype='b';
            }
            else
            {
                orderbook[new_arbitrage_array[index].indices[k]].otype='s';
            }
            cout<<orderbook[new_arbitrage_array[index].indices[k]].netprice<<" "<<new_arbitrage_array[index].nq[k]<<" "<<orderbook[new_arbitrage_array[index].indices[k]].otype<<"\r"<<endl;
            orderbook[new_arbitrage_array[index].indices[k]].netquantity -= new_arbitrage_array[index].nq[k];
            // cout<<orderbook[new_arbitrage_array[index].indices[k]].netquantity<<endl;
            // cout<<new_arbitrage_array[index].indices[k]<<endl;
            orderbook.erase(orderbook.begin() + new_arbitrage_array[index].indices[k], orderbook.begin() + new_arbitrage_array[index].indices[k] + new_arbitrage_array[index].nq[k]);
            // cout<<orderbook.size()<<endl;
            for(int u=0; u< orderbook[new_arbitrage_array[index].indices[k]].netquantity; u++)
            {
                orderbook[new_arbitrage_array[index].indices[k]+u].netquantity = orderbook[new_arbitrage_array[index].indices[k]].netquantity;
            }
        }
        totalprofit += maxprofit;
    }
    }
    return totalprofit;
}

int main(int argc, char* argv[]) 
{
    Receiver rcv;
    // sleep(5);

    int choice = stoi(argv[1]);
    int tprofit = 0;
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
            while(true)
            {
                string message = rcv.readIML();
                auto endmarker = message.end();
                --endmarker;
                tprofit += processtwo(message);
                if (*endmarker == '$') break;
            }
            cout<<tprofit<<"\r"<<endl;
            break;

        case 3:
            while(true)
            {
                string message = rcv.readIML();
                auto endmarker = message.end();
                --endmarker;
                tprofit += processtwo(message);
                if (*endmarker == '$') break;
            }
            cout<<tprofit<<"\r"<<endl;
            break;
    }
    
    return 0;
}