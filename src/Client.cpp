/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 13:27:06 by root              #+#    #+#             */
/*   Updated: 2023/07/03 12:46:03 by bvernimm         ###   ########.fr       */
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

/* function */

void Client::sendToClient(std::string str)
{
	send(_fd, str.c_str(), str.size(), 0);
}

std::string	Client::receiveFromClient()
{
	ssize_t		ret;
	char		buffer[BUFFER_SIZE + 1];
	std::string	msg;
	std::string	error;

	msg.clear();//is it needed ?
	ret = recv(_fd, buffer, BUFFER_SIZE, 0)
	error = "recv command failed for client with fd : " + _fd;
	if (ret <= -1)
		throw (error);
	buffer[ret] = '\0';
	msg = msg + buffer;
	return (msg);
}

/* setters */
void Client::setNickname(std::string nickName) { this->_nickname = nickName; }
void Client::setUsername(std::string userName) { this->_username = userName; }

/* getters */

int			Client::getFd() { return (this->_fd); }
std::string	Client::getNickname() { return (this->_nickname); }
std::string	Client::getUsername() { return (this->_username); }