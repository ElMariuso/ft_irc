/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 23:57:10 by root              #+#    #+#             */
/*   Updated: 2023/07/11 14:18:41 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Debug.hpp"

void Debug::debug_message(std::string message)
{
    std::cout << BLUE << "[DEBUG]" << RESET << " - " << message << std::endl;
}

void Debug::error_message(std::string message)
{
    std::cerr << RED << "[ERROR]" << RESET << " - " << message << std::endl;
}

std::string	Debug::intToString(int nb)
{
	std::stringstream ss;
	
	ss << nb;
	return (ss.str());
}

bool Debug::stop(int indicator)
{
	static bool	stop;
	
	if (indicator == 0)
		stop = false;
	if (indicator == 2)
		stop = true;
	return (stop);
}