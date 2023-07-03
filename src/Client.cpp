/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 13:27:06 by root              #+#    #+#             */
/*   Updated: 2023/07/03 11:40:19 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {
}

Client::Client(int fd) : _fd(fd)
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

Client::~Client()
{
	close(_fd);
}


/* getters */

int			Client::getFd() { return (this->_fd); }
std::string	Client::getNickname() { return (this->_nickname); }
std::string	Client::getUsername() { return (this->_username); }