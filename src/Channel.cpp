/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:22:37 by bvernimm          #+#    #+#             */
/*   Updated: 2023/07/05 10:29:03 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

/* mode function */
int	Channel::addMode(char mode)
{
	if (mode == 't' || mode == 'n' || mode == 's' || mode == 'i' || mode == 'k' || mode == 'p' || mode == 'l') // list of existing modes
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
	
void	Channel::rmMode(char mode)
{
	if (mode == 't' || mode == 'n' || mode == 's' || mode == 'i' || mode == 'k' || mode == 'p' || mode == 'l') // list of existing modes
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
		return (false)
	return (true);
}