//
//  main.c
//  SimpleMessenger
//
//  Created by Alexandre Bintz on 23/05/14.
//  Copyright (c) 2014 Alexandre Bintz. All rights reserved.
//

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MODE_SERVER 1
#define MODE_CLIENT 2

#define SERVER_MODE_INFRA 1
#define SERVER_MODE_ADHOC 2

void usage(const char* argv0);
void help(const char* argv0);

void client(int sock,char* host,long port);
void server(int sock,long port,int s_mode);

//
// see help function for command line args
//
int main(int argc, const char * argv[]) {

    // parse command lines arguments
    
    int cs_mode = -1;   // client or server mode: MODE_SERVER|MODE_CLIENT
    int s_mode  = -1;   // server mode: SERVER_MODE_INFRA|SERVER_MODE_ADHOC
    char str_host[15];  // dots-and-numbers format (e.g. "192.168.5.10") i.e. 15 char max
    char str_port[5];   // ports range from 0 to 65635 i.e. 5 char max
    
    if(argc >= 2 && strcmp(argv[1],"--help")==0) {
        help(argv[0]);
        return EXIT_SUCCESS;
    }
    else if(argc >= 4 && strcmp(argv[1],"-c")==0) {
        cs_mode = MODE_CLIENT;
        strncpy(str_host, argv[2], 15);
        strncpy(str_port, argv[3], 5);
    }
    else if(argc >= 3 && strcmp(argv[1],"-s")==0) {
        cs_mode = MODE_SERVER;
        s_mode  = SERVER_MODE_ADHOC;
        strncpy(str_port, argv[2], 5);
    }
    else if(argc >= 3 && strcmp(argv[1],"-ss")==0) {
        cs_mode = MODE_SERVER;
        s_mode  = SERVER_MODE_INFRA;
        strncpy(str_port, argv[2], 5);
    }
    else {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }
    
    // process input data
    
    errno = 0;
    long port = strtol(str_port,0,10);
    if(errno!=0) {
        perror("strtol");
        printf("error parsing port\n");
        return EXIT_SUCCESS;
    }

    // create socket
    
    int s = socket(AF_INET, SOCK_STREAM,0);
    if(s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // run client/server specific code
    
    if(cs_mode==MODE_SERVER) {
        server(s,port,s_mode);
    }
    else if(cs_mode==MODE_CLIENT) {
        client(s,str_host,port);
    }
    
    // close socket
    
    if(close(s) < 0) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    
    // exit program
    
    printf("exiting\n");
    
    return EXIT_SUCCESS;
}

//
// client specific code
//
void client(int s,char* str_host,long port) {
    
    // connect
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int r = inet_pton(AF_INET,str_host,&addr.sin_addr);
    if(r==0) {
        printf("error parsing address: invalid address\n");
        return;
    }
    else if(r==-1) {
        perror("inet_pton");
        return;
    }
    
    if(connect(s, (struct sockaddr*)&addr, sizeof(addr))<0) {
        perror("connect");
        return;
    }
    
    // send data
    
    char buff[] = "Hello, World!";
    write(s, buff, 14);
}

//
// server specific code
//
void server(int s,long port,int s_mode) {
    
    // bind
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);   // listen to any address
    
    if(bind(s, (struct sockaddr*)&addr, sizeof(addr))<0) {
        perror("bind");
        return;
    }
    
    // listen
    
    if(listen(s, 1)<0) {
        perror("listen");
        return;
    }

    // accept
    
    struct sockaddr client_addr;
    unsigned int socklen = sizeof(client_addr); // socklen_t is not accessible on OSX, but it is equivalent to uint
    int sc = accept(s, &client_addr, &socklen);
    
    // read data
    
    char buff[100];
    read(sc, buff, 100);
    printf("%s",buff);
}

//
// displays usage hint
//
void usage(const char* argv0)
{
    printf("USAGE: %s -c host port\n"
           "       %s -s port\n"
           "       %s -ss port\n"
           "\n"
           "type %s --help for more help\n"
           "\n",argv0,argv0,argv0,argv0);
}

//
// prints help page
//
void help(const char* argv0)
{
    printf("Simple Messenger help:\n"
           "\n"
           "%s -c 127.0.0.1 7777\n"
           "client mode, connects to server on given IP and port\n"
           "receives and sends messages from/to the server\n"
           "\n"
           "%s -s 7777\n"
           "adhoc server mode, listens on given port\n"
           "allow a unique client to connect\n"
           "receives and sends messages from/to the client\n"
           "\n"
           "%s -ss 7777\n"
           "infrastructure server mode, listens on given port\n"
           "allow multiple clients to connect\n"
           "receives messages from a client and send it to all connected clients\n"
           "\n",argv0,argv0,argv0
           );
}

