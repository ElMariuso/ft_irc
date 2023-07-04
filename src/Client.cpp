/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 13:27:06 by root              #+#    #+#             */
/*   Updated: 2023/07/04 11:13:14 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {
}

Client::Client(int fd) : _fd(fd), _nickname("DefaultNickname"), _invisible(false), _wallops(false), _operator(false), _serverNotice(false), _hiddenHost(false), _away(false), _restricted(false), _quiet(false), _bot(false), _deaf(false), _callerID(false)
{
	std::stringstream ss;
	
	ss << fd; // turn int to std::string
	_username = "DefaultUsername" + ss.str() // put the file descriptor in client's default username to help identify him
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
	
	error = "recv command failed for client : " + _username;
	if (ret <= -1)
		throw (error); // if recv function failed, throw an error
		
	buffer[ret] = '\0'; // null terminate the string received
	msg = msg + buffer; // turn the string fron "char*" to "std::string"
	return (msg);
}

/* setters */
void Client::setNickname(std::string nickName) { this->_nickname = nickName; }
void Client::setUsername(std::string userName) { this->_username = userName; }
void Client::setInvisible(bool mode) { this->_invisible = mode; }
void Client::setWallops(bool mode) { this->_wallops = mode; }
void Client::setOperator(bool mode) { this->_operator = mode; }
void Client::setServerNotice(bool mode) { this->_serverNotice = mode; }
void Client::setHiddenHost(bool mode) { this->_hiddenHost = mode; }
void Client::setAway(bool mode) { this->_away = mode; }
void Client::setRestricted(bool mode) { this->_restricted = mode; }
void Client::setQuiet(bool mode) { this->_quiet = mode; }
void Client::setBot(bool mode) { this->_bot = mode; }
void Client::setDeaf(bool mode) { this->_deaf = mode; }
void Client::setCallerID(bool mode) { this->_callerID = mode; }

/* getters */
int			Client::getFd() { return (this->_fd); }
std::string	Client::getNickname() { return (this->_nickname); }
std::string	Client::getUsername() { return (this->_username); }
bool		Client::isInvisible() { return (this->_invisible); }
bool		Client::isWallops() { return (this->_wallops); }
bool		Client::isOperator() { return (this->_operator); }
bool		Client::isServerNotice() { return (this->_serverNotice); }
bool		Client::isHiddenHost() { return (this->_hiddenHost); }
bool		Client::isAway() { return (this->_away); }
bool		Client::isRestricted() { return (this->_restricted); }
bool		Client::isQuiet() { return (this->_quiet); }
bool		Client::isBot() { return (this->_bot); }
bool		Client::isDeaf() { return (this->_deaf); }
bool		Client::isCallerID() { return (this->_callerID); }