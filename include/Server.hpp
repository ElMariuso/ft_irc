/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:30:52 by root              #+#    #+#             */
/*   Updated: 2023/07/10 22:39:40 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "library.hpp"

class Client;
class Channel;
class Socket;

class Server
{
    public:
        Server(std::string port_str, std::string password, std::string name);
        ~Server();

        /* Main Process */
        int     processServer();

        /* New connection */
        int     acceptNewConnection();
        void    addNewClient(int client_socket);
        void    handleNewConnection(Client &client);

        /* Messages */
        int     handleEvent(int client_socket);

        /* Logout */
        void    handleDisconnection(int client_socket);
        
        /* Utils */
        int     createServerSocket(int port);

    private:
        int                             serverSocket;
        std::string                     name;
        std::string                     password;
        std::vector<struct pollfd>      fds;
        std::map<int, Client*>          clientsList;
        std::map<std::string, Channel*> channelsList;
};

#endif