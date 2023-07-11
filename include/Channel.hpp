/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvernimm <bvernimm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 22:24:42 by root              #+#    #+#             */
/*   Updated: 2023/07/11 11:58:46 by bvernimm         ###   ########.fr       */
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
		int		addMode(char mode); // add an channel mode, return 0 if it went well
		int		rmMode(char mode); // remove an channel mode, return 0 if it went well
		bool	hasMode(char mode); // search if channel has mode activated
		int		addOp(Client client); // add a channel operator, return 0 if it went well
		int		rmOp(Client client); // remove a channel operator, return 0 if it went well
		bool	isOp(Client client); // search if client is a channel operator
		
    private:
        std::string             _name;
        std::map<int, Client*>  _connected;
		std::vector<int> 		_operators; // list of channel operator : user who have operator status only within this channel
		std::string				_modesList;
};

/* all channel modes currently added : 
		
	invite-only (i) : users can join the channel only if they are invited by an operator
	topic protection (t) : only channel operators can change the channel topic
	key (k) : a password is required to join the channel
	limit (l) : restricts the number of users that can join the channel
*/

#endif