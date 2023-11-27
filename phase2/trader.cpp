// This is your trader. Place your orders from here

#include <fstream> 
#include <string>
#include <iostream>
#include <atomic>
#include <vector>
using namespace std;

extern std::atomic<int> commonTimer;
extern std::mutex printMutex; 

int n = 0;
// no of lines already read, so that i consider only new orders

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

        for (auto u: v) cerr<<u<<endl;
        // now v has the set of all orders we have seen till now
        // relevant part is only from [n:]

        // process v

        n = v.size();

        int currentTime;
        {
            currentTime = commonTimer.load();
        }
        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << currentTime << " " << "Sharvanee SELL GE $1 #50 6" << std::endl;
        cerr<<"----"<<endl;
    }
    return 1;
}

int trader(std::string *message)
{
    return 1;
}
