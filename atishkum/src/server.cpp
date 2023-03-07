//
// Created by Atish Sinha on 2/25/23.
//
#include "utilities.h"
#include "command.h"
#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256


//int Server::get_address_info(char* ip, char* node,struct addrinfo hints,struct addrinfo** res){
//    return getaddrinfo(ip, node, &hints, res);
//}


//int Server::create_socket(int domain, int type, int protocol) {
//    return socket(domain, type, protocol);
//}

int Server::bind_socket(int sd, sockaddr *addr, socklen_t t) {
    return bind(sd, addr, t);
}

int Server::listen_socket(int socket_descriptor, int no_of_sockets_allowed=BACKLOG) {
    return listen(socket_descriptor,no_of_sockets_allowed);
}

Server::f_descriptors Server::socket_set_up_and_listen(Utilities ut, struct addrinfo* p) {
    f_descriptors fd;
    /*
  * Creation of socket.
  */
    fd.sock_fd = ut.create_socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if(fd.sock_fd == -1){
        perror("server: socket");
    }

    /*
     * Binding of socket.
     */
    fd.bind_fd = bind_socket(fd.sock_fd, p->ai_addr, p->ai_addrlen);
    if(fd.bind_fd==-1){
        close(fd.sock_fd);
        perror("server:bind");
    }
    /*
     * Once the socket is bound we need not use the res again, so we free it.
     */
    freeaddrinfo(p);

    /*
     * Listening for incoming connections.
     */
    fd.listen_fd = listen_socket(fd.sock_fd);
    if(fd.listen_fd ==- 1){
        perror("listen");
        exit(1);
    }
    return fd;
}




