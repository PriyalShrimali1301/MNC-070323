//
// Created by Atish Sinha on 2/27/23.
//

#include "command.h"
#include "../include/logger.h"
#include <unistd.h>
#include "string.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <stdio.h>
#include "utilities.h"

#define MAX_LEN 1024
typedef void(Command::*commandFunctionType)(void);



void Command::author() {
    cse4589_print_and_log("[%s:SUCCESS]\n", "AUTHOR");
    std::string name = "atishkum";
    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", name.c_str());
    cse4589_print_and_log("[%s:END]\n", "AUTHOR");
}

void Command::ip(){
    char hostname[MAX_LEN];
    struct hostent *ht;
    char ip_add[64];
    if(gethostname(hostname, MAX_LEN) == 0) {
        if((ht = gethostbyname(hostname))!=NULL){
            strcpy(ip_add, inet_ntoa(*((struct in_addr *)ht->h_addr)));
//            std::cout << "IP address is: " << ip_add << std::endl;
            cse4589_print_and_log("[%s:SUCCESS]\n", "IP");

            cse4589_print_and_log("IP:%s\n", ip_add);
            cse4589_print_and_log("[%s:END]\n", "IP");


        }
    }
}


void Command::port(int port) {
    cse4589_print_and_log("[%s:SUCCESS]\n", "PORT");
    cse4589_print_and_log("PORT:%d\n", port);
    cse4589_print_and_log("[%s:END]\n", "PORT");

}
void refresh() {
	 std::cout << "Connected clients: ";
	        for (int i = 0; i < clientSockets.size(); i++) {
	            std::cout << clientSockets[i] << " ";
	        }
	        std::cout << "\n";
}
char* Command::list_buffer(int socket_fd, char* mssg, int buffer_size, char* buffer){
//    std::cout<<"from inside list_buffer : "<<mssg;
    send(socket_fd, mssg, strlen(mssg), 0);
    if (recv(socket_fd, buffer, buffer_size, 0) >= 0) {
//        std::cout << "Server responded: " << buffer;
//        std::cout.flush();
    }
    return buffer;
}

void login(char *serverip, char* port){

}
void startup(){

}

void Command::exit(){

}

