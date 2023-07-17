/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:31 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/17 15:41:22 by mthiry           ###   ########.fr       */
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
    // client.sendToFD(welcome003);
    // client.sendToFD(welcome004);
}

void Command::privmsgMessages(Server *server, Client *src, std::string destNickname, std::string message)
{
    if (destNickname[0] == '#')
        Command::privmsgMessagesChannel(server, src, destNickname, message);
    else
        Command::privmsgMessagesUser(server, src, destNickname, message);
}

void Command::privmsgMessagesChannel(Server *server, Client *src, std::string destNickname, std::string message)
{
    Channel *dest = Command::checkForChannel(*server, destNickname);
    
    if (dest == NULL) // ERR_CANNOTSENDTOCHAN (404)
    {
        std::string msg404 = ":" + server->getName() + " 404 " + destNickname \
            + " :Cannot send to channel" + "\r\n";

        Utils::debug_message(src->getNickname() + " tried to send a message to a non-existing channel.");
        src->sendToFD(msg404);
    }
    else // Sending to channels
    {
        std::string msg001 = ":" + src->getNickname() + " PRIVMSG " \
            + destNickname + " :" + message + "\r\n";
        for (std::map<int, Client*>::iterator it = dest->getConnected().begin(); it != dest->getConnected().end(); ++it)
        {
            Client  *client = it->second;

            Utils::debug_message(src->getNickname() + " send a message to " + client->getNickname());
            client->sendToFD(msg001);
        }
    }
}

void Command::privmsgMessagesUser(Server *server, Client *src, std::string destNickname, std::string message)
{
    Client  *dest = Command::checkForUser(*server, destNickname);

    if (dest == NULL) // ERR_NOSUCHNICK (401)
    {
        std::string msg401 = ":" + server->getName() + " 401 " + destNickname \
            + " :No such nick/channel" + "\r\n";
        
        Utils::debug_message(src->getNickname() + " tried to send a message to a non-existing user.");
        src->sendToFD(msg401);
    }
    else // Sending to users
    {
        std::string msg001 = ":" + src->getNickname() + " PRIVMSG " \
            + dest->getNickname() + " :" + message + "\r\n";

        Utils::debug_message(src->getNickname() + " send a message to " + dest->getNickname());
        dest->sendToFD(msg001);
    }
    //402

    //407
    //411
    //412
    //413
    //414
    //301
}

void Command::nickMessages(Server &server, Client *client, std::string newNickname)
{
    std::string nick001 = ":" + server.getName() + " 001 " + newNickname \
        + " :You're now known as " + newNickname + "\r\n";
    std::string nick431 = ":" + server.getName() + " 431 *" \
        + " :No nickname given" + "\r\n";
    std::string nick432 = ":" + server.getName() + " 432 * " + newNickname \
        + " :Erroneous nickname" + "\r\n";
    std::string nick433 = ":" + server.getName() + " 433 * " + newNickname \
        + " :Nickname is already in use" + "\r\n";
    
    if (newNickname.empty())
    {
        Utils::debug_message("Not enough arguments on NICK from: " + client->getUsername());
        client->sendToFD(nick431);
    }
    else if (Command::isNotRightNickname(server, newNickname))
    {
        Utils::debug_message(client->getUsername() + " tried to change his nickname to an invalid nickname.");
        client->sendToFD(nick432);
    } 
    else if (Command::nicknameIsAlreadyInUse(server, newNickname))
    {
        Utils::debug_message(client->getUsername() + " tried to change his nickname to something that already exists.");
        client->sendToFD(nick433);
    }
    else
    {
        Utils::debug_message("Change " + client->getUsername() + " nickname to: " + newNickname);
        client->setNickname(newNickname);
        client->sendToFD(nick001);
    }   
}

/* Messages Utils */
Channel* Command::checkForChannel(const Server &server, const std::string &nickname)
{
    const std::map<std::string, Channel*>           &channels = server.getChannelsList();
    std::map<std::string, Channel*>::const_iterator it = channels.find(nickname);
    
    if (it != channels.end())
        return (it->second);
    return (NULL);
}

Client* Command::checkForUser(const Server &server, const std::string &nickname)
{
    const std::map<int, Client*>  &clients = server.getClientsList();
    
    if (clients.size() == 0 || clients.size() == 1)
        return (NULL);
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second && it->second->getNickname() == nickname)
            return (it->second);
        ++it;
    }
    return (NULL);
}

/* Nick Utils */
bool Command::nicknameIsAlreadyInUse(const Server &server, const std::string &newNickname)
{
    const std::map<int, Client*>  &clients = server.getClientsList();

    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->getNickname() == newNickname)
            return (true);
    }
    return (false);
}

bool Command::isNotRightNickname(const Server &server, const std::string &newNickname)
{
    if (newNickname == server.getName())
        return (true);
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
    else if (type == "USER")
        this->type = USER;
    else if (type == "JOIN")
        this->type = JOIN;
    else if (type == "PONG")
        this->type = PONG;
    else if (type == "MODE")
        this->type = MODE;
    else
        this->type = UNKNOW;
}

void Command::setArgs()
{
    std::size_t pos;

    pos = this->message.find(' ');
    if (pos != std::string::npos)
    {
        std::string arg = this->message.substr(0, pos);
        this->args.push_back(arg);
        this->message.erase(0, pos + 1);
    }
    if (!this->message.empty())
    {
        this->message = this->message.substr(0, this->message.find('\r'));
        pos = this->message.find(':');
        if (pos != std::string::npos)
            this->message.erase(0, pos + 1);
        this->args.push_back(this->message);
    }
}

/* Getters */
std::string Command::getMessage() { return (this->message); }
MessageType Command::getType() { return (this->type); }
std::vector<std::string> Command::getArgs() { return (this->args); }