/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:51 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/21 22:41:14 by mthiry           ###   ########.fr       */
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
        void                        mode(const Server &server, Client *src, const std::string &destName, const std::string &modes, const std::string &args) const;
        void                        modeCheck(const std::string &serverName, const std::string &srcName, const std::string &destName, const Client &client, const Server &server) const;
        void                        modeAdd(const std::string &serverName, const std::string &srcName, const std::string &destName, Client *src, const Server &server, const std::string &modes, const std::string &args) const;
        void                        kick(const Server &server, const Client &src, Client *dest, const std::string &message, Channel *channel) const;

        /* Nick Utils */
        bool                        isNotRightNickname(const std::string &serverName, const std::string &newNickname) const;

        /* MODE Utils */
        void                        setModes(const std::string &serverName, Channel *channel, const Client &src, const std::string &modes, const std::string &args) const;
        void                        addMode(Channel *channel, const char &mode) const;
        void                        rmMode(Channel *channel, const char &mode) const;
        void                        changeRestriction(const std::string &serverName, Channel *channel, const Client &src, const char &mode, const std::string &args) const;
        bool                        isInteger(const std::string &str) const;

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
