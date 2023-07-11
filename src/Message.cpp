/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 17:35:11 by root              #+#    #+#             */
/*   Updated: 2023/07/11 20:32:09 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Message.hpp"

Message::Message(): type(UNKNOW) {}
Message::Message(bool isResponse)
{
    if (isResponse == true)
        this->type = RESPONSE;
    else
        this->type = UNKNOW;
}
Message::Message(const std::string &message) { this->parsing(message); }
Message::~Message() {}

/* Utils */
void Message::parsing(const std::string &message)
{
    this->defaultMessage = message;
    if (message[0] == ':')
        this->prefix = this->setPrefix();
    this->type = this->setType();
    for (std::size_t i = 0; i != this->defaultMessage.size(); ++i)
    {
        if (this->defaultMessage[i] == ':')
        {
            this->defaultMessage.erase(0, 1);
            this->args.push_back(this->defaultMessage);
            break ;
        }
        else
        {
            this->args.push_back(this->defaultMessage.substr(0, this->defaultMessage.find(' ')));
            this->defaultMessage.erase(0, this->defaultMessage.find(' '));
        }
    }
}

/* Setters */
std::string Message::setPrefix()
{
    std::string prefix;

    prefix = this->defaultMessage.substr(0, this->defaultMessage.find(' '));
    this->defaultMessage.erase(0, this->defaultMessage.find(' '));
    return (prefix);
}

MessageType Message::setType()
{
    std::string type;

    type = this->defaultMessage.substr(0, this->defaultMessage.find(' '));
    this->defaultMessage.erase(0, this->defaultMessage.find(' '));
    if (type == "NICK")
        return (NICK);
    return (UNKNOW);
}

/* To Send */
std::string Message::welcomeMessage(Server &server, Client &client)
{
    std::string welcome = "Welcome to " + server.getName() + "!";
    std::string welcomeResponse = ":" + server.getName() + " 001 " + client.getNickname() + " :" + welcome + "\r\n";

    return (welcomeResponse);
}

std::string Message::nicknameMessage(Server &server, Client &client)
{
    std::string success = "Your nickname has been changed successfully!";
    std::string message = ":" + server.getName() + " 001 " + client.getNickname() + " :" + success + "\r\n";

    return (message);
}

/* Getters */
std::string Message::getPrefix() const { return (this->prefix); }
MessageType Message::getType() const { return (this->type); }
std::vector<std::string> Message::getArgs() const { return (this->args); }
