/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:31 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/12 17:19:00 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Command.hpp"

Command::Command(const std::string &message)
{
    this->setMessage(message);
    this->setType();
    this->setArgs();
}
Command::~Command() {}

/* Commands */
void Command::welcomeMessage(Server &server, Client &client)
{
    std::string welcome001 = ":" + server.getName() + " 001 " + client.getNickname() \
        + " :Welcome to " + server.getName() + " " + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + "\r\n";
        
    client.sendToFD(welcome001);
}

/* Setters */
void Command::setMessage(const std::string &message) { this->message = message; }

void Command::setType()
{
    std::string type;

    type = this->message.substr(0, this->message.find(' '));
    this->message.erase(0, this->message.find(' ') + 1);
    if (type == "PRIVMSG")
        this->type = PRIVMSG;
    else if (type == "NICK")
        this->type = NICK;
    else
        this->type = UNKNOW;
}

void Command::setArgs()
{
    std::string arg;
    std::size_t spaceIndex;
    
    while (!this->message.empty() && this->message[0] != ':')
    {
        spaceIndex = this->message.find(' ');
        if (spaceIndex != std::string::npos)
        {
            arg = this->message.substr(0, spaceIndex);
            this->args.push_back(arg);
            this->message.erase(0, spaceIndex + 1);
        }
        else
        {
            arg = this->message;
            this->args.push_back(arg);
            this->message.clear();
        }
    }
    if (!this->message.empty() && this->message[0] == ':')
        this->args.push_back(this->message);
}

/* Getters */
std::string Command::getMessage() { return (this->message); }
MessageType Command::getType() { return (this->type); }
std::vector<std::string> Command::getArgs() { return (this->args); }