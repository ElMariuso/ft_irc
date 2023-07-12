/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 22:25:55 by root              #+#    #+#             */
/*   Updated: 2023/07/12 17:14:11 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "library.hpp"

class Client
{
    private:
		Client(); // default constructor should never be called
    public:
		Client(int fd);
        ~Client();

		/* function */
		void		sendToFD(std::string str); // write a string on the client's fd
		std::string	receiveFromFD(); // receive and return a string written by the client

		/* mode function */
		int		addMode(char mode); // add an user mode, return 0 if it went well
		int		rmMode(char mode); // remove an user mode, return 0 if it went well
		bool	hasMode(std::string mode); // search if client has mode activated with the mode's name
		bool	hasModeLetter(char mode); // search if client has mode activated with the mode's letter

		/* setters */
		void		setNickname(std::string nickName);
		void		setUsername(std::string userName);
		void		setHostname();
		
		/* getters */
		int			getFd();
		std::string	getNickname();
		std::string	getUsername();
		std::string	getHostname();
        
    private:
        int         _fd;
        std::string _nickname;
        std::string _username;
		std::string	_hostname;
		std::string _modesList;
};

/* all user modes currently added : 
		
	invisible (i) : hides the user from WHO and NAMES lists
	wallops (w) : receives "wallops" messages, which are system-wide messages sent by operators
	operator (o) : grants operator privileges to the user
	serverNotice (s) : receives server notices
	hiddenHost (x) : hides the user's host information
	away (a) : indicates that the user is currently afk
	restricted (r) : restricts private messages from unregistered or unidentified users
	quiet (q) : prevents a user from receiving channel messages or private messages from users who are not on their access list or buddy list
	bot (b) : marks the user as a bot
	deaf (D) : disables the user's ability to receive channel messages, they user can still send messages but won't receive any messages from others
	callerID (G) : enables the user to see the IP address or hostname of other users in private messages or channel user lists
		
!!! We can choose to add/remove modes, those are just the more common one !!! */

#endif