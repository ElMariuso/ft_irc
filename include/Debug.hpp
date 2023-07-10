/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 23:55:29 by root              #+#    #+#             */
/*   Updated: 2023/07/11 00:01:10 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_HPP
# define DEBUG_HPP
# include "library.hpp"

class Debug
{
    public:
        static void debug_message(std::string message);
        static void error_message(std::string message);
};

#endif