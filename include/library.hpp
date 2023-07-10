/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   library.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:22:05 by root              #+#    #+#             */
/*   Updated: 2023/07/10 20:17:11 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBRARY_HPP
# define LIBRARY_HPP

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

# include <vector>
# include <map>
# include <string>

/* Includes.h *************************************************************** */

# include "Channel.hpp"
# include "Client.hpp"
# include "Message.hpp"
# include "Server.hpp"

#endif