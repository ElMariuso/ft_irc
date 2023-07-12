/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:31 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/12 18:23:32 by mthiry           ###   ########.fr       */
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
void Command::welcomeMessages(Server &server, Client &client)
{
    std::string welcome001 = ":" + server.getName() + " 001 " + client.getNickname() \
        + " :Welcome to " + server.getName() + ", " + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + "\r\n";
    std::string welcome002 = ":" + server.getName() + " 002 " + client.getNickname() \
        + " :Your host is " + server.getName() + ", running version 0.1" + "\r\n";
    std::string welcome003 = ":" + server.getName() + " 003 " + client.getNickname() \
        + " :..." + "\r\n";
    std::string welcome004 = ":" + server.getName() + " 004 " + client.getNickname() \
        + " :..." + "\r\n";
        
    Utils::debug_message("Send welcome message to: " + client.getUsername());
    client.sendToFD(welcome001);
    client.sendToFD(welcome002);
    client.sendToFD(welcome003);
    client.sendToFD(welcome004);
}

void Command::nickMessages(Server &server, Client &client, std::string newNickname)
{
    std::string nick001 = ":" + server.getName() + " NICK " + client.getNickname() \
        + " :..." + "\r\n";
    std::string nick431 = ":" + server.getName() + " 431 *" \
        + " :No nickname given" + "\r\n";
    std::string nick432 = ":" + server.getName() + " 432 * " + newNickname \
        + " :Erroneous nickname" + "\r\n";
    std::string nick433 = ":" + server.getName() + " 433 * " + newNickname \
        + " :Nickname is already in use" + "\r\n";
    
    if (newNickname.empty())
    {
        client.sendToFD(nick431);   
    }
    else if (Command::nicknameIsAlreadyInUse(server, newNickname))
    {
        client.sendToFD(nick433);
    }
    else
    {
        client.setNickname(newNickname);
        client.sendToFD(nick001);
    }

    /* Can be added later */
    // else if ()
    // {
    //     client.sendToFD(nick432);
    // }    
}

/* Utils */
bool Command::nicknameIsAlreadyInUse(Server &server, std::string newNickname)
{
    std::map<int, Client*>              clients;
    std::map<int, Client*>::iterator    it;

    clients = server.getClientsList();
    it = clients.begin();
    while (it != clients.end())
    {
        if (it->second->getNickname() == newNickname)
            return (true);
        ++it;
    }
    return (false);
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