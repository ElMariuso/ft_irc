/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:42:57 by root              #+#    #+#             */
/*   Updated: 2023/07/20 01:22:49 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(const std::string &port_str, const std::string &password, const std::string &name)
{
    int ret;

    /* Creating server socket */
    ret = this->createServerSocket(std::atoi(port_str.c_str()));
    if (ret < 0)
        throw (std::runtime_error("Problem during creating server socket!"));
    this->password = password;
    this->name = name;

    /* Start server processing */
    ret = this->processServer();
    if (ret < 0)
        throw (std::runtime_error("Problem during running the server!"));
}

Server::~Server()
{
    Utils::debug_message("Closing " + this->name + "...");
    /* Remove clients list */
    for (std::map<std::string, Client*>::iterator it = this->clientsList.begin(); it != this->clientsList.end(); ++it)
    {
        close(it->second->getFd());
        delete it->second;
    }
    this->clientsList.clear();

    /* Remove channels list */
    for (std::map<std::string, Channel*>::iterator it = this->channelsList.begin(); it != this->channelsList.end(); ++it)
    {
        delete it->second;
    }
    this->channelsList.clear();

    /* Close server Socket */
    close(this->serverSocket);
}

/* Main Process */
int Server::processServer()
{
    struct pollfd   serverPfd;
    int             ready;
    int             new_socket;
    int             client_socket;

    /* Initialization of the poll descriptor for the server socket */
    serverPfd.fd = this->serverSocket;
    serverPfd.events = POLLIN;
    this->fds.push_back(serverPfd);
    /* Main loop */
    do
    {
        /* Wait for events on sockets */
        Utils::debug_message("Waiting for events with poll()...");
        ready = poll(this->fds.data(), this->fds.size(), -1);
		if (Utils::stop(1))
			break ;
        if (ready == -1 && !Utils::stop(1))
        {
            Utils::error_message("Error in poll()!");
            return (-1);
        }
        
        /* Check if a new connection is in waiting */
        if (this->fds[0].revents & POLLIN)
        {
            Utils::debug_message("New connection detected!");
            new_socket = this->acceptNewConnection();
            if (new_socket == -1)
            {
                Utils::error_message("Error in accept()!");
                continue ;
            }
            this->addNewClient(new_socket);
            Utils::debug_message("New connection accepted. Client socket: " + Utils::intToString(new_socket));

            /* Ask for authentification */
            Client *client = this->findClientByFD(new_socket)->second;
            client->sendToFD(Message::connection(this->getName(), client->getNickname()));
        }

        /* Browse existing clients sockets */
        for (std::size_t i = 1; i < this->fds.size(); ++i)
        {
            client_socket = this->fds[i].fd;
            if (this->fds[i].revents & POLLIN) /* Check if a new message is in waiting */
            {
                Utils::debug_message("Incoming event on client socket: " + Utils::intToString(client_socket));
                if (this->handleEvent(client_socket) == 0)
                {
                    Utils::debug_message("Client disconnected on event: " + Utils::intToString(client_socket));
                    this->handleDisconnection(client_socket, "leaving");
                
                    /* Remove from fds */
                    this->fds.erase(this->fds.begin() + i);

                    i--; /* Client disconnected */
                }
            }
	    if ((this->fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) || this->pingTimeOut(client_socket) == true ) /* Check for logout */
            {
                Utils::debug_message("Client disconnected on disconnection handling: " + Utils::intToString(client_socket));
                this->handleDisconnection(client_socket, "leaving");
                
                /* Remove from fds */
                this->fds.erase(this->fds.begin() + i);

                i--; /* Client disconnected */
            }
	    else
            this->sendPingMessage(client_socket);
        }
    } while (!Utils::stop(1));
    return (0);
}

/* New connection */
int Server::acceptNewConnection()
{
    int         client_socket;
    sockaddr_in client_addr;
    socklen_t   client_addr_len;

    /* Accept new connection */
    client_addr_len = sizeof(client_addr);
    client_socket = accept(this->serverSocket, (struct sockaddr*)&client_addr, &client_addr_len);
    fcntl(client_socket, F_SETFL, O_NONBLOCK);
    return (client_socket);
}

void Server::addNewClient(const int client_socket)
{
    struct pollfd   clientPfd;

    /* Add new client to the list */
    std::string nickname = "Guest" + client_socket;
    std::string username = "User" + client_socket;

    Client *new_client = new Client(nickname, username, client_socket, true);
    clientsList.insert(std::make_pair(nickname, new_client));
    clientPfd.fd = client_socket;
    clientPfd.events = POLLIN;
    this->fds.push_back(clientPfd);
}

