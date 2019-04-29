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
                recvstring.str("");
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

int ParsingCmd(std::string packet, std::string &to, std::string &cmd, std::string &param)
{
    std::string s = "scott>=tiger>=mushroom";
    std::string delimiter = ">=";
    
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        std::cout << token << std::endl;
        s.erase(0, pos + delimiter.length());
    }
    std::cout << s << std::endl;
    return 0;
}

int Connection::ProcessPacket()
{
    while (recvQueue.size()) {
        std::string packet = recvQueue.front();
        recvQueue.pop();
        std::string to, cmd, param;
        ParsingCmd(packet, to, cmd, param);
        
        if (to.compare("me") || to.compare("self"))
        {
            if (cmd.compare("setname"))
            {
                username = param;
            }
            else if (cmd.compare("whoami"))
            {
                
            }
            else if (cmd.compare("whoonline"))
            {
                
            }
        }
        
        
    }
    return 0;
    
}

int Server::CreateSocket()
{
    int port = 5678;
    const char *ip = "192.168.31.48";
    int error = 0;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddrin;
    saddrin.sin_len = sizeof(struct sockaddr_in);
    saddrin.sin_family = PF_INET;
    saddrin.sin_addr.s_addr = inet_addr(ip);
    saddrin.sin_port = htons(port);
    std::cout << ip << ":" << port << "\n";
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


int Server::ProcessPacket()
{
    for(std::list<Connection *>::iterator it = connections.begin();
        it != connections.end(); it++)
    {
        Connection *conn = *it;
        conn->ProcessPacket();
    }
    return 0;
}

int Server::Run()
{
    while (true) {
        ProcessAccept();
        ProcessReceive();
        ProcessPacket();
        //sleep(10);
    }
    return 0;
}
