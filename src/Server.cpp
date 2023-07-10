/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:42:57 by root              #+#    #+#             */
/*   Updated: 2023/07/10 18:32:23 by root             ###   ########.fr       */
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
    struct pollfd   serverPfd;
    int             ready;
    int             new_socket;
    int             client_socket;

    /* Init fds vector */
    serverPfd.fd = this->serverSocket;
    serverPfd.events = POLLIN;
    this->fds.push_back(serverPfd);
    /* Main loop */
    while (true)
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
            // new_socket = this->acceptNewConnection();
            if (new_socket == -1)
            {
                std::cout << "[DEBUG] - Error in accept()!" << std::endl;
                continue ;
            }
            std::cout << "[DEBUG] - New connection accepted. Client socket: " << new_socket << std::endl;
            // this->handleNewConnection(new_socket);
        }

        /* Browse existing clients sockets */
        for (std::size_t i = 1; i < this->fds.size(); ++i)
        {
            if (this->fds[i].revents & POLLIN) /* Check if a new message is in waiting */
            {
                client_socket = this->fds[i].fd;
                std::cout << "[DEBUG] - Incoming message event on client socket: " << client_socket << std::endl;
                // this->handleEvent(client_socket);
            }
            else if (this->fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) /* Check for logout */
            {
                client_socket = this->fds[i].fd;
                std::cout << "[DEBUG] - Disconnection event on client socket: " << client_socket << std::endl;
                // this->handleDisconnection(client_socket);
                i--; /* Client disconnected */
            }
        }
    }
    return (0);
}

/* Utils */
int Server::createServerSocket(int port)
{
    struct sockaddr_in  addr;

    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket == -1)
    {
        std::cout << "[DEBUG] - Failed to create server socket" << std::endl;
        return (-1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(this->serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        close(this->serverSocket);
        std::cout << "[DEBUG] - Failed to bind server socket" << std::endl;
        return (-2);
    }
    if (listen(this->serverSocket, SOMAXCONN) < 0)
    {
        close(this->serverSocket);
        std::cout << "[DEBUG] - Failed to listen on server socket" << std::endl;
        return (-3);
    }
    std::cout << "[DEBUG] - Server socket created and listening on port" << std::endl;
    return (0);
}