int Server::run(char* ip, char* port, Utilities ut, Command comm, std::map<std::string, void(Command::*)(void)>& str_to_f){
    std::cout<<"From inside the server"<<std::endl;
    int head_fd, socket_index, selector_return, fdaccept=0;
    int caddr_len;
    struct addrinfo *res;
    struct sockaddr_in client_addr;

    int list_flag=0;
    int rv;
    fd_set master_list, watch_list;

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /*
     * Get the address information and populate the structures for later use.
     * It Returns the pointer to res (results).
     */
    rv = ut.get_address_info(NULL, port, hints, &res);
    if(rv !=0 ){
      perror("Failure in the function getaddrinfo :");
    }


    /*
     * Sets up the socket, binds socket to the port and starts listening on that port
     */
    f_descriptors fd = socket_set_up_and_listen(ut, res);

    std::cout<<"waiting for connection"<<std::endl;

    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    /* Register the listening socket */
    FD_SET(fd.sock_fd, &master_list);
    /* Register STDIN */
    FD_SET(STDIN, &master_list);

    head_fd = fd.sock_fd;

    while(TRUE){
        memcpy(&watch_list, &master_list, sizeof(master_list));

        //printf("\n[PA1-Server@CSE489/589]$ ");
        //fflush(stdout);

        /* select() system call. This will BLOCK */
        selector_return = select(head_fd + 1, &watch_list, NULL, NULL, NULL);
        if(selector_return < 0)
            perror("select failed.");

        /* Check if we have sockets/STDIN to process */
        if(selector_return > 0){
            /* Loop through socket descriptors to check which ones are ready */
            for(socket_index=0; socket_index<=head_fd; socket_index+=1){

                if(FD_ISSET(socket_index, &watch_list)){

                    /* Check if new command on STDIN */
                    if (socket_index == STDIN){
//                        std::cout<<"Got standard input"<<std::endl;
                        char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

                        memset(cmd, '\0', CMD_SIZE);
                        if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
                            exit(-1);
                        cmd[strcspn(cmd, "\n")] = '\0';
//                        printf("\nI got: %s\n", cmd);
                        //Process PA1 commands here ...
                        std::string cmds = (std::string)cmd;
                        if(cmds.compare("IP") == 0){
//                            std::cout<<"I am here\n";
                            comm.ip();
                        }
                        else if(cmds.compare("AUTHOR") == 0){
                            comm.author();
                        }
                        else if (cmds.compare("LIST") == 0){
                            if(list_flag == 0) {
                                std::string s1 = ut.serialize(ut.m);
                                char *myCharArray = new char[s1.length() + 1]; // Add 1 for null terminator
                                std::strcpy(myCharArray, s1.c_str());
                                ut.m = ut.deserialize(myCharArray);
                                list_flag=1;
                            }
                            ut.list_display(ut.m);
                        }
                        else if (cmds.compare("PORT") == 0){
                            comm.port(std::atoi(port));
                        }
                        else if (cmds.compare("EXIT") == 0){
                            close(socket_index);
                            exit(0);
                        }

                        free(cmd);
                    }
                        /* Check if new client is requesting connection */
                    else if(socket_index == fd.sock_fd){
                        std::cout<<"socket index : "<<fd.sock_fd<<" \n";
                        caddr_len = sizeof(client_addr);
                        fdaccept = accept(fd.sock_fd, (struct sockaddr* )&client_addr, (socklen_t*)&caddr_len);

                        std::cout<<"fd accept : "<<fdaccept<<" \n";

                        if(fdaccept < 0)
                            perror("Accept failed.");
                        struct sockaddr_in addr;
                        socklen_t addr_size = sizeof(struct sockaddr_in);
                        char hostname_info[NI_MAXHOST];
                        int result_hostname = getnameinfo((struct sockaddr *)&client_addr, sizeof(client_addr), hostname_info, NI_MAXHOST, NULL, 0, 0);

                        getpeername(fdaccept, (struct sockaddr*) &addr, &addr_size );
                        char *clientip = new char[20];
                        strcpy(clientip, inet_ntoa(addr.sin_addr));
//                        printf("Peer connected %s:%d\n", clientip, ntohs(addr.sin_port));
//
//                        printf("\nRemote Host connected!\n");

                        /* Add to watched socket list */
                        FD_SET(fdaccept, &master_list);
                        if(fdaccept > head_fd)
                            head_fd = fdaccept;
                    }
                        /* Read from existing clients */
                    else{
//                        std::cout<<"Got it else";
                        /* Initialize buffer to receieve response */
                        char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                        memset(buffer, '\0', BUFFER_SIZE);

                        if(recv(socket_index, buffer, BUFFER_SIZE, 0) <= 0){
                            close(socket_index);
//                            printf("Remote Host terminated connection!\n");

                            /* Remove from watched list */
                            FD_CLR(socket_index, &master_list);
                        }
                        else {
                            //Process incoming data from existing clients here ...

//                            printf("\nClient sent me: %s\n", buffer);


//                            printf("ECHOing it back to the remote host ... ");
//                            std::cout<<"Buffer : "<<((std::string)buffer).substr(0,5)<<" "<<((std::string)buffer).substr(0,5).compare("LOGIN");
                            if(((std::string)buffer).substr(0,5).compare("LOGIN")==0){
//                                std::cout<<"ut serialize : "<<ut.serialize(ut.m).c_str()<<std::endl;
                                char* dupbuffer = strdup(buffer);
                                std::vector<char *> temp1 = ut.create_tokens(dupbuffer, " ");
//                                std::cout<<"temp1:  "<<temp1[2];
                                struct sockaddr_in addr;
                                socklen_t addr_size = sizeof(struct sockaddr_in);
                                getpeername(socket_index, (struct sockaddr*) &addr, &addr_size );
                                char hostname_info[NI_MAXHOST];
                                int result_hostname = getnameinfo((struct sockaddr *)&client_addr, sizeof(client_addr), hostname_info, NI_MAXHOST, NULL, 0, 0);


                                char *clientip = new char[20];
                                strcpy(clientip, inet_ntoa(addr.sin_addr));
//                                printf("Peer connected %s:%d\n", clientip, ntohs(addr.sin_port));
                                if(ut.m.count(std::make_pair(clientip, std::atoi(temp1[2]))) > 0){
                                    ut.m[std::make_pair(clientip,std::atoi(temp1[3]))] = std::make_pair(1, hostname_info);
                                }
                                else{
                                    ut.m[std::make_pair(clientip,std::atoi(temp1[3]))] = std::make_pair(1, hostname_info);
                                }
                                if(send(socket_index, ut.serialize(ut.m).c_str(), strlen(ut.serialize(ut.m).c_str()), 0) == strlen(ut.serialize(ut.m).c_str())){

                                }

//                                    printf("Done!\n");
                            }
                            else if(((std::string)buffer).substr(0,4).compare("EXIT")==0){
                                char* dupbuffer = strdup(buffer);
                                std::vector<char *> temp1 = ut.create_tokens(dupbuffer, " ");
//                                std::cout<<"temp1:  "<<temp1[1];

                                struct sockaddr_in addr_for_exit;
                                socklen_t addr_size_for_exit = sizeof(struct sockaddr_in);
                                getpeername(socket_index, (struct sockaddr*) &addr_for_exit, &addr_size_for_exit );
                                char *clientip_for_exit = new char[20];
                                strcpy(clientip_for_exit, inet_ntoa(addr_for_exit.sin_addr));
//                                printf("Peer Exiting %s:%d\n", clientip_for_exit, ntohs(addr_for_exit.sin_port));
                                ut.m.erase(std::make_pair(clientip_for_exit,std::atoi(temp1[1])));
                            }
                            else if(((std::string)buffer).compare("REFRESH")==0){
                                if(send(socket_index, ut.serialize(ut.m).c_str(), strlen(ut.serialize(ut.m).c_str()), 0) == strlen(ut.serialize(ut.m).c_str()))
                                {

                                }
//                                    printf("refresh done!\n");
                            }
                            else{
                                if(send(socket_index, buffer, strlen(buffer), 0) == strlen(buffer))
                                {

                                }
//                                    printf("Done!\n");
                            }
                            fflush(stdout);
                        }

                        free(buffer);
                    }
                }
            }
        }
    }


}

