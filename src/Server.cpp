/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:42:57 by root              #+#    #+#             */
/*   Updated: 2023/07/12 18:33:14 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(std::string port_str, std::string password, std::string name)
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
    for (std::map<int, Client*>::iterator it = this->clientsList.begin(); it != this->clientsList.end(); ++it)
    {
        close(it->first);
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
        Utils::debug_message("Number of events detected: " + Utils::intToString(ready));
        
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
            this->handleNewConnection(*(this->clientsList.find(new_socket))->second);
        }

        /* Browse existing clients sockets */
        for (std::size_t i = 1; i < this->fds.size(); ++i)
        {
            if (this->fds[i].revents & POLLIN) /* Check if a new message is in waiting */
            {
                client_socket = this->fds[i].fd;
                Utils::debug_message("Incoming event on client socket: " + Utils::intToString(client_socket));
                if (this->handleEvent(client_socket) == 0)
                {
                    Utils::debug_message("Client disconnected on event: " + Utils::intToString(client_socket));
                    this->handleDisconnection(client_socket);
                
                    /* Remove from fds */
                    this->fds.erase(this->fds.begin() + i);

                    i--; /* Client disconnected */
                }
            }
            else if (this->fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) /* Check for logout */
            {
                client_socket = this->fds[i].fd;
                Utils::debug_message("Client disconnected on disconnection handling: " + Utils::intToString(client_socket));
                this->handleDisconnection(client_socket);
                
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
    return (client_socket);
}

void Server::addNewClient(int client_socket)
{
    struct pollfd   clientPfd;

    /* Add new client to the list */
    Client *new_client = new Client(client_socket);
    clientsList.insert(std::make_pair(client_socket, new_client));
    clientPfd.fd = client_socket;
    clientPfd.events = POLLIN;
    this->fds.push_back(clientPfd);
}

void Server::handleNewConnection(Client &client)
{
    Command::welcomeMessages(*this, client);
}

/* Messages */
int Server::handleEvent(int client_socket)
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

void Server::getMessages(const std::string &message, int from)
{
    Command command(message);

    std::map<int, Client*>::iterator clientIterator = this->clientsList.find(from);
    if (clientIterator != this->clientsList.end())
    {
        Client* client = clientIterator->second;

        if (command.getType() == PRIVMSG)
            std::cout << "PRIVMSG from client " << client->getFd() << std::endl;
        else if (command.getType() == NICK)
            Command::nickMessages(*this, client, command.getArgs().at(0));
    }
    else
        Utils::error_message("Client not found from socket: " + Utils::intToString(from));
}

/* Logout */
void Server::handleDisconnection(int client_socket)
{
    /* Manage logout */
    std::map<int, Client*>::iterator    it = this->clientsList.find(client_socket);

    if (this->clientsList.size() == 0)
        Utils::debug_message("No clients connected to the server");
    else
    {
        /* Clean clientsList map */
        close(it->first);
        delete it->second;
        this->clientsList.erase(it);
        
        Utils::debug_message("Logout done");
    }
}

/* Utils */
int Server::createServerSocket(int port)
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

std::vector<std::string> Server::splitCommands(const std::string &message, char delimiter)
{
    std::vector<std::string>    commands;
    std::stringstream           ss(message);
    std::string                 command;

    while (std::getline(ss, command, delimiter))
        commands.push_back(command);
    return (commands);
}

/* Getters */
int Server::getServerSocket() { return (this->serverSocket); }
std::string Server::getName() { return (this->name); }
std::string Server::getPassword() { return (this->password); }
std::vector<struct pollfd> Server::getFds() { return (this->fds); }
std::map<int, Client*> Server::getClientsList() { return (this->clientsList); }
std::map<std::string, Channel*> Server::getChannelsList() { return (this->channelsList); }