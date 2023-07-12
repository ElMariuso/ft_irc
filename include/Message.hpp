/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 16:55:45 by root              #+#    #+#             */
/*   Updated: 2023/07/12 12:25:29 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP
# include "library.hpp"

enum MessageType
{
    UNKNOW,
    RESPONSE,
    NICK,
    PRIVMSG
};

class Server;
class Client;

class Message
{
    public:
        Message();
        Message(bool isResponse);
        Message(const std::string &message);
        ~Message();

        /* Utils */
        void                        parsing(const std::string &message);

        /* Setters */
        std::string                 setPrefix();
        MessageType                 setType();

        /* To Send */
        std::string                 welcomeMessage(Server &server, Client &client);
        std::string                 nicknameMessage(Server &server, Client &client);
        std::string                 sendMessage(Client &send, Client &dest, std::string msg);

        /* Getters */
        std::string                 getPrefix() const;
        MessageType                 getType() const;
        std::vector<std::string>    getArgs() const;

    private:
        std::string                 defaultMessage;
        std::string                 prefix;
        MessageType                 type;
        std::vector<std::string>    args;
};

#endif