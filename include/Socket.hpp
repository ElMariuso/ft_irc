/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 23:59:23 by root              #+#    #+#             */
/*   Updated: 2023/06/30 00:14:48 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP
# include "library.hpp"

class Socket
{
    public:
        Socket();
        ~Socket();

        int createSocket(int port);

    private:
        int                 fd;
        struct sockaddr_in  addr;
}

#endif