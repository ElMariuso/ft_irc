/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 23:57:10 by root              #+#    #+#             */
/*   Updated: 2023/08/10 20:25:13 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Utils.hpp"

std::string Utils::getCurrentTime()
{
    std::time_t now = std::time(0);
    std::tm* localtm = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", localtm);
    return (std::string(buffer));
}

void Utils::debug_message(const std::string &message) { std::cout << BLUE << "[DEBUG - " << Utils::getCurrentTime() << "]" << RESET << " - " << message << std::endl; }
void Utils::error_message(const std::string &message) { std::cerr << RED << "[ERROR - " << Utils::getCurrentTime() << "]" << RESET << " - " << message << std::endl; }

void Utils::waiting_message(const std::string &message)
{
	static int			numDots = 1;
	const int			maxDots = 4;
	std::string 		dots = std::string(numDots, '.');
	
	std::cout << DELETING << GREEN << "[WAITING]" << RESET << " - " << message << dots << std::endl;;

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

std::string Utils::getDate()
{
    std::ostringstream  stream;
    std::time_t         currentTime = std::time(NULL);
    const char*         day[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    const char*         month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    std::tm *timeInfo = std::localtime(&currentTime);

    stream << day[timeInfo->tm_wday] << " " << month[timeInfo->tm_mon] << " " << timeInfo->tm_mday << " ";
    stream << std::setfill('0') << std::setw(2) << timeInfo->tm_hour << ":";
    stream << std::setfill('0') << std::setw(2) << timeInfo->tm_min << " " << 1900 + timeInfo->tm_year;
    return (stream.str());
}

std::string Utils::getDateUNIX()
{
    std::ostringstream  stream;
    std::time_t         currentTime = std::time(NULL);
    
    stream << currentTime;
    return (stream.str());
}

std::string Utils::displayBox(const std::string &message, int minWidth)
{
    std::ostringstream  stream;
    int                 messageLength = message.length();
    int                 boxWidth = std::max(minWidth, messageLength + 6);
    int                 padding = (boxWidth - messageLength - 2) / 2;

    std::string         horizontalLine = std::string(boxWidth, '*');
    std::string         paddingSpaces = std::string(padding, ' ');

    stream << horizontalLine << std::endl \
        << "*" << paddingSpaces << message << paddingSpaces;
    stream << "*" << std::endl \
        << horizontalLine;
    return (stream.str());
}

bool Utils::isInt(const std::string str)
{
    if (str.empty())
        return (false);
    if (str.length() != 1 && str[0] == '0')
        return (false);
    for (std::size_t i = 0; i != str.length(); ++i)
    {
        if (!std::isdigit(str[i]))
            return (false);
    }
    return (true);
}