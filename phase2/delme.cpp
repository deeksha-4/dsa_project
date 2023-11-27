#include <bits/stdc++.h>
using namespace std;
int main()
{
    string line = "6 KarGoTravels SELL AMZN $50 #22 -1";
    string s;
    stringstream ss(line);
 
    vector<string> v;
    while (getline(ss, s, ' ')) {
        v.push_back(s);
    }
    
    

    int start_time = stoi(v[0]);
    int end_time;
    if (*(--v.end()) == "-1") end_time = -1;
    else end_time = stoi(v[6]) + start_time;
    string name = v[1];
    int n = v.size();
    v[n-2].erase(v[n-2].begin());
    int quantity = stoi(v[n-2]);
    v[n-3].erase(v[n-3].begin());
    int price = stoi(v[n-3]);
    char type = v[2][0];
    
}