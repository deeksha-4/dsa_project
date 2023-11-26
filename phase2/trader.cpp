#include <iostream>
#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <fstream>

extern std::atomic<int> commonTimer;
extern std::mutex printMutex;  // Declare the mutex for printing

class pqueue{
public:
    std::vector<std::string> elements;
    size_t front;  
    size_t rear;   
    size_t capacity = 5;

    pqueue(size_t size) : elements(size), front(0), rear(0), capacity(size) {}


    void enqueue(std::string value) {
        if ((rear + 1) % capacity == front) {
            dequeue();
        }
        elements[rear] = value;
        rear = (rear + 1) % capacity;
    }

    void dequeue() {
        if (front == rear) {
            return;
        }
        front = (front + 1) % capacity;
    }

};


int reader(int time)
{

    //std::cout<<"bruh\r\n";
    int t = commonTimer.load();
    //std::cout << t << " Harry SELL AMD $1 #32 3" << std::endl;
    std::vector<std::vector<int>> bruh;
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));

    pqueue lit(5);
    std::vector<std::string> lines;
    std::ifstream inputFile ("output.txt");
    std::string line;
    while (std::getline(inputFile,line)) {
        if (line.compare("TL") == 0) {
            continue;
        }
        int inTime = line[0] - '0';
        int outTime = line[line.size() - 2] - '0';
        std::cout<<"inTime is "<<inTime<<" outTime is "<<outTime<<"\r\n";
        if (inTime + outTime > 5 || outTime == -1) {
            lit.enqueue(line);
            //std::cout<<"Printing line"<<line;
        }
        //std::cout<<"bruh\n";
        //std::cout<<line;
        if (line.compare("!@") == 0 || line.compare("Successfully Initiated!") == 0) {
            break;
        }
        lines.push_back(line);
    }

    {
        t = commonTimer.load();
        std::lock_guard<std::mutex> lock(printMutex);
        for (int i=0; i<lit.elements.size(); i++) {
            
            
            
        }
    }
    return 1;
}

int trader(std::string *message)
{
    return 1;
}

// void* userThread(void* arg)
// {
//     int thread_id = *(int*)arg;
//     while (true) {
//         int currentTime;
//         {
//             currentTime = commonTimer.load();
//         }
//         std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
//         int end = reader(currentTime);
//         if (end) break;
//     }
//     return nullptr;
// }

// void* userTrader(void* arg)
// {
//     return nullptr;
// }