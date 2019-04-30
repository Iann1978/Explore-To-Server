//
//  main.cpp
//  SingleThreadServer
//
//  Created by iann on 2019/4/29.
//  Copyright © 2019 iann. All rights reserved.
//


#include "Server.hpp"
int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    int error = 0;
    
    Server &server = Server::ins;
    
    if (error = server.CreateSocket())
    {
        return -1;
    }
    
    if (error = server.Run())
    {
        return -1;
    }
    
    
    
    return error;
}
