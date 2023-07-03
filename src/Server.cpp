/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:42:57 by root              #+#    #+#             */
/*   Updated: 2023/07/04 00:19:51 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(std::string port_str, std::string password)
{
    int ret;

    this->serverSocket = this->createServerSocket(std::atoi(port_str.c_str()));
    if (this->serverSocket < 0)
        throw (std::runtime_error("Problem during creating server socket!"));
    this->password = password;
    ret = this->processServer();
    if (ret < 0)
        throw (std::runtime_error("Problem during running the server!"));
}
Server::~Server() {}

/* Functions */
int Server::processServer()
{
    int             ret;
    int             ready;
    struct pollfd   serverPfd;

    serverPfd.fd = this->serverSocket;
    serverPfd.events = POLLIN;
    this->fds.push_back(serverPfd);
    do
    {
        std::cout << "Waiting for poll()..." << std::endl;
        ready = poll(this->fds.data(), this->fds.size(), -1);
        if (ready < 0)
            return (-1);
        std::cout << "Login..." << std::endl;
        ret = this->manageNewConnection();
        if (ret < 0)
            return (-2);
        // this->receiveMessage();
        this->manageLogout();
    }
    while (true);
    return (0);
}

int Server::manageNewConnection()
{
    struct sockaddr_in  clientAddr;
    socklen_t           clientAddrLen;
    int                 new_socket;
    struct pollfd       clientPfd;

    if (this->fds[0].revents & POLLIN)
    {
        /* New connection */
        clientAddrLen = sizeof(clientAddr);
        new_socket = accept(this->serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        std::cout << "Establishing new connection: " << new_socket << std::endl;
        if (new_socket >= 0)
        {
            clientPfd.fd = new_socket;
            clientPfd.events = POLLIN;
            this->fds.push_back(clientPfd);
            this->clientsList[new_socket] = new Client();
        }
        else
            return (-1);
    }
    return (0);
}

int Server::receiveMessage()
{
    std::size_t index;
    int         client_socket;
    char        buffer[1024];
    int         bytes_read;

    index = 1;
    while (index < this->fds.size())
    {
        if (this->fds[index].revents & POLLIN)
        {
            client_socket = this->fds[index].fd;
            bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_read <= 0)
            {
                // Error
            }
            else
            {
                
            }
        }
    }
    return (0);
}

int Server::manageLogout()
{
    std::map<int, Client*>::iterator    it;
    int                                 client_socket;
    
    it = this->clientsList.begin();
    while (it != this->clientsList.end()) /* Logout */
    {
        client_socket = it->first;
        if (this->fds[client_socket].revents & POLLHUP)
        {
            std::cout << "Logout: " << client_socket << std::endl;
            this->fds.erase(this->fds.begin() + client_socket);
            this->clientsList.erase(it++);
        }
        ++it;
    }
    return (0);
}

/* Utils */
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