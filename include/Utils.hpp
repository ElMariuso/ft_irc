/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 23:55:29 by root              #+#    #+#             */
/*   Updated: 2023/07/23 13:23:26 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP
# include "library.hpp"

class Utils
{
    public:
        static void			debug_message(const std::string &message);
        static void			error_message(const std::string &message);
        static void         waiting_message(const std::string &message);
		static std::string	intToString(int nb); // Turn int nb into a string
		static bool			stop(int indicator); // Send 0 to initialise, 1 to get the return and 2 to stop the program
        static char*        ft_strncpy(char* dest, const char* src, size_t n);
        static std::string  getDate();
        static std::string  displayBox(const std::string &message, int minWidth);
};

#endif