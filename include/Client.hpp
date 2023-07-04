/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 22:25:55 by root              #+#    #+#             */
/*   Updated: 2023/07/04 11:10:11 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "library.hpp"
# define BUFFER_SIZE 1024

class Client
{
    private:
		Client(); // default constructor should never be called
    public:
		Client(int fd);
        ~Client();

		/* function */
		void		sendToClient(std::string str); // write a string on the client's fd
		std::string	receiveFromClient(); // receive and return a string written by the client

		/* setters */
		void		setNickname(std::string nickName);
		void		setUsername(std::string userName);
		void		setInvisible(bool mode);
		void		setWallops(bool mode);
		void		setOperator(bool mode);
		void		setServerNotice(bool mode);
		void		setHiddenHost(bool mode);
		void		setAway(bool mode);
		void		setRestricted(bool mode);
		void		setQuiet(bool mode);
		void		setBot(bool mode);
		void		setDeaf(bool mode);
		void		setCallerID(bool mode);
		
		/* getters */
		int			getFd();
		std::string	getNickname();
		std::string	getUsername();
		bool		isInvisible();
		bool		isWallops();
		bool		isOperator();
		bool		isServerNotice();
		bool		isHiddenHost();
		bool		isAway();
		bool		isRestricted();
		bool		isQuiet();
		bool		isBot();
		bool		isDeaf();
		bool		isCallerID();
        
    private:
        int         _fd;
        std::string _nickname;
        std::string _username;

		/* user modes */  
		/* !!! We can choose to add/remove modes, those are just the more common one !!! */
		bool		_invisible; // hides the user from WHO and NAMES lists
		bool		_wallops; //  receives "wallops" messages, which are system-wide messages sent by operators
		bool		_operator; // grants operator privileges to the user
		bool		_serverNotice; // receives server notices
		bool		_hiddenHost; //  hides the user's host information
		bool		_away; //  indicates that the user is currently afk
		bool		_restricted; //  restricts private messages from unregistered or unidentified users
		bool		_quiet; //  prevents a user from receiving channel messages or private messages from users who are not on their access list or buddy list
		bool		_bot; //  marks the user as a bot
		bool		_deaf; //  disables the user's ability to receive channel messages, they user can still send messages but won't receive any messages from others
		bool		_callerID; //   enables the user to see the IP address or hostname of other users in private messages or channel user lists
};

#endif