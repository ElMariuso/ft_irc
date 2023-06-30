/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 00:01:13 by root              #+#    #+#             */
/*   Updated: 2023/06/30 00:16:21 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket() {}
Socket::~Socket() {}

int Socket::createSocket(int port)
{
    this->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
        return (-1);
    this->addr.sin_family = AF_INET;
    this->addr.sin_port = htons(port);
    this->addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(this->fd, (struct sockaddr*)&this->addr, sizeof(this->addr)) < 0)
    {
        close(this->fd);
        return (-2);
    }
    if (listen(this->fd, SOMAXCONN) < 0)
    {
        close(this->fd);
        return (-3);
    }
}