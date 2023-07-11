/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 23:55:29 by root              #+#    #+#             */
/*   Updated: 2023/07/11 14:11:22 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_HPP
# define DEBUG_HPP
# include "library.hpp"

class Debug
{
    public:
        static void			debug_message(std::string message);
        static void			error_message(std::string message);
		static std::string	intToString(int nb); // Turn int nb into a string
		static bool			stop(int indicator); // Send 0 to initialise, 1 to get the return and 2 to stop the program
};

#endif