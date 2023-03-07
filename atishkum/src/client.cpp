//
// Created by Atish Sinha on 2/27/23.
//
#include "utilities.h"
#include "command.h"
#include "client.h"
#include<string.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>

#define TRUE 1
#define MSG_SIZE 256
#define BUFFER_SIZE 256


#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <cstring>
#include "../include/logger.h"
#include <typeinfo>


std::vector<int> client_sockets;

int Client::connect_to_server(int sock_fd, struct addrinfo* res){
    return connect(sock_fd, res->ai_addr, res->ai_addrlen);
}


Client::f_descriptors_client Client::connect_to_host(char *server_ip, char *port, struct addrinfo hints, Utilities ut, int &failure_flag)
{
    f_descriptors_client fd;
    struct addrinfo *res;
    int rv;

    std::cout<<server_ip<<" : "<<port<<" \n";
    rv = ut.get_address_info(server_ip, port, hints, &res);
    if(rv !=0 ){
        failure_flag=1;
        //perror("Failure in the function getaddrinfo :");
    }

    /* Socket */
    fd.sock_fd = ut.create_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(fd.sock_fd == -1){
        failure_flag=1;
        //perror("server: socket");
    }
    /* Connect */
    if (connect_to_server(fd.sock_fd, res) < 0){
        failure_flag=1;
        //perror("Connect failed");
    }

    freeaddrinfo(res);

    return fd;
}


int Client::run(char* server, char* port, Utilities ut, Command comm)
{
//    std::cout<<"Hey I am inside the client"<<std::endl;
    int list_flag = 0;
    int login_flag=0;
    f_descriptors_client fd;
    char* server_ip;
    char* port_number;

    struct addrinfo hints, *res;

    /* Set up hints structure */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    while(TRUE){
        char *buffer = new char[2048];
        memset(buffer, '\0', BUFFER_SIZE);

//        std::cout << "\n[PA1-Client@CSE489/589]$ ";
//        std::cout.flush();

        char *msg = new char[MSG_SIZE];
        memset(msg, '\0', MSG_SIZE);
        if (fgets(msg, MSG_SIZE - 1, stdin) == NULL){
            exit(-1);
        }
        msg[std::strcspn(msg, "\n")] = '\0';
        std::string s = (std::string)msg;


        if(s.length() >5){
            if(s.substr(0,5).compare("LOGIN")==0) {
                login_flag=1;
                int failure_flag = 0;
                char *dupmsg = strdup(msg);
                std::vector<char *> out = ut.create_tokens(dupmsg, " ");
                strcat(strcat(msg, " "),port);
                if(out.size() == 3){
                    server_ip = out[1];
                    port_number = out[2];
                }else{
                    cse4589_print_and_log("[%s:ERROR]\n", "LOGIN");
                    cse4589_print_and_log("[%s:END]\n", "LOGIN");
                    failure_flag=1;
                }

                server = strdup(server_ip);
                char* port_ = strdup(port_number);
//                std::cout << "out  : "<<out[1]<<":"<<out[2];
//                std::cout<<"----"<<typeid(ut.validate_ip(server_ip)).name();
                if(failure_flag == 0){
                    try {
                        if(ut.validate_ip(server) == true && ut.validate_port(port_) == true){
                            fd = connect_to_host(server_ip, port_number, hints, ut, failure_flag);
                            if(failure_flag==0){
                                cse4589_print_and_log("[%s:SUCCESS]\n", "LOGIN");
                                cse4589_print_and_log("[%s:END]\n", "LOGIN");
                                client_sockets.push_back(new_socket);
                                ut.m = ut.deserialize(comm.list_buffer(fd.sock_fd, msg, 2048, buffer));
                            }
                            else{
                                cse4589_print_and_log("[%s:ERROR]\n", "LOGIN");
                                cse4589_print_and_log("[%s:END]\n", "LOGIN");
                            }
                        }
                        else{
                            cse4589_print_and_log("[%s:ERROR]\n", "LOGIN");
                            cse4589_print_and_log("[%s:END]\n", "LOGIN");
                        }
                        free(server);
                        free(port_);
                        free(dupmsg);

                    } catch (std::exception& e) {
                        cse4589_print_and_log("[%s:ERROR]\n", "LOGIN");
                        cse4589_print_and_log("[%s:END]\n", "LOGIN");
                    }
                }

            }
            else if(s.compare("AUTHOR")==0){
                comm.author();
            }
            else if(s.compare("REFRESH") == 0){
                try{
                    if(login_flag == 1){
                            ut.m = ut.deserialize(comm.list_buffer(fd.sock_fd, msg, 2048, buffer));
                            comm.refresh()
                            cse4589_print_and_log("[%s:SUCCESS]\n", "REFRESH");
                            cse4589_print_and_log("[%s:END]\n", "REFRESH");
                    }else{
                            cse4589_print_and_log("[%s:ERROR]\n", "REFRESH");
                            cse4589_print_and_log("[%s:END]\n", "REFRESH");
                    }

                }
                catch (std::exception& e) {
                    cse4589_print_and_log("[%s:ERROR]\n", "REFRESH");
                    cse4589_print_and_log("[%s:END]\n", "REFRESH");
                }
            }
        }
        else if(s.compare("IP") == 0) {
            comm.ip();
        }
        else if(s.compare("PORT") == 0) {
            comm.port(std::atoi(port));
        }
        else if(s.compare("EXIT") == 0){
            strcat(strcat(msg, " "), port);
            send(fd.sock_fd, msg, strlen(msg), 0);
            close(fd.sock_fd);
            cse4589_print_and_log("[%s:SUCCESS]\n", "EXIT");
            cse4589_print_and_log("[%s:END]\n", "EXIT");
            exit(0);
        }
        else if (s.compare("LIST") == 0){
            ut.list_display(ut.m);
        }
        else if(s.length()>0){
            std::cout<<"Invalid command\n";
        }
        else{
            if (recv(fd.sock_fd, buffer, BUFFER_SIZE, 0) >= 0) {
//                std::cout << "Server responded: " << buffer;
                std::cout.flush();
            }else{
            }
        }


        delete[] buffer;
        delete[] msg;


    }

}


