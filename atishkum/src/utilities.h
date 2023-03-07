//
// Created by Atish Sinha on 2/27/23.
//

#ifndef MNC_UTILITIES_H
#define MNC_UTILITIES_H
#include "command.h"
#include <map>
#include <string>
#include <vector>
class Utilities {
    public:
        std::map<std::pair<std::string, int>, std::pair<int, std::string> > m;
        int get_address_info(char* ip, char* node,struct addrinfo hints, addrinfo** res);
        int create_socket(int domain, int type, int protocol);
        void send_to_server(int socket_fd,char* msg );

//        void command_initializer(std::map<std::string, void (Command::*)(void)> &cf);
        std::string serialize(std::map<std::pair<std::string, int>, std::pair<int, std::string> > m);
        std::map<std::pair<std::string, int>, std::pair<int, std::string > > deserialize(char* s);
        void list_display(std::map<std::pair<std::string, int>, std::pair<int, std::string> > &m);
        bool validate_ip(char* server_ip);
        bool validate_port(char* port);
        std::vector<char*> create_tokens(char* const input, const char* seperator);


    };


#endif //MNC_UTILITIES_H
