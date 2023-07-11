/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   library.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:22:05 by root              #+#    #+#             */
/*   Updated: 2023/07/11 14:06:06 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBRARY_HPP
# define LIBRARY_HPP

/* Color ******************************************************************** */

# define RESET      "\033[0m"
# define BLACK      "\033[30m"      
# define RED        "\033[31m"     
# define GREEN      "\033[32m"      
# define YELLOW     "\033[33m"      
# define BLUE       "\033[34m"      
# define MAGENTA    "\033[35m"      
# define CYAN       "\033[36m"      
# define WHITE      "\033[37m"
# define PINK       "\033[38;5;206m"

/* Define ******************************************************************* */

# define BUFFER_SIZE 1024

/* Includes ***************************************************************** */

# include <poll.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <fcntl.h>

# include <iostream>
# include <stdexcept>
# include <cstdlib>
# include <sstream>
# include <csignal>

# include <vector>
# include <map>
# include <string>

/* Includes.h *************************************************************** */

# include "Channel.hpp"
# include "Client.hpp"
# include "Debug.hpp"
# include "Message.hpp"
# include "Server.hpp"

#endif