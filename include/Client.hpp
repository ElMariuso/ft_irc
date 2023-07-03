/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 22:25:55 by root              #+#    #+#             */
/*   Updated: 2023/07/03 12:44:59 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "library.hpp"
# define BUFFER_SIZE 1024

class Client
{
    public:
        Client();
		Client(int fd);
        ~Client();

		/* function */
		void		sendToClient(std::string str);
		std::string	receiveFromClient();

		/* setters */
		void		setNickname(std::string nickName);
		void		setUsername(std::string userName);
		
		/* getters */
		int			getFd();
		std::string	getNickname();
		std::string	getUsername();
        
    private:
        int         _fd;
        std::string _nickname;
        std::string _username;
};

#endif