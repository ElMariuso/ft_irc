/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 23:57:10 by root              #+#    #+#             */
/*   Updated: 2023/07/11 00:06:07 by root             ###   ########.fr       */
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