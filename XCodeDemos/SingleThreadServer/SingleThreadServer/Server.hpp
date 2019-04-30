//
//  Connection.hpp
//  SingleThreadServer
//
//  Created by iann on 2019/4/29.
//  Copyright © 2019 iann. All rights reserved.
//

#ifndef Connection_hpp
#define Connection_hpp
#include <iostream>
#include <stdio.h>

#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include "fcntl.h"
#include <list>
#include <queue>
#include <string>
#include <sstream>
#include <vector>


using std::string;
using std::queue;
using std::vector;
using std::list;
using std::stringstream;

//using namespace std;

typedef int SOCKET;
typedef fd_set FD_SET;
typedef unsigned long DWORD;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;


class Connection {
    enum Status
    {
        Connectted,
        Breaked,
        Closed,
    };
    Status status;
public:
    Connection(int sock);
    ~Connection();
    int Update();
    int ProcessRecieve();
    int ProcessPacket();
    int ProcessSend();
    int Close();
private:
    string username;
    int sock;
    std::stringstream recvstring;
    std::queue<std::string> recvQueue;
    std::queue<std::string> sendQueue;
};

class Server {
public:
    int CreateSocket();
    int Run();
    
    int ProcessAccept();
    int ProcessReceive();
    int ProcessPacket();
    int ProcessSend();
private:
    int sock;
    std::list<Connection *> connections;
    
};


#endif /* Connection_hpp */
