/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:22:37 by bvernimm          #+#    #+#             */
/*   Updated: 2023/07/20 06:24:39 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

Channel::Channel(const std::string &name)
{
	this->setName(name);
	this->setTopic("");
	this->setPassword("");
	this->setLimit(0);
	this->setHasInvitedList(false);
}
Channel::~Channel() {}

/* mode function */
int	Channel::addMode(const char &mode)
{
	if (mode == 't' || mode == 'i' || mode == 'k' || mode == 'l') // list of existing modes
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
	
int	Channel::rmMode(const char &mode)
{
	if (mode == 't' || mode == 'i' || mode == 'k' || mode == 'l') // list of existing modes
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

bool Channel::hasMode(const char &mode) const
{
	if (_modesList.find(mode) == std::string::npos)
		return (false);
	return (true);
}

int	Channel::addOp(const Client &client)
{
	if (isOp(client) == true)
		return (-1); // Error, client is already an operator in this channel
	_operators.push_back(client.getFd());
	return (0);
}

int	Channel::rmOp(const Client &client)
{
	std::vector<int>::iterator it;

	it = this->_operators.begin();
	while (it != this->_operators.end())
	{
		if (*it == client.getFd())
			break ;
		it++;
	}
	if (it == _operators.end())
		return (-1); // Error, client is not an operator in this channel
	_operators.erase(it);
	return (0);
}

bool Channel::isOp(const Client &client) const
{
	std::vector<int>::const_iterator	it;

	it = this->_operators.begin();
	while (it != this->_operators.end())
	{
		if (*it == client.getFd())
			break ;
		it++;
	}
	if (it != _operators.end())
		return (true);
	return (false);
}

bool Channel::hasTopic() const
{
	if (this->_topic.empty())
		return (false);
	return (true);
}

bool Channel::hasPassword() const
{
	if (this->password.empty())
		return (false);
	return (true);
}

bool Channel::hasLimit() const
{
	if (this->limit <= 0)
		return (false);
	return (true);
}

/* Setters */
void Channel::setName(const std::string &name) { this->_name = name; }
void Channel::setConnected(Client *client) { this->_connected.insert(std::make_pair(client->getFd(), client)); }
void Channel::setConnectedList(std::map<int, Client*> connected) { this->_connected = connected; }
void Channel::setOperator(int fd) { this->_operators.push_back(fd); }
void Channel::setOperators(std::vector<int> operators) { this->_operators = operators; }
void Channel::setModesList(const std::string &modesList) { this->_modesList = modesList; }
void Channel::setTopic(const std::string &topic) { this->_topic = topic; }
void Channel::setPassword(const std::string &password) { this->password = password; }
void Channel::setLimit(std::size_t limit)  { this->limit = limit; }
void Channel::setHasInvitedList(bool hasInvitedList) { this->hasInvitedList = hasInvitedList; }
void Channel::setInvited(const std::string &name) { this->invited.insert(std::make_pair(name, true)); }
void Channel::setInvitedList(std::map<std::string, bool> &invited) { this->invited = invited; }

/* Removers */
void Channel::removeConnected(const int &fd) { this->_connected.erase(fd); }

/* Getters */
std::string Channel::getName() const { return (this->_name); }
std::map<int, Client*> Channel::getConnected() const { return (this->_connected); }
std::vector<int> Channel::getOperators() const { return (this->_operators); }
std::string Channel::getModesList() const { return (this->_modesList); }
std::string Channel::getTopic() const { return (this->_topic); }
std::string	Channel::getPassword() const { return (this->password); }
std::size_t Channel::getLimit() const  { return (this->limit); }
bool Channel::getHasInvitedList() const { return (this->hasInvitedList); }
std::map<std::string, bool>	Channel::getInvited() const { return (this->invited); }

/* Finders */
Client* Channel::findConnected(const int &fd)
{
	std::map<int, Client*>::const_iterator it = this->_connected.find(fd);
	
	if (it != this->_connected.end())
        return (it->second);
    return (NULL);
}

std::map<int, Client*>::const_iterator Channel::findConnectedByName(const std::string &name) const
{
    for (std::map<int, Client*>::const_iterator it = this->_connected.begin(); it != this->_connected.end(); ++it)
    {
        if (it->second->getNickname() == name)
            return (it);
    }
    return (this->_connected.end());
}

/* Senders */
void Channel::sendToAll(const std::string &message, const std::string &srcName, bool sendToSRC)
{
	const std::map<int, Client*>	&connected = this->_connected;

	for (std::map<int, Client*>::const_iterator it = connected.begin(); it != connected.end(); ++it)
	{
		const Client &client = *(it->second);

		if (client.getNickname() == srcName && sendToSRC == true)
			client.sendToFD(message);
		else if (client.getNickname() != srcName)
			client.sendToFD(message);
	}
}