/* Commands */
int Server::handleEvent(const int client_socket)
{
    ssize_t		                ret;
    char		                buffer[BUFFER_SIZE + 1];
    std::string                 msg;
    std::vector<std::string>    commands;

    msg.clear();
    ret = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (ret <= 0)
    {
        if (ret < 0)
        {
            Utils::error_message("Error in recv() on client socket: " + Utils::intToString(client_socket));
            return (ret);
        }
        return (ret);
    }

    /* Processing of data received from the client */
    buffer[ret] = '\0'; // null terminate the string received
	msg = msg + buffer;
    Utils::debug_message(Utils::intToString(client_socket) + " send a message: " + msg);

    commands = this->splitCommands(msg, '\n');
    for (std::size_t i = 0; i != commands.size(); ++i)
        this->getMessages(commands.at(i), client_socket);
    return (ret);
}

void Server::getMessages(const std::string &message, const int client_socket)
{
    Command command(message);

    std::map<std::string, Client*>::const_iterator it = this->findClientByFD(client_socket);
    if (it != this->clientsList.end())
    {
        Client  *client = it->second;
        
        /* No need to authenticate */
        this->withoutAuthentification(command, client);

        /* Need to authenticate */
        if (client->getIsConnected() && client->getIsAuthenticated())
            this->withAuthentification(command, client);
        else if (client->getIsConnected() && !client->getIsAuthenticated() && command.getType() != UNKNOW)
            client->sendToFD(Message::err_notregistered_451(this->getName(), client->getNickname()));
    }
    else
        Utils::error_message("Client not found from socket: " + Utils::intToString(client_socket));
}

void Server::withoutAuthentification(const Command &command, Client *client)
{
    if (command.getType() == QUIT) /* Bye :( */
        this->handleDisconnection(client->getFd(), command.getArgs().at(0));
    else if ((command.getType() == PASS) && client->getIsAuthenticated()) /* Already registered */
        client->sendToFD(Message::err_alreadyregistered_462(this->getName(), client->getNickname()));
    else if (command.getType() == PASS) /* Authentification */
    {
        if (this->password == command.getArgs().at(0)) /* Ok */
        {
            client->sendToFD(Message::welcome(this->getName(), client->getNickname(), client->getUsername(), client->getHostname()));
            client->setIsAuthenticated(true);
        }
        else /* No */
            client->sendToFD(Message::err_passwdmismatch_464(this->getName(), client->getNickname()));
    }
}

void Server::withAuthentification(const Command &command, Client *client)
{
    (void)client;
    switch (command.getType())
    {
        case NICK:
            command.nick(*this, client, command.getArgs().at(0));
            break ;
        case JOIN:
            break ;
        case PART:
            break ;
        case PRIVMSG:
            break ;
        default:
            break ;
    }
}

/* Ping message */
void Server::sendPingMessage(const int client_socket)
{
    (void)client_socket;
	// std::map<int, Client*>::iterator	it = this->clientsList.find(client_socket);
	// std::string				identifier;
	// std::string				pingMessage;

	// std::srand(std::time(0));
	// identifier = Utils::intToString(std::rand()); //random identifier
	// pingMessage = "PING " + identifier;
	// if (it->second->getTimeSinceLastPing() >= 300) //time between 2 ping msg in seconds
	// {
	// 	it->second->sendToFD(pingMessage);
	// 	it->second->setTimeSinceLastPing();
	// 	it->second->setLastPingIdentifier(identifier);
	// }
}

bool Server::pingTimeOut(const int client_socket)
{
    (void)client_socket;
	// std::map<int, Client*>::iterator    it = this->clientsList.find(client_socket);

	// if (it->second->getTimeSinceLastPing() >= 600 && it->second->getLastPingIdentifier() != "-1" )
	// 	return (true);
	// return (false);
    return (false);
}

