/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 13:27:06 by root              #+#    #+#             */
/*   Updated: 2023/07/20 05:00:13 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client() {}

Client::Client(const std::string &nickname, const std::string &username, const std::string &realname, int fd, bool isConnected);
{	
	/* Arguments */
	this->setNickname(nickname);
	this->setUsername(username);
	this->setRealname(realname);
	this->setFd(fd);
	this->setIsConnected(isConnected);
	this->setIsAuthenticated(false);
	this->_hostname = "127.0.0.1";
	this->initHostname();
	this->setTimeSinceLastPing();
	this->setLastPingIdentifier("-1");
}

Client::~Client()
{
	close(_fd);
}

/* function */
void Client::sendToFD(const std::string &str) const
{
	send(_fd, str.c_str(), str.size(), 0); // write "str" on "_fd"
	Utils::debug_message(Utils::intToString(this->getFd()) + " get a message " + str);
}

std::string	Client::receiveFromFD()
{
	ssize_t		ret;
	char		buffer[BUFFER_SIZE + 1];
	std::string	msg;
	std::string	error;

	msg.clear(); // make sur "msg" start of empty
	ret = recv(_fd, buffer, BUFFER_SIZE, 0); // fill "buffer" witch a string written on "_fd", maximum "BUFFER_SIZE" character, return the string's lenght or -1 when there is an error
	
	error = "recv command failed for client : " + _username;
	if (ret <= -1)
		throw (error); // if recv function failed, throw an error
		
	buffer[ret] = '\0'; // null terminate the string received
	msg = msg + buffer; // turn the string fron "char*" to "std::string"
	return (msg);
}

void Client::initHostname()
{
	sockaddr_storage	addr;
    socklen_t 			len;
    char 				clientHost[NI_MAXHOST];
    char 				*clientIP;
	struct sockaddr_in	*s;
	struct hostent		*hostEntry;

	len = sizeof(addr);
    if (getsockname(this->_fd, (struct sockaddr*)&addr, &len) == -1)
	{
		Utils::error_message("Error getting socket information");
		return ;
    }
	if (addr.ss_family == AF_INET)
	{
		s = (struct sockaddr_in*)&addr;
		clientIP = inet_ntoa(s->sin_addr);
	}
	else if (addr.ss_family == AF_INET6)
	{
		Utils::error_message("IPv6 address not supported");
		return ;
    }
	else
	{
		Utils::error_message("Unsupported address family");
		return ;
	}
	hostEntry = gethostbyname(clientIP);
	if (hostEntry == NULL)
	{
		Utils::error_message("Error during reverse resolution");
		return ;
	}
	Utils::ft_strncpy(clientHost, hostEntry->h_name, NI_MAXHOST);
	this->_hostname = clientHost;
}

/* mode function */
int	Client::addMode(char mode)
{
	if (mode == 'i' || mode == 'w' || mode == 'o' || mode == 's' || mode == 'x' || mode == 'a' || mode == 'r' || mode == 'q' || mode == 'b' || mode == 'D' || mode == 'G') // list of existing modes
	{
		if (_modesList.find(mode) == std::string::npos)
			_modesList = _modesList + mode;
		else
			return (1); // error, mode is already activated
	}
	else
		return (-1); // error, mode invalid
	return (0);
}
	
int	Client::rmMode(char mode)
{
	if (mode == 'i' || mode == 'w' || mode == 'o' || mode == 's' || mode == 'x' || mode == 'a' || mode == 'r' || mode == 'q' || mode == 'b' || mode == 'D' || mode == 'G') // list of existing modes
	{
		if (_modesList.find(mode) != std::string::npos)
			_modesList.erase(_modesList.find(mode), 1);
		else
			return (1); // error, mode is not activated
	}
	else
		return (-1); // error, mode invalid
	return (0);
}

bool Client::hasMode(std::string mode)
{
	if (mode == "invisible")
		return (hasModeLetter('i'));
	if (mode == "wallops")
		return (hasModeLetter('w'));
	if (mode == "operator")
		return (hasModeLetter('o'));
	if (mode == "serverNotice")
		return (hasModeLetter('s'));
	if (mode == "hiddenHost")
		return (hasModeLetter('x'));
	if (mode == "away")
		return (hasModeLetter('a'));
	if (mode == "restricted")
		return (hasModeLetter('r'));
	if (mode == "quiet")
		return (hasModeLetter('q'));
	if (mode == "bot")
		return (hasModeLetter('b'));
	if (mode == "deaf")
		return (hasModeLetter('D'));
	if (mode == "callerID")
		return (hasModeLetter('G'));
	return (false);
}

bool Client::hasModeLetter(char mode)
{
	if (_modesList.find(mode) == std::string::npos)
		return (false);
	return (true);
}

/* Setters */
void Client::setFd(const int fd) { this->_fd = fd; }
void Client::setIsAuthenticated(bool isAuthenticated) { this->isAuthenticated = isAuthenticated; }
void Client::setIsConnected(bool isConnected) { this->isConnected = isConnected; }
void Client::setNickname(std::string nickName) { this->_nickname = nickName; }
void Client::setUsername(std::string userName) { this->_username = userName; }
void Client::setHostname(std::string hostName) { this->_hostname = hostName; }
void Client::setRealname(std::string realName) { this->_realname = realName; }
void	Client::setTimeSinceLastPing() { this->_timeSinceLastPing = clock(); }
void	Client::setLastPingIdentifier(std::string identifier) { this->_LastPingIdentifier = identifier; }

/* getters */
int Client::getFd() const { return (this->_fd); }
bool Client::getIsAuthenticated() const { return (this->isAuthenticated); }
bool Client::getIsConnected() const { return (this->isConnected); }
std::string	Client::getNickname() const { return (this->_nickname); }
std::string	Client::getUsername() const { return (this->_username); }
std::string	Client::getHostname() const { return (this->_hostname); }
std::string	Client::getRealname() const { return (this->_realname); } 
float Client::getTimeSinceLastPing() const
{
	clock_t	t;

	t = clock();
	return ((float)t - (float)this->_timeSinceLastPing);
}
std::string	Client::getLastPingIdentifier() const { return (this->_LastPingIdentifier); }
