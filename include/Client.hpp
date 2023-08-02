/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 22:25:55 by root              #+#    #+#             */
/*   Updated: 2023/08/02 16:05:36 by mthiry           ###   ########.fr       */
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
		Client(const std::string &nickname, const std::string &username, int fd, bool isConnected);
        ~Client();

		/* function */
		void		sendToFD(const std::string &str) const; // write a string on the client's fd
		std::string	receiveFromFD(); // receive and return a string written by the client

		/* mode function */
		int			addMode(char mode); // add an user mode, return 0 if it went well
		int			rmMode(char mode); // remove an user mode, return 0 if it went well
		bool		hasMode(std::string mode); // search if client has mode activated with the mode's name
		bool		hasModeLetter(char mode); // search if client has mode activated with the mode's letter
		bool		isMode(const char &mode) const;

		/* Setters */
		void		setFd(const int fd);
		void		setIsAuthenticated(bool isAuthenticated);
		void		setIsConnected(bool isConnected);
		void		setIsRegistered(bool Registered);
		void		setNickname(std::string nickName);
		void		setDefaultNickname(std::string nickname);
		void		setUsername(std::string userName);
		void		setDefaultUsername(std::string userName);
		void		setHostname();
		void		setRealname(std::string realName);
		void		setLastActivityTime(std::time_t time);
		void		setLastPingTime(std::time_t time);
		void		setSavedCommand(std::string savedCommand);
		
		/* Getters */
		int			getFd() const;
		bool		getIsAuthenticated() const;
		bool		getIsConnected() const;
		bool		getIsRegistered() const;
		std::string	getNickname() const;
		std::string	getDefaultNickname() const;
		std::string	getUsername() const;
		std::string	getDefaultUsername() const;
		std::string	getHostname() const;
		std::string	getRealname() const;
		std::string	getModesList() const;
		std::time_t	getLastActivityTime() const;
		std::time_t	getLastPingTime() const;
		std::string	getSavedCommand() const;
        
    private:
        int         _fd;
		bool		isAuthenticated;
		bool		isConnected;
		bool		isRegistered;
		std::string	_defaultNickname;
        std::string _nickname;
		std::string	_defaultUsername;
        std::string _username;
		std::string	_hostname;
		std::string	_realname;
		std::string _modesList;
		std::time_t	lastActivityTime;
		std::time_t	lastPingTime;
		std::string	savedCommand;
};

#endif
