/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:22:37 by bvernimm          #+#    #+#             */
/*   Updated: 2023/07/11 11:59:38 by bvernimm         ###   ########.fr       */
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

	it = find(_operators.begin(), _operators.end(), client.getFd());
	if (it == _operators.end())
		return (-1); // Error, client is not an operator in this channel
	_operators.erase(it);
	return (0);
}

bool Channel::isOp(Client client)
{
	std::vector<int>::iterator it;

	it = find(_operators.begin(), _operators.end(), client.getFd());
	if (it != _operators.end())
		return (true);
	return (false);
}