/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:42:57 by root              #+#    #+#             */
/*   Updated: 2023/07/26 23:45:17 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(const std::string &port_str, const std::string &password, const std::string &name): Message(name)
{
    int ret;

    this->setDate(Utils::getDate());
    /* Creating server socket */
    ret = this->createServerSocket(std::atoi(port_str.c_str()));
    if (ret < 0)
        throw (std::runtime_error("Problem during creating server socket!"));
    this->password = password;
}

Server::~Server()
{
    Utils::debug_message("Closing " + this->name + "...");
    /* Remove clients list */
    for (std::map<int, Client*>::iterator it = this->clientsList.begin(); it != this->clientsList.end(); ++it)
    {
        close(it->second->getFd());
        delete it->second;
    }
    this->clientsList.clear();

    /* Remove channels list */
    for (std::map<std::string, Channel*>::iterator it = this->channelsList.begin(); it != this->channelsList.end(); ++it)
        delete it->second;
    this->channelsList.clear();

    /* Close server Socket */
    close(this->serverSocket);
}

/* Main Process */
int Server::processServer()
{
    int             ret;
    time_t          currentTime;

    /* Initialization of the poll descriptor for the server socket */
    this->setServerPfd();
    /* Main loop */
    while (!Utils::stop(1))
    {
        currentTime = time(NULL);
        
        /* PING system */
        this->pingSystem(currentTime);

        /* Wait for events on sockets */
        ret = this->pollChecking();
        if (ret == 1)
            break ;
        else if (ret == -1)
        {
            Utils::error_message("Error in poll()!");
            break ;
        }
        
        /* Check if a new connection is in waiting */
        ret = this->checkForNewConnection();
        if (ret == -1)
        {
            Utils::error_message("Error in accept()!");
            continue ;
        }
        else if (ret == -2)
        {
            Utils::error_message("Can't set socket to non-blocking!");
            break ;
        }
        else if (ret == -3)
        {
            Utils::error_message("Failed to allocate memory for new client");
            break ;
        }
        
        /* Browse existing clients sockets */
        this->browseClients(currentTime);
    }
    return (0);
}

void Server::setServerPfd()
{
    struct pollfd   serverPfd;
    
    serverPfd.fd = this->serverSocket;
    serverPfd.events = POLLIN;
    this->fds.push_back(serverPfd);
}

void Server::pingSystem(time_t currentTime)
{
    if (!this->clientsList.empty()) /* Send PING to all connected clients */
    {
        for (std::map<int, Client*>::iterator it = this->clientsList.begin(); it != this->clientsList.end(); ++it)
        {
            if (it->second->getLastActivityTime() + 120 < currentTime)
                it->second->sendToFD(this->ping());
        }
    }
    else
        Utils::debug_message("No clients connected to the server");
}

int Server::pollChecking()
{
    int                         ready;
    std::vector<struct pollfd>  fds_vector(this->fds.begin(), this->fds.end());

    // Utils::waiting_message("Waiting for events with poll()");
    ready = poll(fds_vector.data(), fds_vector.size(), 333);
	if (Utils::stop(1))
        return (1);
    if (ready == -1 && !Utils::stop(1))
    {
        int savedErrno = errno;
        if (savedErrno == EINTR)
            Utils::error_message("poll() interrupted by a signal");
        else
            Utils::error_message("Error in poll()");
        return (ready);
    }
    else if (ready != 0)
        Utils::debug_message("Event detected!");
    return (0);
}

int Server::checkForNewConnection()
{
    int     new_socket = 0;
    Client  *client;
    
    if (this->fds.begin()->revents & POLLIN)
    {
        Utils::debug_message("New connection detected!");
        new_socket = this->acceptNewConnection();
        if (new_socket < 0)
            return (new_socket);
        client = this->addNewClient(new_socket);
        if (!client)
            return (-3);
        Utils::debug_message("New connection accepted. Client socket: " + Utils::intToString(new_socket));

        /* Ask for authentification */
        client->sendToFD(this->connection(client->getNickname()));
    }
    return (new_socket);
}

void Server::browseClients(time_t currentTime)
{
    int ret = 1;
    
    for (std::list<struct pollfd>::iterator it = ++this->fds.begin(); it != this->fds.end();)
    {
        Client              *client = this->findClient(it->fd);

        if (!client)
        {
            Utils::error_message("Can't find client on iteration");
            it = this->fds.erase(it);
            continue ;
        }

        /* Used for messages */
        const std::string   &username = client->getUsername();

        if (it->revents & POLLIN) /* Check if a new message is in waiting */
        {
            Utils::debug_message("Incoming event for " + username);
            ret = this->handleEvent(it->fd);
        }
        if (ret <= 0 || (it->revents & (POLLHUP | POLLERR | POLLNVAL))) /* Check for logout */
        {
            Utils::debug_message(username + " leaving on revents");
            this->handleDisconnection(it->fd, "leaving");
            it = this->fds.erase(it);
            ret = 1;
            continue ;
        }
        else if ((client->getLastActivityTime() + 120 < currentTime) && (client->getLastPingTime() + 600 < currentTime)) /* Time out */
        {
            this->handleDisconnection(it->fd, " timed out");
            it = this->fds.erase(it);
            continue ;
        }
        ++it;
    }
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
    if (client_socket == -1)
        return (client_socket);
    if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
        return (-2);
    return (client_socket);
}

