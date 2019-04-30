//
//  Connection.cpp
//  SingleThreadServer
//
//  Created by iann on 2019/4/29.
//  Copyright © 2019 iann. All rights reserved.
//

#include "Server.hpp"


Connection::Connection(int sock)
    :status(Connectted)
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
    else if (recvbyte < 0)
    {
        //printf("recvbyte < 0\n");
        int error = errno;
        
        if (errno == EAGAIN)
        {
            
        }
        else
        {
            printf("errno:%d\n",error);
            status = Breaked;
            return 1;
        }
        
        
        
    }
    else if (recvbyte == 0)
    {
        printf("recvbyte == 0\n");
        status = Breaked;
        return 1;
    }
    
    return 0;
}

int ParsingCmd(std::string packet, std::string &to, std::string &cmd, std::string &param)
{
    vector<string> stringlist;
    const char *str = packet.c_str();
    const char delimiter = ' ';
    std::stringstream ss;
    char c = 0;
    while (c = *str++) {
        if (c == delimiter)
        {
            stringlist.push_back(ss.str());
            ss.str("");
            ss.clear();
        }
        else
        {
            ss.put(c);
        }
    }
    stringlist.push_back(ss.str());
    if (stringlist.size()>0) to = stringlist[0];
    if (stringlist.size()>1) cmd = stringlist[1];
    if (stringlist.size()>2) param = stringlist[2];

    return 0;
}

int Connection::ProcessPacket()
{
    while (recvQueue.size()) {
        std::string packet = recvQueue.front();
        recvQueue.pop();
        std::string to, cmd, param;
        ParsingCmd(packet, to, cmd, param);
        
        if ((0 == to.compare("me")) || (0 == to.compare("self")))
        {
            if ((0 == cmd.compare("setname")) || (0 == cmd.compare("iam")))
            {
                username = param;
            }
            else if (0 == cmd.compare("whoami") || 0 == cmd.compare("whoami\r"))
            {
                sendQueue.push(string("i am ") + username + "\n");
            }
            else if (0 == cmd.compare("whoonline"))
            {
                
            }
        }
        
        
    }
    return 0;
    
}

int Connection::ProcessSend()
{
    if (sendQueue.size())
    {
        string sendCatch = sendQueue.front();
        sendQueue.pop();
        int ret = send(sock, (char*)sendCatch.c_str(), (int)sendCatch.size(), 0);
        if (ret != sendCatch.size())
        {
            printf("Error in %s", __FUNCTION__);
        }

    }
    return 0;
}

int Server::CreateSocket()
{
    int port = 5679;
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

int Connection::Close()
{
    close(sock);
    return 0;
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
        it != connections.end(); )
    {
        Connection *conn = *it;
        if (conn->ProcessRecieve())
        {
            conn->Close();
            it++;
            connections.remove(conn);
            delete conn;
        }
        else
        {
            it++;
        }
    }
    return 0;
}


int Server::ProcessPacket()
{
    for(std::list<Connection *>::iterator it = connections.begin();
        it != connections.end(); it++)
    {
        Connection *conn = *it;
        if (conn->ProcessPacket())
        {
            conn->Close();
            connections.remove(conn);
            delete conn;
        }
    }
    return 0;
}

int Server::ProcessSend()
{
    for(std::list<Connection *>::iterator it = connections.begin();
        it != connections.end(); it++)
    {
        Connection *conn = *it;
        if (conn->ProcessSend())
        {
            conn->Close();
            connections.remove(conn);
            delete conn;
        }
    }
    return 0;
}

int Server::Run()
{
    while (true) {
        ProcessAccept();
        ProcessReceive();
        ProcessPacket();
        ProcessSend();
        //sleep(10);
    }
    return 0;
}
