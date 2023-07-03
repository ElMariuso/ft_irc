/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   library.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:22:05 by root              #+#    #+#             */
/*   Updated: 2023/07/03 13:40:31 by bvernimm         ###   ########.fr       */
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
# include <sstream>

# include <vector>
# include <map>
# include <string>

# include "Channel.hpp"
# include "Client.hpp"
# include "Server.hpp"

#endif