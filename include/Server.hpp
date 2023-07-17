/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:30:52 by root              #+#    #+#             */
/*   Updated: 2023/07/17 17:33:13 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "library.hpp"

class Client;
class Channel;

class Server
{
    public:
        Server(const std::string port_str, const std::string password, const std::string name);
        ~Server();

        /* Main Process */
        int                             processServer();

        /* New connection */
        int                             acceptNewConnection();
        void                            addNewClient(int client_socket);

        /* Messages */
        int                             handleEvent(int client_socket);
        void                            getMessages(const std::string &message, int from);
        
        /* Logout */
        void                            handleDisconnection(int client_socket);
        
        /* Utils */
        int                             createServerSocket(int port);
        std::vector<std::string>        splitCommands(const std::string &message, char delimiter);

        /* Setters */
        void                            setServerSocket(int serverSocket);
        void                            setName(std::string name);
        void                            setPassword(std::string password);
        void                            setFd(struct pollfd fd);
        void                            setFds(std::vector<struct pollfd> fds);
        void                            setClient(int fd, Client *client);
        void                            setClients(std::map<int, Client*> clients);
        void                            setChannel(std::string name, Channel *channel);
        void                            setChannels(std::map<std::string, Channel*> channels);

        /* Getters */
        int                             getServerSocket() const;
        std::string                     getName() const;
        std::string                     getPassword() const;
        std::vector<struct pollfd>      getFds() const;
        std::map<int, Client*>          getClientsList() const;
        std::map<std::string, Channel*> getChannelsList() const;

    private:
        int                             serverSocket;
        std::string                     name;
        std::string                     password;
        std::vector<struct pollfd>      fds;
        std::map<int, Client*>          clientsList;
        std::map<std::string, Channel*> channelsList;
};

#endif