/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   library.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:22:05 by root              #+#    #+#             */
/*   Updated: 2023/07/03 16:56:18 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBRARY_HPP
# define LIBRARY_HPP

/* Includes ***************************************************************** */

# include <poll.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>

# include <iostream>
# include <stdexcept>
# include <cstdlib>

# include <vector>
# include <map>

/* Includes.h *************************************************************** */

# include "Channel.hpp"
# include "Client.hpp"
# include "Message.hpp"
# include "Server.hpp"

#endif