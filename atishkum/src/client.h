//
// Created by Atish Sinha on 2/27/23.
//

#ifndef MNC_CLIENT_H_H
#define MNC_CLIENT_H_H
#include<string>
#include <vector>

class Client {
    public:
        struct f_descriptors_client{
            int sock_fd;
        };
       void tokenize(std::string const &str, const char* delim,
            std::vector<char* > &out);
        int connect_to_server(int sock_fd, struct addrinfo* res);
        f_descriptors_client connect_to_host(char *server_ip, char *server_port, struct addrinfo hints, Utilities ut, int &failure_flag);
        int run(char *server_ip, char *server_port, Utilities ut, Command comm);
//        void another_run(Utilities ut, Command comm);

    };


#endif //MNC_CLIENT_H_H
