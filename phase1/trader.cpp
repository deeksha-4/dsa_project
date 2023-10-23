#include "receiver.h"
#include <vector>
using namespace std;

string previo, preprevio;

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
            start = end + 1;
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
    // process the message
    // for now just printing
    vector<string> orders = extract_orders(message);
    orders[0] = previo + orders[0];
    previo = preprevio;
    for (auto u: orders)
    {
        cout<<u<<endl;
    }
}

int main() {

    Receiver rcv;
    sleep(5);

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

// int main()
// {
//     Receiver rcv;
//     sleep(5);
//     string message = rcv.readIML();
//     auto it = message.end();
//     --it;
//     --it;
//     --it;
//     cout<<*it;
//     // cout<<message;
// }
