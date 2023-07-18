/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:51 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/18 15:13:30 by mthiry           ###   ########.fr       */
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
    UNKNOW,
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
    WHOIS
};

class Command
{
    public:
        Command(const std::string &message);
        ~Command();

        /* WELCOME and PASS */
        static void                 connectionMessage(const Server &server, const Client &client);
        static void                 welcomeMessages(const Server &server, Client *client, const std::string password);
        static void                 authentificationMessages(const Server &server, const Client &client);

        /* JOIN */
        static void                 joinMessages(Server *server, const std::string &channelName, const std::string &password);

        /* PRIVMSG */
        static void                 privmsgMessages(const Server &server, const Client &src, const std::string destNickname, const std::string message);
        static void                 privmsgMessagesChannel(const Server &server, const Client &src, const std::string destNickname, const std::string message);
        static void                 privmsgMessagesUser(const Server &server, const Client &src, const std::string destNickname, const std::string message);

        /* NICK */
        static void                 nickMessages(const Server &server, Client *client, const std::string newNickname);

        /* Messages Utils */
        static Channel*             checkForChannel(const Server &server, const std::string &nickname);
        static Client*              checkForUser(const Server &server, const std::string &nickname);

        /* Nick Utils */
        static bool                 nicknameIsAlreadyInUse(const Server &server, const std::string &newNickname);
        static bool                 isNotRightNickname(const Server &server, const std::string &newNickname);

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