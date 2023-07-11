/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 23:57:10 by root              #+#    #+#             */
/*   Updated: 2023/07/11 16:02:05 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Utils.hpp"

void Utils::debug_message(std::string message)
{
    std::cout << BLUE << "[DEBUG]" << RESET << " - " << message << std::endl;
}

void Utils::error_message(std::string message)
{
    std::cerr << RED << "[ERROR]" << RESET << " - " << message << std::endl;
}

std::string	Utils::intToString(int nb)
{
	std::stringstream ss;
	
	ss << nb;
	return (ss.str());
}

bool Utils::stop(int indicator)
{
	static bool	stop;
	
	if (indicator == 0)
		stop = false;
	if (indicator == 2)
		stop = true;
	return (stop);
}