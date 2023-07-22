/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 23:57:10 by root              #+#    #+#             */
/*   Updated: 2023/07/22 19:44:20 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Utils.hpp"

void Utils::debug_message(const std::string &message) { std::cout << BLUE << "[DEBUG]" << RESET << " - " << message << std::endl; }
void Utils::error_message(const std::string &message) { std::cerr << RED << "[ERROR]" << RESET << " - " << message << std::endl; }

void Utils::waiting_message(const std::string &message)
{
	static int			numDots = 1;
	const int			maxDots = 4;
	std::string 		dots = std::string(numDots, '.');
	
	std::cout << "\x1b[1F\x1b[2K\r" << GREEN << "[WAITING]" << RESET << " - " << message << dots << std::flush << std::endl;;

	numDots = (numDots + 1) % maxDots;
	if (numDots == 0)
		numDots = 1;

	std::cout.flush();
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

char*	Utils::ft_strncpy(char* dest, const char* src, size_t n)
{
    size_t	i;
	
    for (i = 0; i < n && src[i] != '\0'; ++i)
        dest[i] = src[i];
    for (; i < n; ++i)
        dest[i] = '\0';
    return (dest);
}