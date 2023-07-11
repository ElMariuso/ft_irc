/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 20:43:52 by root              #+#    #+#             */
/*   Updated: 2023/07/11 00:07:05 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/library.hpp"

int main(int argc, char **argv)
{
    try
    {
        if (argc < 3)
            throw (std::runtime_error("Not enough arguments!"));
        else if (argc > 3)
            throw (std::runtime_error("Too many arguments!"));
        
        Server  ircserv(argv[1], argv[2], "ft_irc");
    }
    catch (const std::exception& e)
    {
        Debug::error_message(e.what());
        return (1);
    }
    Debug::debug_message("Leaving...");
    return (0);
}