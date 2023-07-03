/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 13:27:06 by root              #+#    #+#             */
/*   Updated: 2023/07/03 13:13:55 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {
}

Client::Client(int fd) : _fd(fd), _nickname("DefaultNickname"), _username("DefaultUsername")
{
	fcntl(fd, F_SETFL, O_NONBLOCK); // set the fd to non-blocking mode
}

Client::~Client()
{
	close(_fd);
}

/* function */
void Client::sendToClient(std::string str)
{
	send(_fd, str.c_str(), str.size(), 0); // write "str" on "_fd"
}

std::string	Client::receiveFromClient()
{
	ssize_t		ret;
	char		buffer[BUFFER_SIZE + 1];
	std::string	msg;
	std::string	error;

	msg.clear(); // make sur "msg" start of empty
	ret = recv(_fd, buffer, BUFFER_SIZE, 0) // fill "buffer" witch a string written on "_fd", maximum "BUFFER_SIZE" character, return the string's lenght or -1 when there is an error
	
	error = "recv command failed for client with fd : " + _fd;
	if (ret <= -1)
		throw (error); // if recv function failed, throw an error
		
	buffer[ret] = '\0'; // null terminate the string received
	msg = msg + buffer; // turn the string fron "char*" to "std::string"
	return (msg);
}

/* setters */
void Client::setNickname(std::string nickName) { this->_nickname = nickName; }
void Client::setUsername(std::string userName) { this->_username = userName; }

/* getters */
int			Client::getFd() { return (this->_fd); }
std::string	Client::getNickname() { return (this->_nickname); }
std::string	Client::getUsername() { return (this->_username); }