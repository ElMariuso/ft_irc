/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:51 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/20 04:50:02 by mthiry           ###   ########.fr       */
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
        Command();
        Command(const std::string &message);
        ~Command();

        /* Commands */
        void                        nick(const Server &server, Client *client, const std::string &name) const;
        void                        join(Server *server, Client *client, const std::string &name, const std::string &password, Channel *channel) const;
        void                        part(Server *server, const Client &client, const std::string &name, const std::string &message, Channel *channel) const;
        void                        privmsg(const Server &server, const Client &src, const std::string &destName, const std::string &message) const;

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
