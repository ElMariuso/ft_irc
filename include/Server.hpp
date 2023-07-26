/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:30:52 by root              #+#    #+#             */
/*   Updated: 2023/07/26 22:38:08 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "library.hpp"

class Client;
class Channel;
class Command;

class Server: public Message
{
    public:
        Server(const std::string &port_str, const std::string &password, const std::string &name);
        ~Server();

        /* Main Process */
        int                                             processServer();
        void                                            setServerPfd();
        void                                            pingSystem(time_t currentTime);
        int                                             pollChecking();
        int                                             checkForNewConnection();
        void                                            browseClients(time_t currentTime);

        /* New connection */
        int                                             acceptNewConnection();
        void                                            addNewClient(const int client_socket);

        /* Commands */
        int                                             handleEvent(const int client_socket);
        void                                            getMessages(const std::string &message, const int client_socket);
        void                                            withoutAuthentification(const Command &command, Client *client, const std::string &arg0);
        void                                            withAuthentification(const Command &command, Client *client, const std::vector<std::string> &args);

        /* Logout */
        void                                            handleDisconnection(const int client_socket, const std::string &message);
        
        /* Utils */
        int                                             createServerSocket(const int port);
        std::vector<std::string>                        splitCommands(const std::string &message, const char delimiter);

        /* Setters */
        void                                            setServerSocket(int serverSocket);
        void                                            setPassword(std::string password);
        void                                            setFd(struct pollfd fd);
        void                                            setFds(std::list<struct pollfd> fds);
        void                                            setClient(const int &fd, Client *client);
        void                                            setClients(std::map<int, Client*> clients);
        void                                            setChannel(std::string name, Channel *channel);
        void                                            setChannels(std::map<std::string, Channel*> channels);
        void                                            setDate(const std::string &date);

        /* Removers */
	    void                                            removeChannel(Channel *channel);

        /* Getters */
        int                                             getServerSocket() const;
        std::string                                     getPassword() const;
        std::list<struct pollfd>                        getFds() const;
        std::map<int, Client*>                          getClientsList() const;
        std::map<std::string, Channel*>                 getChannelsList() const;
        std::string                                     getDate() const;

        std::map<int, Client*>::const_iterator          getClientsListEnd() const;
        std::map<std::string, Channel*>::const_iterator getChannelsListEnd() const;

        /* Finders */
        Client*                                         findClient(const int &fd) const;
        Channel*                                        findChannel(const std::string &name) const;
        std::map<int, Client*>::const_iterator          findClientByName(const std::string &name) const;

        /* Senders */
        void                                            sendToAll(const std::string &message);

    private:
        int                             serverSocket;
        std::string                     password;
        std::list<struct pollfd>        fds;
        std::map<int, Client*>          clientsList;
        std::map<std::string, Channel*> channelsList;
        std::string                     date;
};

#endif