Client* Server::addNewClient(const int client_socket)
{
    struct pollfd   clientPfd;
    Client          *new_client;

    /* Add new client to the list */
    std::string nickname = "ft_Guest" + Utils::intToString(client_socket);
    std::string username = "ft_User" + Utils::intToString(client_socket);

    try
    {
        new_client = new Client(nickname, username, client_socket, true);
        clientsList.insert(std::make_pair(client_socket, new_client));
        clientPfd.fd = client_socket;
        clientPfd.events = POLLIN;
        this->fds.push_back(clientPfd);
    }
    catch (std::bad_alloc &ba)
    {
        return (NULL);
    }
    return (new_client);
}

/* Commands */
int Server::handleEvent(const int client_socket)
{
    ssize_t		                ret;
    char		                buffer[BUFFER_SIZE + 1];
    std::string                 msg;
    std::vector<std::string>    commands;
    
    ret = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (ret <= 0)
    {
        if (ret < 0)
            Utils::error_message("Error in recv() on client socket: " + Utils::intToString(client_socket));
        return (-1);
    }
    
    /* Processing of data received from the client */
    buffer[ret] = '\0'; // null terminate the string received
	msg = msg + buffer;
    Utils::debug_message(Utils::intToString(client_socket) + " send a message - " + msg);

    commands = this->splitCommands(msg, '\n');
    for (std::size_t i = 0; i != commands.size(); ++i)
        this->getMessages(commands[i], client_socket);
    return (0);
}

void Server::getMessages(const std::string &message, const int client_socket)
{
    Command                                 command(message);
    Client                                  *client;
    std::vector<std::string>                args = this->setArgsCommands(command);
    std::map<int, Client*>::const_iterator  it = this->clientsList.find(client_socket);
    
    if (it != this->clientsList.end() && command.getType() != UNKNOW)
    {
        client = it->second;
        client->setLastActivityTime(time(NULL));

        if (command.getType() == QUIT)
        {
            this->handleDisconnection(client_socket, args[0]);
            return ;
        }
        if (client->getIsConnected())
            this->withoutAuthentification(command, client, args[0]);
        if (client->getIsConnected() && client->getIsAuthenticated())
            this->withAuthentification(command, client, args);
        else if (client->getIsConnected() && !client->getIsAuthenticated())
            client->sendToFD(this->err_notregistered_451(client->getNickname()));
    }
    else if (command.getType() == UNKNOW)
        Utils::debug_message("Unknow command");
    else
        Utils::error_message("Client not found from socket: " + Utils::intToString(client_socket));
}

std::vector<std::string> Server::setArgsCommands(const Command &command)
{
    std::vector<std::string>    ret;

    ret = command.getArgs();
    while (ret.size() < 3)
        ret.push_back("");
    return (ret);
}

void Server::withoutAuthentification(const Command &command, Client *client, const std::string &arg0)
{
    const std::string   &nickname = client->getNickname();
    const std::string   &username = client->getUsername();
    const std::string   &hostname = client->getHostname();
    
    switch (command.getType())
    {
        case PASS:
            if (client->getIsAuthenticated()) /* Already registered */
                client->sendToFD(this->err_alreadyregistered_462(nickname));
            else
            {
                if (this->password == arg0) /* Ok */
                {
                    client->sendToFD(this->welcome(nickname, username, hostname, this->date));
                    client->setIsAuthenticated(true);
                }
                else /* No */
                    client->sendToFD(this->err_passwdmismatch_464(nickname));
            }
            break ;
        case PING:
            client->sendToFD(this->pong());
            break ;
        case PONG:
        {
            const std::string &arg = arg0.substr(1);
            if (arg == this->name)
                client->setLastPingTime(time(NULL));
            else
                client->sendToFD(this->err_nosuchserver_402(client->getNickname(), arg));
            break ;
        }
        default:
            break ;
    }
}

void Server::withAuthentification(const Command &command, Client *client, const std::vector<std::string> &args)
{
    switch (command.getType())
    {
        case NICK:
            command.nick(*this, client, args[0]);
            break ;
        case USER:
            command.user(*this, client);
            break ;
        case JOIN:
            command.join(this, client, args[0], args[1]);
            break ;
        case PART:
            command.part(this, *client, this->findChannel(args[0]), args[0], args[1]);
            break ;
        case PRIVMSG:
            command.privmsg(*this, *client, args[0], args[1]);
            break ;
        case MODE:
            command.mode(*this, client, args[0], args[1], args[2]);
            break ;
        case TOPIC:
            command.topic(*this, *client, args[0], args[1]);
            break ;
        case KICK:
            command.kick(*this, *client, this->findClientByName(args[1])->second, args[2], this->findChannel(args[0]));
            break ;
        case INVITE:
            command.invite(*this, *client, args[0], args[1]);
            break ;
        default:
            break ;
    }
}

