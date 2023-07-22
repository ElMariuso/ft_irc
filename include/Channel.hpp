/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 22:24:42 by root              #+#    #+#             */
/*   Updated: 2023/07/21 22:10:22 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "library.hpp"

class Client;

class Channel
{
    public:
		Channel(const std::string &name);
		~Channel();

		/* mode function */
		int											addMode(const char &mode); // add an channel mode, return 0 if it went well
		int											rmMode(const char &mode); // remove an channel mode, return 0 if it went well
		bool										hasMode(const char &mode) const; // search if channel has mode activated
		bool										isMode(const char &mode) const;
		int											addOp(const Client &client); // add a channel operator, return 0 if it went well
		int											rmOp(const Client &client); // remove a channel operator, return 0 if it went well
		bool										isOp(const Client &client) const; // search if client is a channel operator
		bool										hasTopic() const;

		/* Setters */
		void										setName(const std::string &name);
		void										setConnected(Client *client);
        void										setConnectedList(std::map<int, Client*> connected);
		void										setOperator(int fd);
		void										setOperators(std::vector<int> operators);
		void 										setModesList(const std::string &modesList);
		void										setTopic(const std::string &topic); // change the channel topic
		void										setPassword(const std::string &password);
		void										setLimit(std::size_t limit);
		void										setInvited(const std::string &name);
		void										setInvitedList(std::map<std::string, bool> &invited);
		void										setHasInvitedList(bool hasInvitedList);
		void										setHasTopicProtection(bool hasTopicProtection);
		void										setHasPassword(bool hasPassword);
		void										setHasLimit(bool hasLimit);

		/* Removers */
		void										removeConnected(const int &fd);

		/* Getters */
		std::string									getName() const;
        std::map<int, Client*>						getConnected() const;
		std::vector<int>							getOperators() const;
		std::string									getModesList() const;
		std::string									getTopic() const;
		std::string									getPassword() const;
		std::size_t									getLimit() const;
		std::map<std::string, bool>					getInvited() const;
		bool										getHasInvitedList() const;
		bool										getHasTopicProtection() const;
		bool										getHasPassword() const;
		bool										getHasLimit() const;

		std::map<int, Client*>::const_iterator		getConnectedEnd() const;
		std::map<std::string, bool>::const_iterator	getInvitedEnd() const;

		/* Finders */
		Client*										findConnected(const int &fd);
		std::map<int, Client*>::const_iterator		findConnectedByName(const std::string &name) const;

		/* Senders */
		void										sendToAll(const std::string &message, const std::string &srcName, bool sendToSRC);

		/* Clearers */
		void										clearInvited();
		
    private:
        std::string             	_name;
        std::map<int, Client*>		_connected;
		std::vector<int> 			_operators; // list of channel operator : user who have operator status only within this channel
		std::string					_modesList;
		std::string					_topic;
		std::string					password;
		std::size_t					limit;
		std::map<std::string, bool>	invited;
		bool						hasInvitedList;
		bool						hasTopicProtection;
		bool						hasPassword;
		bool						hasLimit;
};

/* all channel modes currently added : 
		
	invite-only (i) : users can join the channel only if they are invited by an operator
	topic protection (t) : only channel operators can change the channel topic
	key (k) : a password is required to join the channel
	limit (l) : restricts the number of users that can join the channel
*/

#endif