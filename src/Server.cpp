/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:42:57 by root              #+#    #+#             */
/*   Updated: 2023/07/23 17:02:15 by mthiry           ###   ########.fr       */
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
    this->name = name;

    /* Start server processing */
    this->setLastPingTime(clock());
    ret = this->processServer();
    if (ret < 0)
        throw (std::runtime_error("Problem during running the server!"));
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
        /* Check last PING */
        clock_t currentTime = clock();
        double elapsedTime = static_cast<double>(currentTime - lastPingTime) / CLOCKS_PER_SEC;
        double elapsedTimeMilliseconds = elapsedTime * 1000;
        if (elapsedTimeMilliseconds >= 53.000) /* Send PING to all connected clients - Latence of 7 seconds on the real time */
        {
            this->sendToAll(this->ping());
            std::cout << std::endl;
            lastPingTime = currentTime;
        }

        /* Wait for events on sockets */
        Utils::waiting_message("Waiting for events with poll()");
        ready = poll(this->fds.data(), this->fds.size(), 100);
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
            Client *client = this->findClient(new_socket);
            client->sendToFD(this->connection(client->getNickname()));
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
	        if ((this->fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) || this->clientsList.find(client_socket)->second->getPingCount() == 4) /* Check for logout */
            {
                Utils::debug_message("Client disconnected on disconnection handling: " + Utils::intToString(client_socket));
                this->handleDisconnection(client_socket, "leaving");
                
                /* Remove from fds */
                this->fds.erase(this->fds.begin() + i);

                i--; /* Client disconnected */
            }
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
    std::string nickname = "Guest" + Utils::intToString(client_socket);
    std::string username = "User" + Utils::intToString(client_socket);

    Client *new_client = new Client(nickname, username, client_socket, true);
    clientsList.insert(std::make_pair(client_socket, new_client));
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
    Command                                 command(message);
    Client                                  *client;
    std::vector<std::string>                args = command.getArgs();
    std::map<int, Client*>::const_iterator  it = this->clientsList.find(client_socket);

    if (command.getArgs().size() <= 2)
        args.push_back("");   
    if (command.getArgs().size() == 1)
        args.push_back("");
    if (it != this->clientsList.end())
    {
        client = it->second;
        
        /* No need to authenticate */
        this->withoutAuthentification(command, client, args[0]);

        /* Need to authenticate */
        if (client->getIsConnected() && client->getIsAuthenticated())
            this->withAuthentification(command, client, args);
        else if (client->getIsConnected() && !client->getIsAuthenticated() && command.getType() != UNKNOW)
            client->sendToFD(this->err_notregistered_451(client->getNickname()));
    }
    else
        Utils::error_message("Client not found from socket: " + Utils::intToString(client_socket));
}

void Server::withoutAuthentification(const Command &command, Client *client, const std::string &arg0)
{
    int                 fd = client->getFd();
    const std::string   &nickname = client->getNickname();
    const std::string   &username = client->getUsername();
    const std::string   &hostname = client->getHostname();

    switch (command.getType())
    {
        case QUIT: /* Bye :( */
            this->handleDisconnection(fd, arg0);
            break ;
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
        case USER:
            command.user(*this, client);
            break ;
        case PING:
            client->sendToFD(this->pong());
            break ;
        case PONG:
        {
            const std::string &arg = arg0.substr(1);
            if (arg == this->name)
                client->setPingCount(0);
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
        case JOIN:
            command.join(this, client, args[0], args[1], this->findChannel(args[0]));
            break ;
        case PART:
            command.part(this, *client, args[0], args[1], this->findChannel(args[0]));
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
    if (this->clientsList.size() == 0)
        Utils::debug_message("No clients connected to the server");
    
    /* Manage logout */
    std::map<int, Client*>::iterator    it = this->clientsList.find(client_socket);
    Client                              *client = this->findClient(client_socket);
    int                                 fd;

    if (client != NULL && client->getIsConnected() == true)
    {
        fd = client->getFd();
        client->setIsConnected(false);
        
        /* Clean channels */
        for (std::map<std::string, Channel*>::iterator it1 = this->channelsList.begin(); it1 != this->channelsList.end(); ++it1)
        {
            Channel *channel = it1->second;

            const std::map<int, Client*>            &connected = channel->getConnected();

            std::map<int, Client*>::const_iterator  it2 = connected.find(client_socket);
            if (it2 != connected.end())
            {
                Command command;
                const std::string   &channelName = channel->getName();
                
                command.part(this, *(it->second), channelName, message, this->findChannel(channelName));
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
void Server::setName(std::string name) { this->name = name; this->setServerName(name); }
void Server::setPassword(std::string password) { this->password = password; }
void Server::setFd(struct pollfd fd) { this->fds.push_back(fd); }
void Server::setFds(std::vector<struct pollfd> fds) { this->fds = fds; }
void Server::setClient(const int &fd, Client *client) { this->clientsList.insert(std::make_pair(fd, client)); }
void Server::setClients(std::map<int, Client*> clients) { this->clientsList = clients; }
void Server::setChannel(std::string name, Channel *channel) { this->channelsList.insert(std::make_pair(name, channel)); }
void Server::setChannels(std::map<std::string, Channel*> channels) { this->channelsList = channels; }
void Server::setDate(const std::string &date) { this->date = date; }
void Server::setLastPingTime(clock_t time) { this->lastPingTime = time; }

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
std::map<int, Client*> Server::getClientsList() const { return (this->clientsList); }
std::map<std::string, Channel*> Server::getChannelsList() const { return (this->channelsList); }
std::string Server::getDate() const { return (this->date); }
clock_t Server::getLastPingTime() const { return (this->lastPingTime); };

std::map<int, Client*>::const_iterator Server::getClientsListEnd() const { return (this->clientsList.end()); }
std::map<std::string, Channel*>::const_iterator Server::getChannelsListEnd() const { return (this->channelsList.end()); }

/* Finders */
Client* Server::findClient(const int &fd) const
{
    std::map<int, Client*>              map = this->clientsList;
    std::map<int, Client*>::iterator    it = map.find(fd);
    
    if (it != map.end())
        return (it->second);
    return (NULL);
}

Channel* Server::findChannel(const std::string &name) const
{
    const std::map<std::string, Channel*>           &map = this->channelsList;
    std::map<std::string, Channel*>::const_iterator it = map.find(name);
    
    if (it != map.end())
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

    if (clients.empty())
    {
        Utils::debug_message("No clients connected to the server");
        return ;
    }
	for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		const Client &client = *(it->second);

		client.sendToFD(message);
	}
}


void Server::sendPingToAll()
{
    const std::map<int, Client*>	&clients = this->clientsList;

    if (clients.empty())
    {
        Utils::debug_message("No clients connected to the server");
        return ;
    }
	for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client *client = it->second;

		client->incPingCount();
	} 
}