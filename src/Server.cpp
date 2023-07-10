/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:42:57 by root              #+#    #+#             */
/*   Updated: 2023/07/10 17:19:38 by root             ###   ########.fr       */
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
    return (0);
}

/* Utils */
int Server::createServerSocket(int port)
{
    struct sockaddr_in  addr;

    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket == -1)
    {
        std::cout << "Failed to create server socket" << std::endl;
        return (-1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(this->serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        close(this->serverSocket);
        std::cout << "Failed to bind server socket" << std::endl;
        return (-2);
    }
    if (listen(this->serverSocket, SOMAXCONN) < 0)
    {
        close(this->serverSocket);
        std::cout << "Failed to listen on server socket" << std::endl;
        return (-3);
    }
    std::cout << "Server socket created and listening on port" << std::endl;
    return (0);
}