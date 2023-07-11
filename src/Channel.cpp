/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:22:37 by bvernimm          #+#    #+#             */
/*   Updated: 2023/07/12 00:31:21 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

/* mode function */
int	Channel::addMode(char mode)
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
	
int	Channel::rmMode(char mode)
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

bool Channel::hasMode(char mode)
{
	if (_modesList.find(mode) == std::string::npos)
		return (false);
	return (true);
}

int	Channel::addOp(Client client)
{
	if (isOp(client) == true)
		return (-1); // Error, client is already an operator in this channel
	_operators.push_back(client.getFd());
	return (0);
}

int	Channel::rmOp(Client client)
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

bool Channel::isOp(Client client)
{
	std::vector<int>::iterator it;

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

void	Channel::setTopic(std::string topic)
{
	_topic = topic;
}

/* Getters */
std::string Channel::getName() { return (this->_name); }
std::map<int, Client*> Channel::getConnected() { return (this->_connected); }
std::vector<int> Channel::getOperators() { return (this->_operators); }
std::string Channel::getModesList() { return (this->_modesList); }
std::string Channel::getTopic() { return (this->_topic); }