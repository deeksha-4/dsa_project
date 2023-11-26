#include "market.h"
#include <iostream>
#include <fstream>
using namespace std;

void print(string buyer, string seller, int qtty, vector<Stock> stonk, int p)
{
    if (!qtty) return;
    cout<<buyer<<" purchased "<<qtty<<" share of ";
    if (stonk.size() == 1 && stonk[0].quantity == 1) cout<<stonk[0].name<<" ";
    else for (auto u: stonk) cout<<u.name<<" "<<u.quantity<<" ";
    cout<< "from "<< seller<<" for $"<<p<<"/share\r"<<endl;
}

market::market(int argc, char** argv)
{
    vector<Order> orderBook;
}

void market::start()
{
    int moni=0, num_trades=0, num_shares=0;
    vector<Company> companies;
    ifstream inf;
    inf.open("output.txt");
    string line;
    while(getline(inf, line))
    {
        if (*(--line.end()) == '\r') line.pop_back();
        if (line == "!@") break;
        if (line == "TL") continue;
        Order new_order;
        string temp, temp1, temp2, temp3, temp4;
        auto it = line.begin();
        while(*it!=' ')
        {
            temp.push_back(*it);
            it++;
        }
        it++;
        new_order.start = stoi(temp);
        auto jt = line.end();
        jt--;
        while(*jt!=' ')
        {
            temp1.push_back(*jt);
            jt--;
        }
        jt--;
        if (temp1 == "1-") new_order.end = -1;
        else
        {
            for (int k = temp1.size()-1; k>=0; k--)
            {
                temp2.push_back(temp1[k]);
            }
            
            new_order.end = new_order.start + stoi(temp2);
        }
        while(*jt!='#')
        {
            temp3.push_back(*jt);
            jt--;
        }
        jt--; jt--;
        for (int k = temp3.size()-1; k>=0; k--)
        {
            temp4.push_back(temp3[k]);
        }
        new_order.quantity = stoi(temp4);
        string temp5, temp6;
        while(*jt!='$')
        {
            temp5.push_back(*jt);
            jt--;
        }
        jt--; // jt is at last whitespace
        for (int k = temp5.size()-1; k>=0; k--)
        {
            temp6.push_back(temp5[k]);
        }
        new_order.price = stoi(temp6);
        string temp7, temp8;
        while(*it!=' ')
        {
            temp7.push_back(*it);
            it++;
        }
        it++;
        new_order.name = temp7;
        string tempo;

        while(*it!=' ')
        {
            tempo.push_back(*it);
            it++;
        }
        it++;
        new_order.type = tempo[0];

        while(*it!=' ')
        {
            temp8.push_back(*it);
            it++;
        }
        if (it==jt) // no lincomb
        {
            Stock s;
            s.name = temp8;
            s.quantity = 1;
            new_order.items.push_back(s);
        }

        else
        {
            it++;
            // first take corresponding qtty
            string temp9;
            while(*it!=' ')
            {
                temp9.push_back(*it);
                it++;
            }
            
            Stock s;
            s.name = temp8;
            s.quantity = stoi(temp9);
            new_order.items.push_back(s);

            while(it!=jt)
            {
                it++;
                string temp10, temp11;
                while(*it!=' ')
                {
                    temp10.push_back(*it);
                    it++;
                }
                it++;
                while(*it!=' ')
                {
                    temp11.push_back(*it);
                    it++;
                }
                Stock s;
                s.name = temp10;
                s.quantity = stoi(temp11);
                new_order.items.push_back(s);
            }
        }

        bool f = 0;

        for (int l = 0; l<companies.size(); ++l)        
        {
            if (companies[l].name == new_order.name)
            {
                f = 1;
                break;
            }
        }

        if (!f)
        {
            Company c;
            c.buy = 0;
            c.sell = 0;
            c.net = 0;
            c.name = new_order.name;
            companies.push_back(c);
        }

        // first check if any trade is possible, could avoid an unnecessary push

        vector<candidate> possible_matches;

        for (int i = 0; i<orderBook.size(); ++i)
        {
            if (orderBook[i].end >= 0 && orderBook[i].end <= new_order.start)
            {
                // order already ded
                continue;
            }
            if (orderBook[i].type != new_order.type)
            {
                // matching is possible
                if (new_order.items == orderBook[i].items)
                // gotta do this custom 
                {
                    if (tempo[0] == 'B')
                    {
                        // incoming order is buy, match with sell
                        if (orderBook[i].price <= new_order.price)
                        {
                            candidate c;
                            c.index = i;
                            c.order = orderBook[i];
                            possible_matches.push_back(c);
                            // possible_matches.push_back(orderBook[i]);
                            // index.push_back(i);
                        }
                    }
                    else
                    {
                        // incoming order is sell, match with buy
                        if (orderBook[i].price >= new_order.price)
                        {
                            candidate c;
                            c.index = i;
                            c.order = orderBook[i];
                            possible_matches.push_back(c);
                            // possible_matches.push_back(orderBook[i]);
                            // index.push_back(i);
                        }
                    }
                }
            }
        }

        // now possible_matches has all the candidates
        if (tempo[0] =='B') sort(possible_matches.begin(), possible_matches.end(), buy_key());
        else if (tempo[0] =='S') sort(possible_matches.begin(), possible_matches.end(), sell_key());

        bool flag = 0;
        vector<int> indices_to_delete;

        for (int i = 0; i<possible_matches.size(); ++i)
        {
            if (flag) break;
            string buyer, seller;
            if (tempo[0] == 'B')
            {
                buyer = new_order.name;
                seller = possible_matches[i].order.name;
            }
            else
            {
                seller = new_order.name;
                buyer = possible_matches[i].order.name;
            }

            if (possible_matches[i].order.quantity >= new_order.quantity)
            {
                // match some and delete
                flag = 1;
                // cout<<possible_matches[i].quantity<<" ";
                // cout<<orderBook[index[i]].quantity<<" ";
                orderBook[possible_matches[i].index].quantity -= new_order.quantity;
                // cout<<orderBook[index[i]].quantity<<endl;
                // cout<<possible_matches[i].quantity<<endl;
                moni += possible_matches[i].order.price*new_order.quantity;
                num_shares += new_order.quantity;
                if (possible_matches[i].order.quantity) num_trades++;
                print(buyer, seller, new_order.quantity, new_order.items, possible_matches[i].order.price);

                if (buyer == seller)
                {

                    for (int j = 0; j< companies.size(); ++j)
                    {
                        if (companies[j].name == buyer)
                        {
                         
                            companies[j].buy += new_order.quantity;
                            companies[j].sell += new_order.quantity;
                            break;
                        }
                    }

                }

                else
                {
                    bool bf = 0, sf = 0;
                    for (int j = 0; j< companies.size(); ++j)
                    {
                        if (bf*sf) break;
                        if (companies[j].name == buyer)
                        {
                            bf = 1;
                            companies[j].buy += new_order.quantity;
                            companies[j].net -= new_order.quantity*possible_matches[i].order.price;
                        }
                        if (companies[j].name == seller)
                        {
                            sf = 1;
                            companies[j].sell += new_order.quantity;
                            companies[j].net += new_order.quantity*possible_matches[i].order.price;
                        }
                    }
                }

                new_order.quantity = 0;

            }
            else
            {
                // delete the old order completely
                // indices_to_delete.push_back(index[i]);
                // cout<<new_order.quantity<<" ";
                new_order.quantity -= possible_matches[i].order.quantity;
                if (new_order.quantity == 0) flag = 1;
                orderBook[possible_matches[i].index].quantity = 0;
                // cout<<new_order.quantity<<endl;
                num_shares += possible_matches[i].order.quantity;
                if (possible_matches[i].order.quantity) num_trades++;
                moni += possible_matches[i].order.price*possible_matches[i].order.quantity;
                print(buyer, seller, possible_matches[i].order.quantity, new_order.items, possible_matches[i].order.price);
                if (buyer == seller)
                {
                    
                    for (int j = 0; j< companies.size(); ++j)
                    {
                        if (companies[j].name == buyer)
                        {
                            
                            companies[j].buy += possible_matches[i].order.quantity;
                            companies[j].sell += possible_matches[i].order.quantity;
                            break;
                        }
                    }

                }

                else
                {
                    bool bf = 0, sf = 0;
                    for (int j = 0; j< companies.size(); ++j)
                    {
                        if (bf*sf) break;
                        if (companies[j].name == buyer)
                        {
                            bf = 1;
                            companies[j].buy += possible_matches[i].order.quantity;
                            companies[j].net -= possible_matches[i].order.quantity*possible_matches[i].order.price;
                        }
                        if (companies[j].name == seller)
                        {
                            sf = 1;
                            companies[j].sell += possible_matches[i].order.quantity;
                            companies[j].net += possible_matches[i].order.quantity*possible_matches[i].order.price;
                        }
                    }
                }
            }
        }

        if (new_order.quantity) orderBook.push_back(new_order);
        // for (int u = indices_to_delete.size()-1; u>=0; --u)
        // {
        //     orderBook.erase(orderBook.begin() + indices_to_delete[u]);            
        // }
    }

    cout<<'\r'<<endl;
    cout<<"---End of Day---\r"<<endl<<"Total Amount of Money Transferred: $"<<moni<<'\r'<<endl<<"Number of Completed Trades: "<<num_trades<<'\r'<<endl<<"Number of Shares Traded: "<<num_shares<<'\r'<<endl;
    for (auto u: companies)
    {
        cout<<u.name<<" bought "<<u.buy<<" and sold "<<u.sell<<" for a net transfer of $"<<u.net<<'\r'<<endl;
    }
}