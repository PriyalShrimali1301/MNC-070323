/**
 * @atishkum_assignment1
 * @author  Atish Kumar Sinha <atishkum@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
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
#include "utilities.h"
#include "command.h"
#include "server.h"
#include "client.h"


/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/* Clear LOGFILE*/
    fclose(fopen(LOGFILE, "w"));

    Utilities ut = Utilities();
    std::map<std::string, void(Command::*)()> str_to_f;
    Command cmd = Command();
//    ut.command_initializer(str_to_f);

//    for(auto it = str_to_f.begin(); it!=str_to_f.end();it++){
//        std::cout<<it->second<<std::endl;
//    }

    if((std::string)argv[1]=="s"){
        std::cout<<"Hi from the server"<<std::endl;
        Server s = Server();
        s.run(NULL, argv[2], ut, cmd, str_to_f);
    }


    if((std::string)argv[1]=="c"){
        std::cout<<"Hi from the client"<<std::endl;
        Client c = Client();
//        c.run("128.205.36.46", argv[2], ut);
        c.run(NULL, argv[2], ut, cmd);
    }

	return 0;
}
