/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:42:57 by root              #+#    #+#             */
/*   Updated: 2023/06/30 13:00:17 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(std::string port_str, std::string password)
{
    int port;

    port = this->createPort(port_str);
    this->serverSocket = this->createServerSocket(port);
    if (this->serverSocket < 0)
        throw (std::runtime_error("Problem during creating server socket!"));
    this->password = password;
    this->processServer();
}
Server::~Server() {}

/* Operators */
bool Server::operator==(const Server& rhs) const
{
    if (this->password != rhs.password)
        return (false);
    return (true);
}

Server& Server::operator=(const Server& rhs)
{
    if (rhs == *this)
        return (*this);
    return (*this);
}

/* Functions */
int Server::processServer()
{
    int             ready;
    int             new_socket;
    int             client_socket;
    sockaddr_in     clientAddr;
    socklen_t       clientAddrLen;
    struct pollfd   serverPfd;

    serverPfd.fd = this->serverSocket;
    serverPfd.events = POLLIN;
    this->fds.push_back(serverPfd);
    do
    {
        ready = poll(this->fds.data(), this->fds.size(), -1);
        if (ready == -1)
            return (-1);
        if (this->fds[0].revents & POLLIN)
        {
            struct pollfd   clientPfd;
            
            clientAddrLen = sizeof(clientAddr);
            new_socket = accept(this->serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
            this->clientsList[new_socket] = new Client(); // new client
            clientPfd.fd = new_socket;
            clientPfd.events = POLLIN;
            this->fds.push_back(clientPfd);
        }

        std::map<int, Client*>::iterator it = this->clientsList.begin();
        while (it != this->clientsList.end()) /* Logout */
        {
            client_socket = it->first;
            if (this->fds[client_socket].revents & POLLHUP)
            {
                this->fds.erase(this->fds.begin() + client_socket);
                this->clientsList.erase(it++);
            }
            ++it;
        }
    }
    while (true);
    return (0);
}

/* Utils */
int Server::createPort(std::string port_str)
{
    int port;
    
    return (port);
}

int Server::createServerSocket(int port)
{
    struct sockaddr_in  addr;

    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket == -1)
        return (-1);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(this->serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        close(this->serverSocket);
        return (-2);
    }
    if (listen(this->serverSocket, SOMAXCONN) < 0)
    {
        close(this->serverSocket);
        return (-3);
    }
    return (0);
}