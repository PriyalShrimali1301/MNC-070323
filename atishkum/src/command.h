//
// Created by Atish Sinha on 2/27/23.
//

#ifndef MNC_COMMAND_H
#define MNC_COMMAND_H

#include <map>
#include <string>

class Command {
    public:
            void ip();
//            void ip_v2();
            void author();
            void port(int port);
            void refresh();
            char* list_buffer(int socket_fd, char* buffer, int buffer_size, char* output_buffer);
            void login();
            void startup();
            void exit();

//            void list();
};


#endif //MNC_COMMAND_H
