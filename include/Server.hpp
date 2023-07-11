/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:30:52 by root              #+#    #+#             */
/*   Updated: 2023/07/12 00:17:20 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "library.hpp"

class Client;
class Channel;
class Message;

class Server
{
    public:
        Server(std::string port_str, std::string password, std::string name);
        ~Server();

        /* Main Process */
        int                             processServer();

        /* New connection */
        int                             acceptNewConnection();
        void                            addNewClient(int client_socket);
        void                            handleNewConnection(Client &client);

        /* Messages */
        int                             handleEvent(int client_socket);
        void                            getMessages(const std::string &message, int from);

        /* Commands */
        void                            nickCommand(Message &new_message, int from);
        void                            msgCommand(Message &new_message, int from);

        /* Logout */
        void                            handleDisconnection(int client_socket);
        
        /* Utils */
        int                             createServerSocket(int port);
        std::vector<std::string>        splitCommands(const std::string &message, char delimiter);

        /* Getters */
        int                             getServerSocket();
        std::string                     getName();
        std::string                     getPassword();
        std::vector<struct pollfd>      getFds();
        std::map<int, Client*>          getClientsList();
        std::map<std::string, Channel*> getChannelsList();

    private:
        int                             serverSocket;
        std::string                     name;
        std::string                     password;
        std::vector<struct pollfd>      fds;
        std::map<int, Client*>          clientsList;
        std::map<std::string, Channel*> channelsList;
};

#endif