/* Logout */
void Server::handleDisconnection(const int client_socket, const std::string &message)
{
    (void)message;

    if (this->clientsList.size() == 0)
        Utils::debug_message("No clients connected to the server");
    
    /* Manage logout */
    std::map<std::string, Client*>::iterator        it = this->findClientByFD(client_socket);
    Client                                          *client;
    int                                             fd;

    if (it != this->clientsList.end())
        client = it->second;
    else
        client = NULL;
    if (client != NULL && client->getIsConnected() == true)
    {
        fd = client->getFd();
        client->setIsConnected(false);
        
        /* Clean channels */
        for (std::map<std::string, Channel*>::iterator it1 = this->channelsList.begin(); it1 != this->channelsList.end(); ++it1)
        {
            Channel *channel = it1->second;

            std::map<std::string, Client*>::iterator it2 = channel->findConnectedByFD(client_socket);
            if (it2 != channel->getConnected().end())
            {
                // TODO

                // Command::partMessages(this, *(it->second), channel->getName(), message);


                channel->rmOp(*it->second);   
            }
            if (this->channelsList.empty())
                break ;
        }

        /* Clean clientsList map */
        close(fd);
        delete client;
        this->clientsList.erase(it);
        
        Utils::debug_message("Logout done");
    }
}

/* Utils */
int Server::createServerSocket(const int port)
{
    struct sockaddr_in  addr;
    int                 optVal;
    int                 optValSize;

    optVal = 1;
    optValSize = sizeof(optVal);
    /* Creating socket server */
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, optValSize) == -1)
    {
        Utils::error_message("Failed to create server socket options");
        return (-1);
    }
	fcntl(this->serverSocket, F_SETFL, O_NONBLOCK);
    if (this->serverSocket == -1)
    {
        Utils::error_message("Failed to create server socket");
        return (-2);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    /* Bind data to the server socket */
    if (bind(this->serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        close(this->serverSocket);
        Utils::error_message("Failed to bind server socket on port: " + Utils::intToString(port));
        return (-3);
    }

    /* Put the server socket on listen */
    if (listen(this->serverSocket, SOMAXCONN) < 0)
    {
        close(this->serverSocket);
        Utils::error_message("Failed to listen on server socket");
        return (-4);
    }
    Utils::debug_message("Server socket created and listening on port");
    return (0);
}

std::vector<std::string> Server::splitCommands(const std::string &message, const char delimiter)
{
    std::vector<std::string>    commands;
    std::stringstream           ss(message);
    std::string                 command;

    while (std::getline(ss, command, delimiter))
        commands.push_back(command);
    return (commands);
}

/* Setters */
void Server::setServerSocket(int serverSocket) { this->serverSocket = serverSocket; }
void Server::setName(std::string name) { this->name = name; }
void Server::setPassword(std::string password) { this->password = password; }
void Server::setFd(struct pollfd fd) { this->fds.push_back(fd); }
void Server::setFds(std::vector<struct pollfd> fds) { this->fds = fds; }
void Server::setClient(const std::string &name, Client *client) { this->clientsList.insert(std::make_pair(name, client)); }
void Server::setClients(std::map<std::string, Client*> clients) { this->clientsList = clients; }
void Server::setChannel(std::string name, Channel *channel) { this->channelsList.insert(std::make_pair(name, channel)); }
void Server::setChannels(std::map<std::string, Channel*> channels) { this->channelsList = channels; }

/* Removers */
void Server::removeChannel(Channel *channel)
{
    this->channelsList.erase(channel->getName());
    delete channel;
}

/* Getters */
int Server::getServerSocket() const { return (this->serverSocket); }
std::string Server::getName() const { return (this->name); }
std::string Server::getPassword() const { return (this->password); }
std::vector<struct pollfd> Server::getFds() const { return (this->fds); }
std::map<std::string, Client*> Server::getClientsList() const { return (this->clientsList); }
std::map<std::string, Channel*> Server::getChannelsList() const { return (this->channelsList); }

/* Finders */
template <typename List>
typename List::mapped_type* Server::findElem(const List &list, const std::string &name) const
{
    typename List::const_iterator it = list.find(name);
    
    if (it != list.end())
        return (it->second);
    return (NULL);
}

std::map<std::string, Client*>::const_iterator Server::findClientByFD(const int fd, bool isConst) const
{
    (void)isConst;
    const std::map<std::string, Client*>  &clients = this->clientsList;
    
    for (std::map<std::string, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->getFd() == fd)
            return (it);
    }
    return (clients.end());
}

std::map<std::string, Client*>::iterator Server::findClientByFD(const int fd)
{
    std::map<std::string, Client*>  &clients = this->clientsList;
    
    for (std::map<std::string, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->getFd() == fd)
            return (it);
    }
    return (clients.end());
}
