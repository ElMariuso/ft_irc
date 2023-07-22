/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 23:57:10 by root              #+#    #+#             */
/*   Updated: 2023/07/12 22:48:49 by mthiry           ###   ########.fr       */
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

char*	Utils::ft_strncpy(char* dest, const char* src, size_t n)
{
    size_t	i;
	
    for (i = 0; i < n && src[i] != '\0'; ++i)
        dest[i] = src[i];
    for (; i < n; ++i)
        dest[i] = '\0';
    return (dest);
}

std::string Utils::strCopyWithoutFirst(std::string str)
{
	std::string tmp;

	for (int i = 0; i + 1 != str.size(); i++)
		tmp[0] = str[i + 1];
	return (tmp);
}
