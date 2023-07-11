/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 20:43:52 by root              #+#    #+#             */
/*   Updated: 2023/07/11 16:03:22 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/library.hpp"

void crtlcHandler(int signal)
{
	(void) signal;
	Utils::stop(2);
}

int main(int argc, char **argv)
{
    try
    {
        if (argc < 3)
            throw (std::runtime_error("Not enough arguments!"));
        else if (argc > 3)
            throw (std::runtime_error("Too many arguments!"));
        
		Utils::stop(0);
		signal(SIGINT, crtlcHandler);
        Server  ircserv(argv[1], argv[2], "ft_irc");
    }
    catch (const std::exception& e)
    {
        Utils::error_message(e.what());
        return (1);
    }
    Utils::debug_message("Leaving...");
    return (0);
}