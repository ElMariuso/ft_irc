/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 17:35:11 by root              #+#    #+#             */
/*   Updated: 2023/07/12 00:46:57 by mthiry           ###   ########.fr       */
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
    std::size_t pos;
    
    this->defaultMessage = message;
    if (message[0] == ':')
        this->prefix = this->setPrefix();
    this->type = this->setType();
    while ((pos = this->defaultMessage.find(' ')) != std::string::npos)
    {
        std::string arg = this->defaultMessage.substr(0, pos);
        this->args.push_back(arg);
        this->defaultMessage.erase(0, pos + 1);
    }
    if (!this->defaultMessage.empty())
    {
        this->defaultMessage = this->defaultMessage.substr(0, this->defaultMessage.find('\r'));
        this->args.push_back(this->defaultMessage);
    }
}

/* Setters */
std::string Message::setPrefix()
{
    std::string prefix;

    prefix = this->defaultMessage.substr(0, this->defaultMessage.find(' '));
    this->defaultMessage.erase(0, this->defaultMessage.find(' ') + 1);
    return (prefix);
}

MessageType Message::setType()
{
    std::string type;
    
    type = this->defaultMessage.substr(0, this->defaultMessage.find(' '));
    this->defaultMessage.erase(0, this->defaultMessage.find(' ') + 1);
    if (type == "NICK")
        return (NICK);
    else if (type == "PRIVMSG")
        return (PRIVMSG);
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

std::string Message::sendMessage(std::string dest, std::string msg)
{
    std::string message = "PRIVMSG " + dest + msg + "\r\n";

    return (message);
}

/* Getters */
std::string Message::getPrefix() const { return (this->prefix); }
MessageType Message::getType() const { return (this->type); }
std::vector<std::string> Message::getArgs() const { return (this->args); }
