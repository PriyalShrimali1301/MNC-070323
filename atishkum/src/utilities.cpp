//
// Created by Atish Sinha on 2/27/23.
//

#include "utilities.h"
#include <iostream>
#include "command.h"
#include <sys/socket.h>
#include <netdb.h>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include "../include/logger.h"
#include <cstdlib>
#include <cstring>
#include <cctype>


int Utilities::get_address_info(char* ip, char* node,struct addrinfo hints, addrinfo** res){
    return getaddrinfo(ip, node, &hints, res);
}
int Utilities::create_socket(int domain, int type, int protocol){
    return socket(domain, type, protocol);
}

//std::vector<std::pair<std::pair<std::string, int>, std::pair<int, std::string> > > sort_vector(std::vector<std::pair<std::pair<std::string, int>, std::pair<int, std::string> > >& A) {
//    int n = A.size();
//
//    std::vector<std::pair<std::pair<std::string, int>, std::pair<int, std::string> > > v;
//    for(int i =0; i< n-1;i++){
//        v.push_back(A[i]);
//    }
//    return v;
//}

void Utilities::send_to_server(int socket_fd,char* msg ){
    if (send(socket_fd, msg, strlen(msg), 0) == strlen(msg)){
        std::cout << "Done!\n";
    }
}

std::string Utilities::serialize(std::map<std::pair<std::string, int>, std::pair<int, std::string> > m) {
    std::string s= "";
    std::vector<std::pair<std::pair<std::string, int>, std::pair<int, std::string> > > A;
    for(std::map<std::pair<std::string, int>, std::pair<int, std::string> >::iterator it= m.begin(); it!=m.end(); it++){
        A.push_back(*it);
    }
    int i = 0;
    for( i = 0; i<A.size(); i++){
        std::string c = "";
        if(A[i].second.first == 1){
	    std::stringstream ss, ss1;
	    ss << A[i].first.second;
	    ss1 << A[i].second.first;
            c += A[i].first.first + "#" + ss.str() + "#" +ss1.str() + "#"+ A[i].second.second;
        }
        s+=c + "|";
    }
    return s;
}

std::map<std::pair<std::string, int>, std::pair<int, std::string> > Utilities::deserialize(char* s) {
    //Create a temporary map;
    std::map<std::pair<std::string, int>, std::pair<int, std::string> > tm;
    std::string temp;
    std::stringstream ss(s);
    std::vector<std::string> words;

    while (getline(ss, temp, '|')) {
        words.push_back((std::string)temp);
    }
    for(int i =0; i< words.size(); i++){
        std::string temp2;
        std::stringstream ss1(words[i]);
        std::vector<std::string> tokens;
        while (getline(ss1, temp2, '#')) {
            tokens.push_back(temp2);
        }
//        std::cout<<"tokens - "<<tokens[0]<<" "<<tokens[1]<<" "<<tokens[2]<<" "<<tokens[3]<<std::endl;
        tm[std::make_pair(tokens[0], std::atoi(tokens[1].c_str()))] = std::make_pair(std::atoi(tokens[2].c_str()), tokens[3]);
    }
    return tm;
}

void Utilities::list_display(std::map<std::pair<std::string, int>, std::pair<int, std::string> > &m){
    std::vector<std::pair<std::pair<std::string, int>, std::pair<int, std::string> > > A;
    for(std::map<std::pair<std::string, int>, std::pair<int, std::string> >::iterator it= m.begin(); it!=m.end(); it++){
        A.push_back(std::make_pair(std::make_pair(it->first.first, it->first.second), std::make_pair(it->second.first, it->second.second)));
    }
    int n = A.size();

    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (A[j].first.second > A[j + 1].first.second) {
                std::swap(A[j], A[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
    int i = 1, i1=0;
    cse4589_print_and_log("[%s:SUCCESS]\n", "LIST");

    for(int i1 =0; i1< A.size(); i1++){
        if(A[i1].second.first == 1){
            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i++, A[i1].second.second.c_str(), (A[i1].first.first).c_str(), A[i1].first.second);
        }
    }
    cse4589_print_and_log("[%s:END]\n", "LIST");
}


bool Utilities::validate_ip(char* server_ip){
    char *dup_ser= server_ip;
    if(server_ip == NULL){
        return false;
    }
    std::vector<char*> tokens = create_tokens(server_ip, ".");
        //preceding zero// integer
        if(tokens.size() == 4) {
            if ((tokens[0][0] == '0') || (tokens[1][0] == '0') || (tokens[2][0] == '0') || (tokens[3][0] == '0')) {
                return false;
            }
            for (int i = 0; i < tokens.size(); i++) {
                for (int j = 0; j < strlen(tokens[i]); j++) {
                    if (std::isdigit(tokens[i][j]) == 0) {
                        return false;
                    }
                }
            }

            if (std::atoi(tokens[0]) >= 0 && std::atoi(tokens[0]) <= 255 && std::atoi(tokens[1]) >= 0 &&
                std::atoi(tokens[1]) <= 255 &&
                std::atoi(tokens[2]) >= 0 && std::atoi(tokens[2]) <= 255 && std::atoi(tokens[3]) >= 0 &&
                std::atoi(tokens[3]) <= 255)
                return true;
        }
        return false;
}

bool Utilities::validate_port(char* port){
    if(port == NULL){
        return false;
    }
    for(int i = 0; i< strlen(port);i++){
        if(std::isdigit(port[i]) == 0){
            return false;
        }
    }
    if(std::atoi(port) >= 0 && std::atoi(port) <=65535)
        return true;
    return false;
}

std::vector<char*> Utilities::create_tokens(char* const input, const char* seperator)
{
    std::vector<char*> temp;
    char *token = strtok(input, seperator);
    while (token != NULL)
    {
        temp.push_back(token);
        token = strtok(NULL, seperator);
    }
    return temp;
}

