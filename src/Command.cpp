/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:31 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/18 18:12:52 by mthiry           ###   ########.fr       */
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

/* WELCOME and PASS */
void Command::connectionMessage(const Server &server, const Client &client)
{
    std::stringstream   welcome;
    std::string         message;

    welcome << ":" << server.getName() << " 001 " << client.getNickname() << " :Please authenticate to get the rest of the server" << "\r\n";
    message = welcome.str();

    client.sendToFD(message);
}

void Command::welcomeMessages(const Server &server, Client *client, const std::string password)
{
    std::stringstream   welcome;
    std::string         allMessages;

    if (server.getPassword() == password)
    {
        welcome << ":" << server.getName() << " 001 " << client->getNickname() << " :Welcome to " << server.getName() \
            << ", " << client->getNickname() << "!" << client->getUsername() << "@" << client->getHostname() << "\r\n";
        welcome << ":" << server.getName() << " 002 " << client->getNickname() << " :Your host is " << server.getName() \
            << ", running version 0.1" << "\r\n";
        // std::string welcome003 = ":" + server.getName() + " 003 " + client.getNickname()
        //     + " :..." + "\r\n";
        // std::string welcome004 = ":" + server.getName() + " 004 " + client.getNickname()
        //     + " :..." + "\r\n";
        allMessages = welcome.str();

        client->sendToFD(allMessages);
        client->setIsAuthenticated(true);
    }
    else
    {
        welcome << ":" << server.getName() << " 464 " << client->getNickname() << " :Password incorrect" << "\r\n";
        allMessages = welcome.str();
        
        client->sendToFD(allMessages);
    }
}

void Command::authentificationMessages(const Server &server, const Client &client)
{
    std::stringstream   authentification;
    std::string         message;

    authentification << ":" << server.getName() << " 451 " << client.getNickname() << " :You have not registered" << "\r\n";
    message = authentification.str();

    client.sendToFD(message);
}

/* JOIN */
void Command::joinMessages(Server *server, Client *client, const std::string &channelName, const std::string &password)
{
    Channel             *channel;
    std::stringstream   join;
    std::string         allMessages;

    channel = checkForChannel(*server, channelName);
    if (channel != NULL) /* Channel */
    {
        if (channel->hasLimit() == true && channel->getConnected().size() >= channel->getLimit()) /* ERR_CHANNELISFULL (471) */
        {
            join << ":" << server->getName() << " 471 " << client->getNickname() << " " << channel->getName() \
                << " :Cannot join channel(+l) - Channel user limit reached" << "\r\n";
            allMessages = join.str();

            /* Send to the new user */
            client->sendToFD(allMessages);
            return ;
        }
        else if (false) /* ERR_INVITEONLYCHAN (473) */
        {
            
        }
        else if (channel->hasPassword() == true && password != channel->getPassword()) /* ERR_BADCHANNELKEY (475) */
        {
            join << ":" << server->getName() << " 475 " << client->getNickname() << " " << channel->getName() \
                << " :Cannot join channel(+k) - Bad channel key" << "\r\n";
            allMessages = join.str();

            /* Send to the new user */
            client->sendToFD(allMessages);
            return ;
        }
        /* Connect the user */
        channel->setConnected(client);

        /* Check if topic */
        if (channel->hasTopic() == true) /* RPL_TOPIC (332) */
        {
            join << ":" << server->getName() << " 332 " << client->getNickname() << " " << channel->getName() \
                << " :" << channel->getTopic() << "\r\n";
        }
        else /* RPL_NOTOPIC (331) */
        {
            join << ":" << server->getName() << " 331 " << client->getNickname() << " " << channel->getName() \
                << " :No topic is set" << "\r\n";
        }
        /* Create messages */
        /* RPL_NAMREPLY (353) */
        join << ":" << server->getName() << " 353 " << client->getNickname() << " " << channel->getName() \
            << " :" << userListOnChannel(channel->getConnected(), *channel) << "\r\n";
        /* RPL_ENDOFNAMES (366) */
        join << ":" << server->getName() << " 366 " << client->getNickname() << " " << channel->getName() \
            << " :End of NAMES list" << "\r\n";
        allMessages = join.str();
        
        /* Send to the new user */
        client->sendToFD(allMessages);
    }
    else /* No channel */
    {
        /* Create new channel and connect the user */
        channel = new Channel(channelName);
        channel->setConnected(client);

        /* Set the creator of the channel as operator */
        channel->addOp(*client);

        /* Add the new channel to the channel list on server */
        server->setChannel(channelName, channel);

        /* Create messages */
        /* RPL_NOTOPIC (331) */
        join << ":" << server->getName() << " 331 " << client->getNickname() << " " << channel->getName() \
            << " :No topic is set" << "\r\n";
        /* RPL_NAMREPLY (353) */
        join << ":" << server->getName() << " 353 " << client->getNickname() << " " << channel->getName() \
            << " :@" << client->getNickname() << "\r\n";
        /* RPL_ENDOFNAMES (366) */
        join << ":" << server->getName() << " 366 " << client->getNickname() << " " << channel->getName() \
            << " :End of NAMES list" << "\r\n";

        /* Send to the new user */
        allMessages = join.str();
        client->sendToFD(allMessages);
    }
}

/* PRIVMSG */
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

/* NICK */
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

/* Join utils */
std::string Command::userListOnChannel(const std::map<int, Client*> &userList, Channel &channel)
{
    std::map<int, Client*>::const_iterator  it;
    char                                    prefix;
    const Client                            *client;
    std::string                             ret;

    if (userList.empty())
        return (NULL);
    for (it = userList.begin(); it != userList.end(); ++it)
    {
        client = it->second;
        prefix = (channel.isOp(*client)) ? '@' : '+';
        ret += prefix;
        ret += client->getNickname();
        ret += ' ';
    }
    return (ret);
}

/* Messages Utils */
Channel* Command::checkForChannel(const Server &server, const std::string &nickname) /* Used in JOIN too */
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