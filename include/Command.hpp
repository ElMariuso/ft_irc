/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:51 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/20 02:33:37 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
# define COMMAND_HPP
# include "library.hpp"

class Server;
class Channel;
class Client;

enum MessageType
{
    UNKNOW = 0,
    PASS,
    NICK,
    USER,
    JOIN,
    PART,
    PRIVMSG,
    NOTICE,
    MODE,
    QUIT,
    TOPIC,
    LIST,
    NAMES,
    KICK,
    BAN,
    WHOIS,
    PING,
    PONG
};

class Command
{
    public:
        Command(const std::string &message);
        ~Command();

        /* NICK */
        void                        nick(const Server &server, Client *client, const std::string &name) const;
        
        /* Nick Utils */
        bool                        isNotRightNickname(const std::string &serverName, const std::string &newNickname) const;

        /* Setters */
        void                        setMessage(const std::string &message);
        void                        setType();
        void                        setArgs();

        /* Getters */
        std::string                 getMessage() const;
        MessageType                 getType() const;
        std::vector<std::string>    getArgs() const;
        
    private:
        std::string                 message;
        MessageType                 type;
        std::vector<std::string>    args;
};

#endif