/* Logout */
void Server::handleDisconnection(const int client_socket, const std::string &message)
{
    /* Manage logout */
    std::map<int, Client*>::iterator    it = this->clientsList.find(client_socket);

    if (it == this->clientsList.end())
    {
        Utils::debug_message("Client not found");
        return ;
    }

    Client                              *client = it->second;
    int                                 fd = client->getFd();

    if (client && client->getIsConnected())
    {
        client->setIsConnected(false);
        
        /* Clean channels */
        for (std::map<std::string, Channel*>::iterator it1 = this->channelsList.begin(); it1 != this->channelsList.end(); ++it1)
        {
            Channel                                 *channel = it1->second;
            const std::map<int, Client*>            &connected = channel->getConnected();
            std::map<int, Client*>::const_iterator  it2 = connected.find(client_socket);
            
            if (it2 != connected.end())
            {
                Command command;
                const std::string   &channelName = channel->getName();
                
                command.part(this, *(it->second), this->findChannel(channelName), channelName, message);
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
    if (this->serverSocket == -1)
    {
        Utils::error_message("Failed to create server socket");
        return (-1);
    }
    
    if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, optValSize) == -1)
    {
        close(this->serverSocket);
        Utils::error_message("Failed to create server socket options");
        return (-2);
    }

	if (fcntl(this->serverSocket, F_SETFL, O_NONBLOCK) == -1)
    {
        close(this->serverSocket);
        Utils::error_message("Failed to set server socket to non-blocking");
        return (-3);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    /* Bind data to the server socket */
    if (bind(this->serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        close(this->serverSocket);
        Utils::error_message("Failed to bind server socket on port: " + Utils::intToString(port));
        return (-4);
    }

    /* Put the server socket on listen */
    if (listen(this->serverSocket, SOMAXCONN) < 0)
    {
        close(this->serverSocket);
        Utils::error_message("Failed to listen on server socket");
        return (-5);
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
void Server::setPassword(std::string password) { this->password = password; }
void Server::setFd(struct pollfd fd) { this->fds.push_back(fd); }
void Server::setFds(std::list<struct pollfd> fds) { this->fds = fds; }
void Server::setClient(const int &fd, Client *client) { this->clientsList.insert(std::make_pair(fd, client)); }
void Server::setClients(std::map<int, Client*> clients) { this->clientsList = clients; }
void Server::setChannel(std::string name, Channel *channel) { this->channelsList.insert(std::make_pair(name, channel)); }
void Server::setChannels(std::map<std::string, Channel*> channels) { this->channelsList = channels; }
void Server::setDate(const std::string &date) { this->date = date; }

/* Removers */
void Server::removeChannel(Channel *channel)
{
    this->channelsList.erase(channel->getName());
    delete channel;
}

/* Getters */
int Server::getServerSocket() const { return (this->serverSocket); }
std::string Server::getPassword() const { return (this->password); }
std::list<struct pollfd> Server::getFds() const { return (this->fds); }
std::map<int, Client*> Server::getClientsList() const { return (this->clientsList); }
std::map<std::string, Channel*> Server::getChannelsList() const { return (this->channelsList); }
std::string Server::getDate() const { return (this->date); }

std::map<int, Client*>::const_iterator Server::getClientsListEnd() const { return (this->clientsList.end()); }
std::map<std::string, Channel*>::const_iterator Server::getChannelsListEnd() const { return (this->channelsList.end()); }

/* Finders */
Client* Server::findClient(const int &fd) const
{
    std::map<int, Client*>::const_iterator  it = this->clientsList.find(fd);
    
    if (it != this->clientsList.end())
        return (it->second);
    return (NULL);
}

Channel* Server::findChannel(const std::string &name) const
{
    std::map<std::string, Channel*>::const_iterator it = this->channelsList.find(name);
    
    if (it != this->channelsList.end())
        return (it->second);
    return (NULL);
}

std::map<int, Client*>::const_iterator Server::findClientByName(const std::string &name) const
{
    for (std::map<int, Client*>::const_iterator it = this->clientsList.begin(); it != this->clientsList.end(); ++it)
    {
        if (it->second->getNickname() == name)
            return (it);
    }
    return (this->clientsList.end());
}

/* Senders */
void Server::sendToAll(const std::string &message)
{
    const std::map<int, Client*>	&clients = this->clientsList;
    
	for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		const Client &client = *(it->second);

		client.sendToFD(message);
	}
}