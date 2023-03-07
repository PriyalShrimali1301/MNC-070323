//
// Created by Atish Sinha on 2/25/23.
//

#ifndef MNC_SERVER_H
#define MNC_SERVER_H
#include <iostream>

//#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include "../include/global.h"
#include "../include/logger.h"
#include <arpa/inet.h>

class Server {
    public:
            struct f_descriptors{
                int sock_fd;
                int bind_fd;
                int listen_fd;
            };
//            int get_address_info(char* ip, char* node,struct addrinfo hints, addrinfo** res);
//            int create_socket(int domain, int type, int protocol);
            int bind_socket(int sd, sockaddr* addr, socklen_t t);
            int listen_socket(int socket_descriptor, int no_of_sockets_allowed);
            f_descriptors socket_set_up_and_listen(Utilities ut, struct addrinfo* p);
            int run(char* ip, char* port, Utilities ut, Command comm, std::map<std::string, void(Command::*)(void)>& str_to_f);

};


#endif //MNC_SERVER_H
