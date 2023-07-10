/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 22:24:42 by root              #+#    #+#             */
/*   Updated: 2023/07/05 10:31:35 by bvernimm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "library.hpp"

class Client;

class Channel
{
    public:

		/* mode function */
		int		addMode(char mode); // add an user mode, return 0 if it went well
		int		rmMode(char mode); // remove an user mode, return 0 if it went well
		bool	hasMode(char mode); // search if client has mode activated
		
    private:
        std::string             name;
        std::map<int, Client*>  connected;
		std::string				_modesList;
};

/* all channel modes currently added : 
		
	topic protection (t) : only channel operators can change the channel topic
	no external messages (n) : only users who have joined the channel can send messages to it
	secret (s) : the channel is not listed in the channel listing (/list command)
	invite-only (i) : users can join the channel only if they are invited by an operator
	key (k) : a password is required to join the channel
	private (p) : the channel does not appear in WHOIS responses
	limit (l) : restricts the number of users that can join the channel
	
!!! We can choose to add/remove modes, those are just the more common one !!! */

#endif