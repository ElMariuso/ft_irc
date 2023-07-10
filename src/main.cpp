/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 20:43:52 by root              #+#    #+#             */
/*   Updated: 2023/07/10 22:25:17 by root             ###   ########.fr       */
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
        std::cerr << "[ERROR] : " << e.what() << std::endl;
        return (1);
    }
    std::cout << "Leaving ft_irc..." << std::endl;
    return (0);
}