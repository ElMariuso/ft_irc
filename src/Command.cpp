/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:31 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/17 22:50:15 by mthiry           ###   ########.fr       */
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
void Command::welcomeMessages(const Server &server, const Client &client)
{
    std::stringstream   welcome;
    std::string         allMessages;

    welcome << ":" << server.getName() << " 001 " << client.getNickname() << " :Welcome to " << server.getName() \
        << ", " << client.getNickname() << "!" << client.getUsername() << "@" << client.getHostname() << "\r\n";
    welcome << ":" << server.getName() << " 002 " << client.getNickname() << " :Your host is " << server.getName() \
        << ", running version 0.1" << "\r\n";
    // std::string welcome003 = ":" + server.getName() + " 003 " + client.getNickname()
    //     + " :..." + "\r\n";
    // std::string welcome004 = ":" + server.getName() + " 004 " + client.getNickname()
    //     + " :..." + "\r\n";
    allMessages = welcome.str();

    Utils::debug_message("Send welcome messages to: " + client.getUsername());
    client.sendToFD(allMessages);
}

void Command::privmsgMessages(const Server &server, const Client &src, const std::string destNickname, const std::string message)
{
    if (destNickname[0] == '#')
        Command::privmsgMessagesChannel(server, src, destNickname, message);
    else
        Command::privmsgMessagesUser(server, src, destNickname, message);
}

void Command::privmsgMessagesChannel(const Server &server, const Client &src, const std::string destNickname, const std::string message)
{
    std::stringstream   privmsg;
    std::string         messageToSend;
    Channel             *dest;
    
    dest = Command::checkForChannel(server, destNickname);
    if (dest == NULL) /* ERR_CANNOTSENDTOCHAN (404) */
    {
        privmsg << ":" << server.getName() << " 404 " << destNickname \
            << " :Cannot send to channel" << "\r\n";
        messageToSend = privmsg.str();
        src.sendToFD(messageToSend);
    }
    else /* PRIVMSG */
    {
        const std::map<int, Client*>    &connectedClients = dest->getConnected();
        privmsg << ":" << src.getNickname() << " PRIVMSG " \
            << destNickname << " :" << message << "\r\n";
        messageToSend = privmsg.str();
        for (std::map<int, Client*>::const_iterator it = connectedClients.begin(); it != connectedClients.end(); ++it)
        {
            Client  &client = *it->second;
            client.sendToFD(messageToSend);
        }
    }
}

void Command::privmsgMessagesUser(const Server &server, const Client &src, const std::string destNickname, const std::string message)
{
    std::stringstream   privmsg;
    std::string         messageToSend;
    Client              *dest;
    
    dest = Command::checkForUser(server, destNickname);
    if (dest == NULL) /* ERR_NOSUCHNICK (401) */
    {
        privmsg << ":" << server.getName() << " 401 " << destNickname \
            << " :No such nick/channel" << "\r\n";
        messageToSend = privmsg.str();
        src.sendToFD(messageToSend);
    }
    else /* PRIVMSG */
    {
        privmsg << ":" << src.getNickname() << " PRIVMSG " \
            << dest->getNickname() << " :" << message << "\r\n";
        messageToSend = privmsg.str();
        dest->sendToFD(messageToSend);
    }
}

void Command::nickMessages(const Server &server, Client *client, const std::string newNickname)
{
    std::stringstream   nick001;
    std::stringstream   nick431;
    std::stringstream   nick432;
    std::stringstream   nick433;
    std::string         message;

    nick001 << ":" << server.getName() << " 001 " << newNickname \
        << " :You're now known as " << newNickname << "\r\n";
    nick431 << ":" << server.getName() << " 431 *" \
        << " :No nickname given" << "\r\n";
    nick432 << ":" << server.getName() << " 432 * " << newNickname \
        << " :Erroneous nickname" << "\r\n";
    nick433 << ":" << server.getName() << " 433 * " << newNickname \
        << " :Nickname is already in use" << "\r\n";;
    if (newNickname.empty())
    {
        message = nick431.str();
        client->sendToFD(message);
    }
    else if (Command::isNotRightNickname(server, newNickname))
    {
        message = nick432.str();
        client->sendToFD(message);
    } 
    else if (Command::nicknameIsAlreadyInUse(server, newNickname))
    {
        message = nick433.str();
        client->sendToFD(message);
    }
    else
    {
        message = nick001.str();
        client->sendToFD(message);
        client->setNickname(newNickname);
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
    
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second && it->second->getNickname() == nickname)
            return (it->second);
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
    return (newNickname == server.getName());
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
    std::string member;

    while (!this->message.empty())
    {
        pos = this->message.find(' ');
        if (pos != std::string::npos)
        {
            member = this->message.substr(0, pos);
            this->args.push_back(member);
            this->message.erase(0, pos + 1);
        }
        else
        {
            member = this->message.substr(0, this->message.find('\r'));
            this->args.push_back(member);
            this->message.clear();
        }
        if (this->message[0] == ':')
            break ;
    }
    if (!this->message.empty())
    {
        this->message = this->message.substr(0, this->message.find('\r'));
        pos = this->message.find(':');
        if (pos != std::string::npos)
            this->message.erase(0, pos);
        this->args.push_back(this->message);
    }
}

/* Getters */
std::string Command::getMessage() const { return (this->message); }
MessageType Command::getType() const { return (this->type); }
std::vector<std::string> Command::getArgs() const { return (this->args); }