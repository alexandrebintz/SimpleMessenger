//
//  main.c
//  SimpleMessenger
//
//  Created by Alexandre Bintz on 23/05/14.
//  Copyright (c) 2014 Alexandre Bintz. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MODE_SERVER 1
#define MODE_CLIENT 2

// command line args:
//
// client mode, connects to server on given IP and port
// ./pgm -c 10.20.30.40 4000
//
// server mode, listens to given address and port
// ./pgm -s 127.0.0.1 4000

int main(int argc, const char * argv[])
{
    // check for command lines arguments and configure execution
    
    if(argc<4)
    {
        printf("USAGE: %s -c|-s addr port\n",argv[0]);
        return 0;
    }
    
    int mode = -1;  // client or server mode, see #defines
    long port = strtol(argv[3],0,10);
    
    if(strcmp(argv[1],"-c")==0)
    {
        mode = MODE_CLIENT;
        printf("running in client mode on %s:%ld\n",argv[2],port);
    }
    else if(strcmp(argv[1],"-s")==0)
    {
        mode = MODE_SERVER;
        printf("running in server mode on %s:%ld\n",argv[2],port);
    }
    
    // create socket
    
    int s = socket(AF_INET, SOCK_STREAM,0);
    if(s < 0)
    {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }
    
    // bind
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    if(mode==MODE_SERVER)
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        if(bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        {
            perror("Error binding socket");
            exit(EXIT_FAILURE);
        }
        
        // listen
        
        if(listen(s, 1) < 0)
        {
            perror("Error listening");
            exit(EXIT_FAILURE);
        }
        
        // accept
        
        struct sockaddr client_addr;
        unsigned int socklen = sizeof(client_addr); // socklen_t is not accessible on OSX, but it is equivalent to uint
        int sc = accept(s, &client_addr, &socklen);
        
        char buff[100];
        read(sc, buff, 100);
        
        printf("%s",buff);
    }
    else if(mode==MODE_CLIENT)
    {
        inet_aton(argv[2],&addr.sin_addr);
        
        if(connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        {
            perror("Error connecting");
            exit(EXIT_FAILURE);
        }
        
        char buff[] = "Hello, World!";
        write(s, buff, 14);
        printf("\n");
    }
    
    // close socket
    
    if(close(s) < 0)
    {
        perror("Error closing socket");
        exit(EXIT_FAILURE);
    }
    
    printf("exiting\n");
    
    return 0;
}

