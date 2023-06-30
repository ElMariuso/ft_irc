/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:42:57 by root              #+#    #+#             */
/*   Updated: 2023/06/30 00:33:44 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(std::string port_str, std::string password)
{
    (void)port_str;
    (void)password;
    int port;

    this->socketServer = this->createSocket(port);
    this->processServer();
}
Server::~Server() {}

/* Operators */
bool Server::operator==(const Server& rhs)
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
    struct pollfd   serverPfd;
    struct pollfd   clientPfd:

    serverPfd.fd = this->fd;
    serverPfd.events = POLLIN;
    fds.push_back(serverPfd);
    do
    {
        ready = poll(fds.data(), fds.size(), -1);
        if (ready == -1)
            return (-1);
        if (fds[0].revents & POLLIN)
        {
            new_socket = accept(this->fd)
            this->clientsList[new_socket] = Client(); // new client
            clientPfd.fd = new_socket;
            clientPfd.events = POLLIN;
            fds.push_back(clientPfd);
        }
    }
    while (true);
    return (0);
}