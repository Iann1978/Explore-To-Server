//
//  Connection.cpp
//  SingleThreadServer
//
//  Created by iann on 2019/4/29.
//  Copyright © 2019 iann. All rights reserved.
//

#include "Server.hpp"


Connection::Connection(int sock)
{
    this->sock = sock;
}

Connection::~Connection()
{
    this->sock = 0;
}

int Connection::ProcessRecieve()
{
    char buffer[4096];
    int buffer_len = 4096;
    int recvbyte = recv(sock, buffer, buffer_len, 0);
    if (recvbyte > 0)
    {
        for (int i=0; i<recvbyte; i++)
        {
            char c = buffer[i];
            if (c == '\n')
            {
                std::string str = recvstring.str();
                recvstring.clear();
                recvQueue.push(str);
                std::cout << "\ncmd:" << str << "\n";
            }
            else
            {
                recvstring.put(c);
            }
        }
        
        buffer[recvbyte] = 0;
        std::cout << buffer;
    }
    return 0;
}

int Server::CreateSocket()
{
    int error = 0;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddrin;
    saddrin.sin_len = sizeof(struct sockaddr_in);
    saddrin.sin_family = PF_INET;
    saddrin.sin_addr.s_addr = inet_addr("192.168.31.48");
    saddrin.sin_port = htons(5679);
    if (error = bind(sock, (struct sockaddr*)&saddrin, sizeof(struct sockaddr_in)))
    {
        std::cout << "Error!\n";
        return 1;
    }
    if (error = listen(sock, 60))
    {
        std::cout << "Error!\n";
        return 2;
    }
    if (error = fcntl(sock, F_SETFL, O_NONBLOCK))
    {
        std::cout << "Error!\n";
        return 3;
    }
    
    return error;
}



int Server::ProcessAccept()
{
    SOCKET accSock;
    SOCKADDR_IN accAddr;
    unsigned int iaddrSize = sizeof(SOCKADDR_IN);
    accSock = accept(sock, (SOCKADDR*)&accAddr, &iaddrSize);
    if (accSock != -1)
    {
        std::cout << "New Connection Enter!\n";
        connections.insert(connections.begin(), new Connection(accSock));
    }
    return 0;
}

int Server::ProcessReceive()
{
    for(std::list<Connection *>::iterator it = connections.begin();
        it != connections.end(); it++)
    {
        Connection *conn = *it;
        conn->ProcessRecieve();
    }
    return 0;
}

int Server::Run()
{
    while (true) {
        ProcessAccept();
        ProcessReceive();
        
        //sleep(10);
    }
   
    
    
    return 0;
}
