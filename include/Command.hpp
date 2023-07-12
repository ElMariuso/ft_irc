/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:51 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/12 23:32:17 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
# define COMMAND_HPP
# include "library.hpp"

class Server;
class Client;

enum MessageType
{
    UNKNOW,
    PRIVMSG,
    NICK
};

class Command
{
    public:
        Command(const std::string &message);
        ~Command();

        /* Commands */
        static void                 welcomeMessages(Server &server, Client &client);
        static void                 nickMessages(Server &server, Client *client, std::string newNickname);
        static void                 msgMessages(Server &server, Client *src, Client *dest, std::string message);

        /* Nick Utils */
        static bool                 nicknameIsAlreadyInUse(Server &server, std::string newNickname);

        /* Setters */
        void                        setMessage(const std::string &message);
        void                        setType();
        void                        setArgs();

        /* Getters */
        std::string                 getMessage();
        MessageType                 getType();
        std::vector<std::string>    getArgs();
        
    private:
        std::string                 message;
        MessageType                 type;
        std::vector<std::string>    args;
};

#endif