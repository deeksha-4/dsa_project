#include <iostream>
#include <vector>
#include <string>
using namespace std;

class ran{
    public:
    int data;
    string name;
};

int main()
{
    ran r;
    r.data = 4;
    r.name = "d";
    ran w;
    w.data = 3;
    w.name = "e";
    vector<ran> v;
    v.push_back(r);
    v.push_back(w);

    for (int i =0; i<v.size(); ++i)
    {
        if (v[i].name == "e")
        {
            v[i].data = 8;
        }
    }

    for (auto u: v)
    {
        cout<<u.data<< " "<<u.name<<endl;
    }
}