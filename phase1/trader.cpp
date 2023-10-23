#include "receiver.h"
#include <vector>

int main() {

    Receiver rcv;
    sleep(5);
    std::string message = rcv.readIML();
    // std::cout << message;

    // splitting message into orders
    std::vector<std::string> orders;
    
    int start = 0;
    while(start < message.length())
    {
        int end = message.find('#', start);
        if (end == std::string::npos)
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
    // now orders has the list of orders

    return 0;
}
