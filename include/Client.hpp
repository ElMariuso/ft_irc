/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 22:25:55 by root              #+#    #+#             */
/*   Updated: 2023/07/03 11:27:24 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "library.hpp"

class Client
{
    public:
        Client();
		Client(int fd);
        ~Client();

		int			getFd();
		std::string	getNickname();
		std::string	getUsername();
        
    private:
        int         _fd;
        std::string _nickname;
        std::string _username;
};

#endif