/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:42:57 by root              #+#    #+#             */
/*   Updated: 2023/07/10 22:42:50 by root             ###   ########.fr       */
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
    std::cout << "[DEBUG] - Closing " << this->name << "..." << std::endl;
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
        std::cout << "[DEBUG] - Waiting for events with poll()..." << std::endl;
        ready = poll(this->fds.data(), this->fds.size(), -1);
        if (ready == -1)
        {
            std::cout << "[DEBUG] - Error in poll()!" << std::endl;
            return (-1);
        }
        std::cout << "[DEBUG] - Number of events detected: " << ready << std::endl;
        
        /* Check if a new connection is in waiting */
        if (this->fds[0].revents & POLLIN)
        {
            std::cout << "[DEBUG] - New connection detected!" << std::endl;
            new_socket = this->acceptNewConnection();
            if (new_socket == -1)
            {
                std::cout << "[DEBUG] - Error in accept()!" << std::endl;
                continue ;
            }
            this->addNewClient(new_socket);
            std::cout << "[DEBUG] - New connection accepted. Client socket: " << new_socket << std::endl;
            this->handleNewConnection(*(this->clientsList.find(new_socket))->second);
        }

        /* Browse existing clients sockets */
        for (std::size_t i = 1; i < this->fds.size(); ++i)
        {
            if (this->fds[i].revents & POLLIN) /* Check if a new message is in waiting */
            {
                client_socket = this->fds[i].fd;
                std::cout << "[DEBUG] - Incoming event on client socket: " << client_socket << std::endl;
                if (this->handleEvent(client_socket) != 0)
                    return (-2);
            }
            else if (this->fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) /* Check for logout */
            {
                client_socket = this->fds[i].fd;
                std::cout << "[DEBUG] - Client disconnected: " << client_socket << std::endl;
                this->handleDisconnection(client_socket);
                i--; /* Client disconnected */
            }
        }
    } while (true);
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
    std::string welcome = "Welcome to " + this->name + "!";
    
    client.sendToFD(welcome);
}

/* Messages */
int Server::handleEvent(int client_socket)
{
    ssize_t		ret;
    char		buffer[BUFFER_SIZE + 1];
    std::string msg;

    msg.clear();
    ret = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (ret <= 0)
    {
        if (ret == 0)
        {
            /* Disconnection */
            std::cout << "[DEBUG] - Client disconnected: " << client_socket << std::endl;
            this->handleDisconnection(client_socket);
        }
        else
        {
            std::cout << "[DEBUG] - Error in recv() on client socket: " << client_socket << std::endl;
            return (-1);
        }
        return (0);
    }

    /* Processing of data received from the client */
    buffer[ret] = '\0'; // null terminate the string received
	msg = msg + buffer;
    std::cout << "[DEBUG] - Received message from client " << client_socket << ": " << msg << std::endl;
    return (0);
}

/* Logout */
void Server::handleDisconnection(int client_socket)
{
    /* Manage logout */
    std::map<int, Client*>::iterator    it = this->clientsList.find(client_socket);
    if (it != this->clientsList.end())
    {
        close(it->first);
        delete it->second;
        this->clientsList.erase(it);
    }
}

/* Utils */
int Server::createServerSocket(int port)
{
    struct sockaddr_in  addr;

    /* Creating socket server */
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket == -1)
    {
        std::cout << "[DEBUG] - Failed to create server socket" << std::endl;
        return (-1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    /* Bind data to the server socket */
    if (bind(this->serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        close(this->serverSocket);
        std::cout << "[DEBUG] - Failed to bind server socket" << std::endl;
        return (-2);
    }

    /* Put the server socket on listen */
    if (listen(this->serverSocket, SOMAXCONN) < 0)
    {
        close(this->serverSocket);
        std::cout << "[DEBUG] - Failed to listen on server socket" << std::endl;
        return (-3);
    }
    std::cout << "[DEBUG] - Server socket created and listening on port" << std::endl;
    return (0);
}