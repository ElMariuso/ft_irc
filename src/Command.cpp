/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:31 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/20 04:03:12 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Command.hpp"

Command::Command() {}

Command::Command(const std::string &message)
{
    this->setMessage(message);
    this->setType();
    this->setArgs();
}
Command::~Command() {}

/* NICK */
void Command::nick(const Server &server, Client *client, const std::string &name) const
{
    const std::string &serverName = server.getName();
    
    if (name.empty()) /* ERR_NONICKNAMEGIVEN (431) */
        client->sendToFD(Message::err_nonicknamegiven_431(serverName));
    else if (this->isNotRightNickname(serverName, name)) /* ERR_ERRONEUSNICKNAME (432) */
        client->sendToFD(Message::err_erroneusnickname_432(serverName, name));
    else if (!(server.findClient(name))) /* ERR_NICKNAMEINUSE (433) */
        client->sendToFD(Message::err_nicknameinuse_433(serverName, name));
    else /* NICK */
    {
        client->sendToFD(Message::nick(serverName, name));
        client->setNickname(name);
    }
}

/* JOIN */

/* PART */
void Command::part(Server *server, const Client &client, const std::string &name, const std::string &message, Channel *channel) const
{
    /* Used for messages */
    const std::string                       &serverName = server->getName();
    const std::string                       &clientName = client.getNickname();
    const std::string                       &clientUser = client.getUsername();
    const std::string                       &clientHost = client.getHostname();
    
    if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
    {
        client.sendToFD(Message::err_nosuchchannel_403(serverName, clientName, name));
        return ;
    }

    /* Get the connected list */
    const std::map<std::string, Client*>    &connected = channel->getConnected();

    if (!(channel->findConnected(client.getNickname()))) /* ERR_NOTONCHANNEL (442) */
        client.sendToFD(Message::err_notonchannel_442(serverName, clientName, name));
    else /* PART */
    {
        /* Send to all users */
        channel->sendToAll(Message::part(clientName, clientUser, clientHost, name, message), clientName, true);

        /* Remove the user from the connected list */
        channel->removeConnected(clientName);

        /* Delete the channel if there is no user left */
        if (connected.empty())
            server->removeChannel(channel);
    }
}

/* PRIVMSG */
void Command::privmsg(const Server &server, const Client &src, const std::string &destName, const std::string &message)
[
    if (destName[0] == '#')
    {
        Channel *dest;

        dest = server.findChannel(destName);
    }
    else
    {
        Client  *dest;

        dest = server.findClient(destName);
    }
]

void Command::privmsgChannel(const std::string &serverName, const Client &src, const std::string &destName, const std::string &message, Channel *channel)
{
    /* Used for messages */
    const std::string                       &srcName = client.getNickname();
    
    if (channel ==  NULL) /* ERR_CANNOTSENDTOCHAN (404) */
    {
        src.sendToFD(Message::err_cannotsendtochan_404(serverName, srcName));
        return ;
    }

    /* Get the connected list */
    const std::map<std::string, Client*>    &connected = channel->getConnected();

    if (!(channel->findConnected(srcName))) /* ERR_NOTONCHANNEL (442) */
        src.sendToFD(Message::err_notonchannel_442(serverName, srcName, destName));
    else /* PRIVMSG */
        channel->sendToAll(Message::privmsg(srcName, destName, message));
}

void Command::privmsgClient()
{

}

/* Nick Utils */
bool Command::isNotRightNickname(const std::string &serverName, const std::string &newNickname) const
{
    return (newNickname == serverName);
}

/* Setters */
void Command::setMessage(const std::string &message) { this->message = message; }

void Command::setType()
{
    std::string type;

    type = this->message.substr(0, this->message.find(' '));
    this->message.erase(0, this->message.find(' ') + 1);
    if (type == "PASS") /* Authentification */
        this->type = PASS;
    else if (type == "NICK") /* Change your nickname */
        this->type = NICK;
    else if (type == "USER")
        this->type = USER;
    else if (type == "JOIN")
        this->type = JOIN;
    else if (type == "PART")
        this->type = PART;
    else if (type == "PRIVMSG") /* Send a message */
        this->type = PRIVMSG;
    else if (type == "NOTICE")
        this->type = NOTICE;
    else if (type == "MODE")
        this->type = MODE;
    else if (type == "QUIT")
        this->type = QUIT;
    else if (type == "TOPIC")
        this->type = TOPIC;
    else if (type == "LIST")
        this->type = LIST;
    else if (type == "NAMES")
        this->type = NAMES;
    else if (type == "KICK")
        this->type = KICK;
    else if (type == "BAN")
        this->type = BAN;
    else if (type == "WHOIS")
        this->type = WHOIS;
    else if (type == "PING")
	this->type = PING;
    else if (type == "PONG")
	this->type = PONG;
    else
        this->type = UNKNOW;
}

void Command::setArgs()
{
    std::string         member;
    std::istringstream  iss(this->message);
    bool                breakReached;
    std::stringstream   rest;

    breakReached = false;
    while (std::getline(iss, member, ' '))
    {
        if (member[0] == ':')
        {
            breakReached = true;
            break ;
        }
        if (!member.empty())
            this->args.push_back(member);
    }
    if (breakReached)
    {
        rest << member;
        while (std::getline(iss, member))
        {
            if (!member.empty())
            {
                rest << ' ' << member;
                break ;
            }
        }
        member = rest.str();
        if (!member.empty() && member.length() >= 2)
        {
            member = member.substr(0, member.length() - 1);
            this->args.push_back(member);
        }
    }
    else if (!this->args.empty())
    {
        member = this->args.back();
        if (member.length() >= 2)
        {
            member = member.substr(0, member.length() - 1);
            this->args.back() = member;
        }
    }
}

/* Getters */
std::string Command::getMessage() const { return (this->message); }
MessageType Command::getType() const { return (this->type); }
std::vector<std::string> Command::getArgs() const { return (this->args); }
