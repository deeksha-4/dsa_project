// Listening to a given port no 8888 and printing the incoming messages
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>
#include "trader.h"
#include <sstream>

using namespace std;

bool directoryExists(const std::string &directory) {
    struct stat info;
    return stat(directory.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

void createDirectory(const std::string &directory) {
    #ifdef _WIN32
    mkdir(directory.c_str());
    #else
    mkdir(directory.c_str(), 0777);
    #endif
}

void writei(char type, string stock, int price, int qtty, int exp, int i)
{
    string directory = "outputs";
    string sale;
    if (type == 'B') sale = "BUY";
    else sale = "SELL";
    if (!directoryExists(directory)) {
        createDirectory(directory);
    }
    int currentTime;
    {
        currentTime = commonTimer.load();
    }
    string name = "outputs/output" + to_string(i) + ".txt";
    fstream outputFile(name, ios::app);
    {
        std::lock_guard<std::mutex> lock(printMutex);
        outputFile<<currentTime<<" "<<"22B0943_22B0988 "<<sale<<" "<<stock<<"$"<<price<<" #"<<qtty<<" ";
        if (exp!=-1) outputFile<<currentTime+exp<<'\r'<<endl;
        else outputFile<<-1<<'\r'<<endl;
    }
    outputFile.close();
}

void writeo(string s, int i)
{
    string directory = "outputs";
    
    if (!directoryExists(directory)) {
        createDirectory(directory);
    }
    
    string name = "outputs/output" + to_string(i) + ".txt";
    fstream outputFile(name, ios::app);
    {
        std::lock_guard<std::mutex> lock(printMutex);
        outputFile<<s<<'\r'<<endl;
    }
    outputFile.close();
}

Order to_order(string line)
{
    Order new_order;
    string s;
    stringstream ss(line);

    vector<string> v;
    while (getline(ss, s, ' ')) {
        v.push_back(s);
    }
    //1 KarGoKrab BUY AMZN $130 #12 0

    
    new_order.start = stoi(v[0]);
    if (*(--v.end()) == "-1") new_order.end = -1;
    else new_order.end = stoi(*(--v.end())) + new_order.start;
    new_order.name = v[1];    
    v[4].erase(v[4].begin());
    new_order.quantity = stoi(v[4]);
    v[5].erase(v[5].begin());
    new_order.price = stoi(v[5]);
    new_order.type = v[2][0];
    new_order.stock = v[3];
    return new_order;
    
    
}

const int BUFFER_SIZE = 1024;
int NUM_THREADS = 2;
int pp = 0;
int counto = 0;
int countp = 0;


mutex printMutex;
atomic<int> commonTimer;

vector <client> clients;
int id = 1;

// Structure to store client socket and its details
struct ClientInfo {
    int socket;
    struct sockaddr_in address;
    ClientInfo(int socket, struct sockaddr_in& address) : socket(socket), address(address) {}
    ClientInfo() {};
};

// Function to handle a client connection
void *handleClient(void *arg) 
{
    ClientInfo *clientInfo = static_cast<ClientInfo *>(arg);
    char buffer[BUFFER_SIZE];

    
    // {
    //     std::lock_guard<std::mutex> lock(printMutex);
    //     std::cout << "Connected to client, IP: " << inet_ntoa(clientInfo->address.sin_addr) << ", Port: " << ntohs(clientInfo->address.sin_port) << std::endl;
    // }
    client* c = new client;
    c->id = id;
    id++;
    c->name = to_string(clientInfo->address.sin_port);
    // {
    //     std::lock_guard<std::mutex> lock(printMutex);
    //     cout<<"name: "<<c.name<<" id: "<<c.id<<endl;
    // }
    
    clients.push_back(*c);
    // for (auto u: clients) cerr<<u.name<<" "<<u.id<<endl; 

    while (true) {
        // Receive data from the client
        ssize_t bytesRead = recv(clientInfo->socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
        //     // Error or connection closed
        //     if (bytesRead == 0) {
        //         // std::cout << "Connection closed by client, IP: " << inet_ntoa(clientInfo->address.sin_addr) << ", Port: " << ntohs(clientInfo->address.sin_port) << std::endl;
        //     } else {
        //         perror("Recv error");
        //     }
            cout<<pp;
            break;
        } else {
            cerr<<1<<endl;
            // Process the received message
            buffer[bytesRead] = '\0';
            Order n;
            // {
            //     std::lock_guard<std::mutex> lock(printMutex);
            //     std::cout<<buffer << std::endl;
            // }  
            cerr<<"START"<<buffer<<"END";
            string uffer = buffer;
            if (*(--uffer.end())=='\r') uffer.pop_back();
            if (uffer == "$")
            {
                cerr<<"hi"<<endl;
                counto++;
                if (counto == NUM_THREADS) break;
            }
            string cname = to_string(clientInfo->address.sin_port);
            Order new_order = to_order(uffer);
            
            cerr<<2<<endl;
            bool cancelled = 0;
            int index;
            for (int i = 0; i<clients.size(); ++i) 
            {
                cerr<<3<<endl;
                if (clients[i].name == cname)
                {
                    writeo(uffer, clients[i].id);
                    index = clients[i].id;
                    vector<candidate> possible_matches;
                    
                    // check within this market to see if the order gets cancelled
                    for (int j = 0; j<clients[i].orderBook.size(); ++j)
                    {                        
                        if (clients[i].orderBook[j].end!=-1 && new_order.start > clients[i].orderBook[j].end) continue;
                        if (clients[i].orderBook[j].stock == new_order.stock)
                        {
                            if(new_order.type == 'S')
                            {
                                
                                if (clients[i].orderBook[j].type == 'S') continue;
                                // i will cancel if old buy price is more than new sell price
                                if (clients[i].orderBook[j].price < new_order.price) continue;
                                // cancelled = 1;
                                // clients[i].orderBook.erase(clients[i].orderBook.begin()+j);
                                candidate c;
                                c.index = j;
                                c.order = clients[i].orderBook[j];
                                possible_matches.push_back(c);
                            }
                            else
                            {
                                if (clients[i].orderBook[j].type == 'B') continue;
                                // i will cancel if old buy price is more than new sell price
                                if (clients[i].orderBook[j].price > new_order.price) continue;
                                // cancelled = 1;
                                // clients[i].orderBook.erase(clients[i].orderBook.begin()+j);
                                candidate c;
                                c.index = j;
                                c.order = clients[i].orderBook[j];
                                possible_matches.push_back(c);
                            }
                        }
                    }
                    
                    if (new_order.type =='B') sort(possible_matches.begin(), possible_matches.end(), buy_key());
                    else if (new_order.type =='S') sort(possible_matches.begin(), possible_matches.end(), sell_key());

                    cerr<<4<<endl;

                    bool flag = 0;

                    for (int k = 0; k<possible_matches.size(); ++k)
                    {
                        if (flag) break;

                        if (possible_matches[k].order.quantity >= new_order.quantity)
                        {
                            // match some and delete
                            flag = 1;                            
                            clients[i].orderBook[possible_matches[k].index].quantity -= new_order.quantity;
                            cancelled = 1;
                        }
                        else
                        {
                            // delete the old order completely                            
                            new_order.quantity -= possible_matches[k].order.quantity;
                            if (new_order.quantity == 0) 
                            {
                                flag = 1;
                                cancelled = 1;
                            }
                            clients[i].orderBook.erase(clients[i].orderBook.begin()+possible_matches[k].index);                                  
                        }
                    }
                    // order cancelled maybe
                    break;
                }
                
            }

            cerr<<5<<endl;
            
            // if the order doesnt get cancelled, scour all the other markets for arbit chances
            if (cancelled) continue;
            for (int i = 0; i<clients.size(); ++i)
            {
                
                if (new_order.name == clients[i].name) continue;
                // now look for arbit chances
                if (new_order.type =='B')
                {
                    for (int j = 0; j< clients[i].orderBook.size(); ++i)
                    {
                        cerr<<"^^"<<clients[i].orderBook.size()<<endl;
                        if (clients[i].orderBook[j].type == 'S')                    
                        {
                            cerr<<9<<endl;
                            // arbit mayb possible
                            if (clients[i].orderBook[j].price < new_order.price)
                            {
                                cerr<<8<<endl;
                                writei('B', new_order.stock, clients[i].orderBook[j].price, min(clients[i].orderBook[j].quantity, new_order.quantity), clients[i].orderBook[j].end, clients[i].id);
                                cerr<<7<<endl;
                                writei('S', new_order.stock, new_order.price, min(clients[i].orderBook[j].quantity, new_order.quantity), clients[i].orderBook[j].end, index);
                                pp+=(new_order.price-clients[i].orderBook[j].price)*min(clients[i].orderBook[j].quantity, new_order.quantity);
                                clients[i].orderBook[j].quantity -= min(clients[i].orderBook[j].quantity, new_order.quantity);
                                new_order.quantity -= min(clients[i].orderBook[j].quantity, new_order.quantity);
                                if (new_order.quantity == 0) 
                                {
                                    cancelled = 1;
                                    break;
                                }
                            }
                        }

                    }
                }
                
                else
                {
                    for (int j = 0; j< clients[i].orderBook.size(); ++i)
                    {
                        cerr<<"))"<<clients.size()<<endl;
                        cerr<<"^^"<<clients[1].orderBook.size()<<endl;
                        cerr<<clients[i].orderBook[j].type<<endl;
                        if (clients[i].orderBook[j].type == 'B')                    
                        {
                            // arbit mayb possible
                            cerr<<9<<endl;
                            if (clients[i].orderBook[j].price > new_order.price)
                            {
                                cerr<<8<<endl;
                                writei('S', new_order.stock, clients[i].orderBook[j].price, min(clients[i].orderBook[j].quantity, new_order.quantity), clients[i].orderBook[j].end, clients[i].id);
                                cerr<<7<<endl;
                                writei('B', new_order.stock, new_order.price, min(clients[i].orderBook[j].quantity, new_order.quantity), clients[i].orderBook[j].end, index);
                                pp-=(new_order.price-clients[i].orderBook[j].price)*min(clients[i].orderBook[j].quantity, new_order.quantity);
                                clients[i].orderBook[j].quantity -= min(clients[i].orderBook[j].quantity, new_order.quantity);
                                new_order.quantity -= min(clients[i].orderBook[j].quantity, new_order.quantity);
                                if (new_order.quantity == 0) 
                                {
                                    cancelled = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            cerr<<6<<endl;

            if (!cancelled) clients[index].orderBook.push_back(new_order);
        }
    }
    
    // Close the client socket
    close(clientInfo->socket);
    delete clientInfo;
    pthread_exit(NULL);
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);  // Port number
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind server socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {  // Maximum 5 pending connections
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    // std::cout << "Trader is listening on port 8888..." << std::endl;

    std::vector<pthread_t> clientThreads;

    for(int i = 0; i < NUM_THREADS; i++) {
        // Accept incoming connections
        int clientSocket;
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) == -1) {
            perror("Accept error");
            continue;  // Continue listening for other connections
        }

        // Create a thread to handle this client
        ClientInfo *clientInfo = new ClientInfo(clientSocket, clientAddr);
        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, handleClient, clientInfo) != 0) {
            perror("Thread creation error");
            delete clientInfo;
            continue;  // Continue listening for other connections
        }

        // Store the thread ID for later joining
        clientThreads.push_back(clientThread);
    }

    // Join all client threads (clean up)
    for (auto &thread : clientThreads) {
        pthread_join(thread, NULL);
    }

    // Close the server socket (never reached in this example)
    close(serverSocket);

    return 0;
}