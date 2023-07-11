/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:42:57 by root              #+#    #+#             */
/*   Updated: 2023/07/11 14:19:24 by bvernimm         ###   ########.fr       */
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
    Debug::debug_message("Closing " + this->name + "...");
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
        Debug::debug_message("Waiting for events with poll()...");
        ready = poll(this->fds.data(), this->fds.size(), -1);
		if (Debug::stop(1))
			break ;
        if (ready == -1 && !Debug::stop(1))
        {
            Debug::error_message("Error in poll()!");
            return (-1);
        }
        Debug::debug_message("Number of events detected: " + Debug::intToString(ready));
        
        /* Check if a new connection is in waiting */
        if (this->fds[0].revents & POLLIN)
        {
            Debug::debug_message("New connection detected!");
            new_socket = this->acceptNewConnection();
            if (new_socket == -1)
            {
                Debug::error_message("Error in accept()!");
                continue ;
            }
            this->addNewClient(new_socket);
            Debug::debug_message("New connection accepted. Client socket: " + Debug::intToString(new_socket));
            this->handleNewConnection(*(this->clientsList.find(new_socket))->second);
        }

        /* Browse existing clients sockets */
        for (std::size_t i = 1; i < this->fds.size(); ++i)
        {
            if (this->fds[i].revents & POLLIN) /* Check if a new message is in waiting */
            {
                client_socket = this->fds[i].fd;
                Debug::debug_message("Incoming event on client socket: " + Debug::intToString(client_socket));
                if (this->handleEvent(client_socket) == 0)
                {
                    Debug::debug_message("Client disconnected on event: " + Debug::intToString(client_socket));
                    this->handleDisconnection(client_socket);
                
                    /* Remove from fds */
                    this->fds.erase(this->fds.begin() + i);

                    i--; /* Client disconnected */
                }
            }
            else if (this->fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) /* Check for logout */
            {
                client_socket = this->fds[i].fd;
                Debug::debug_message("Client disconnected on disconnection handling: " + Debug::intToString(client_socket));
                this->handleDisconnection(client_socket);
                
                /* Remove from fds */
                this->fds.erase(this->fds.begin() + i);

                i--; /* Client disconnected */
            }
        }
    } while (!Debug::stop(1));
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
        if (ret < 0)
        {
            Debug::error_message("Error in recv() on client socket: " + Debug::intToString(client_socket));
            return (ret);
        }
        return (ret);
    }

    /* Processing of data received from the client */
    buffer[ret] = '\0'; // null terminate the string received
	msg = msg + buffer;
    Debug::debug_message(Debug::intToString(client_socket) + " send a message: " + msg);
    return (ret);
}

/* Logout */
void Server::handleDisconnection(int client_socket)
{
    /* Manage logout */
    std::map<int, Client*>::iterator    it = this->clientsList.find(client_socket);

    if (this->clientsList.size() == 0)
        Debug::debug_message("No clients connected to the server");
    else
    {
        /* Clean clientsList map */
        close(it->first);
        delete it->second;
        this->clientsList.erase(it);
        
        Debug::debug_message("Logout done");
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
        Debug::error_message("Failed to create server socket options");
        return (-1);
    }
	fcntl(this->serverSocket, F_SETFL, O_NONBLOCK);
    if (this->serverSocket == -1)
    {
        Debug::error_message("Failed to create server socket");
        return (-2);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    /* Bind data to the server socket */
    if (bind(this->serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        close(this->serverSocket);
        Debug::error_message("Failed to bind server socket on port: " + Debug::intToString(port));
        return (-3);
    }

    /* Put the server socket on listen */
    if (listen(this->serverSocket, SOMAXCONN) < 0)
    {
        close(this->serverSocket);
        Debug::error_message("Failed to listen on server socket");
        return (-4);
    }
    Debug::debug_message("Server socket created and listening on port");
    return (0);
}