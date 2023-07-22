/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:31 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/22 14:56:30 by mthiry           ###   ########.fr       */
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

/* Commands */
void Command::nick(const Server &server, Client *client, const std::string &name) const
{
    const std::string               &serverName = server.getName();
    
    if (name.empty()) /* ERR_NONICKNAMEGIVEN (431) */
        client->sendToFD(Message::err_nonicknamegiven_431(serverName));
    else if (this->isNotRightNickname(serverName, name)) /* ERR_ERRONEUSNICKNAME (432) */
        client->sendToFD(Message::err_erroneusnickname_432(serverName, name));
    else if (server.findClientByName(name) != server.getClientsListEnd()) /* ERR_NICKNAMEINUSE (433) */
        client->sendToFD(Message::err_nicknameinuse_433(serverName, name));
    else /* NICK */
    {
        client->sendToFD(Message::nick(serverName, name));
        client->setNickname(name);
    }
}

void Command::join(Server *server, Client *client, const std::string &name, const std::string &password, Channel *channel) const
{
    /* Used for messages */
    const std::string                       &serverName = server->getName();
    const std::string                       &clientName = client->getNickname();
    const std::string                       &clientUser = client->getUsername();
    const std::string                       &clientHost = client->getHostname();

    if (channel != NULL) /* JOIN with channel */
    {
        /* Get some lists */
        const std::map<int, Client*>        &connected = channel->getConnected();
        const std::map<std::string, bool>   &invited = channel->getInvited();
        const std::string                   &passwordChannel = channel->getPassword();

        if (channel->getHasLimit() && connected.size() >= channel->getLimit()) /* ERR_CHANNELISFULL (471) */
            client->sendToFD(Message::err_channelisfull_471(serverName, clientName, name));
        else if (channel->getHasInvitedList() && invited.find(clientName) == invited.end()) /* ERR_INVITEONLYCHAN (473) */
            client->sendToFD(Message::err_inviteonlychan_473(serverName, clientName, name));
        else if (channel->getHasPassword() && password != passwordChannel) /* ERR_BADCHANNELKEY (475) */
            client->sendToFD(Message::err_badchannelkey_475(serverName, clientName, name));
        else if (channel->findConnectedByName(clientName) != channel->getConnectedEnd()) /* ERR_USERONCHANNEL (443) */
            client->sendToFD(Message::err_useronchannel_443(serverName, clientName, name));
        else /* JOIN */
        {
            /* Connect the new user */
            channel->setConnected(client);
            
            /* Send JOIN notification to all */
            channel->sendToAll(Message::join(clientName, clientUser, clientHost, name), clientName, true);

            /* Check for a topic */
            /* Send a message for the new user */
            const std::string   &msg331 = Message::rpl_notopic_331(serverName, clientName, name);
            const std::string   &msg332 = Message::rpl_topic_332(serverName, clientName, name, channel->getTopic());
            const std::string   &msg353 = Message::rpl_namreplay_353(serverName, clientName, name, *channel);
            const std::string   &msg366 = Message::rpl_endofnames_366(serverName, clientName, name);
            std::string         toSend;
            
            if (!channel->hasTopic()) /* RPL_NOTOPIC (331) */
                toSend += msg331;
            else /* RPL_TOPIC (332) */
                toSend += msg332;
            toSend += msg353;
            toSend += msg366;
            client->sendToFD(toSend);
        }
    }
    else /* JOIN without channel */
    {
        /* Create new channel and connect the user */
        channel = new Channel(name);
        channel->setConnected(client);

        /* Set the creator of the channel as operator */
        channel->addOp(*client);

        /* Add the new channel to the channel list on server */
        server->setChannel(name, channel);

        /* Send a message for the new user */
        const std::string   &msg331 = Message::rpl_notopic_331(serverName, clientName, name);
        const std::string   &msg353 = Message::rpl_namreplay_353(serverName, clientName, name, *channel);
        const std::string   &msg366 = Message::rpl_endofnames_366(serverName, clientName, name);

        /* Send confirmation message */
        client->sendToFD(Message::join(clientName, clientUser, clientHost, name) + msg331 + msg353 + msg366);
    }
}

void Command::part(Server *server, const Client &client, const std::string &name, const std::string &message, Channel *channel) const
{
    /* Used for messages */
    const std::string               &serverName = server->getName();
    const std::string               &clientName = client.getNickname();
    const std::string               &clientUser = client.getUsername();
    const std::string               &clientHost = client.getHostname();
    
    if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
    {
        client.sendToFD(Message::err_nosuchchannel_403(serverName, clientName, name));
        return ;
    }

    if (channel->findConnectedByName(client.getNickname()) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
        client.sendToFD(Message::err_notonchannel_442(serverName, clientName, name));
    else /* PART */
    {
        /* Send to all users */
        channel->sendToAll(Message::part(clientName, clientUser, clientHost, name, message), clientName, true);

        /* Remove the user from the connected list */
        channel->removeConnected(client.getFd());

        /* Get the connected list */
        const std::map<int, Client*>    &connected = channel->getConnected();

        /* Delete the channel if there is no user left */
        if (connected.empty())
            server->removeChannel(channel);
    }
}

void Command::privmsg(const Server &server, const Client &src, const std::string &destName, const std::string &message) const
{
    /* Used for messages */
    const std::string   &serverName = server.getName();
    const std::string   &srcName = src.getNickname();
    
    if (destName[0] == '#')
    {
        Channel *channel;

        /* Check if the channel exists */
        channel = server.findChannel(destName);
        if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
        {
            src.sendToFD(Message::err_nosuchchannel_403(serverName, srcName, destName));
            return ;
        }

        /* Get the connected list */
        const std::map<int, Client*>    &connected = channel->getConnected();
        
        if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
            src.sendToFD(Message::err_notonchannel_442(serverName, srcName, destName));
        else /* PRIVMSG */
            channel->sendToAll(Message::privmsg(srcName, destName, message), srcName, false);
    }
    else
    {
        const std::map<int, Client*>            &clients = server.getClientsList();
        std::map<int, Client*>::const_iterator  it = server.findClientByName(destName);
        Client                                  *client;

        /* Check if the user exists */
        if (it == clients.end()) /* ERR_NOSUCHNICK (401) */
        {
            src.sendToFD(Message::err_nosuchnick_401(serverName, destName));
            return ;
        }
        client = it->second;
        client->sendToFD(Message::privmsg(srcName, destName, message));
    }
}

void Command::mode(const Server &server, Client *src, const std::string &destName, const std::string &modes, const std::string &args) const
{
    /* Used for messages */
    const std::string   &serverName = server.getName();
    const std::string   &srcName = src->getNickname();

    if (modes.empty()) /* Check modes */
        this->modeCheck(serverName, srcName, destName, *src, server);
    else /* Add modes */
        this->modeAdd(serverName, srcName, destName, src, server, modes, args);
}

void Command::modeCheck(const std::string &serverName, const std::string &srcName, const std::string &destName, const Client &client, const Server &server) const
{
    if (destName[0] == '#') /* Channel */
    {
        Channel *channel;

        /* Check if the channel exists */
        channel = server.findChannel(destName);
        if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
        {
            client.sendToFD(Message::err_nosuchchannel_403(serverName, srcName, destName));
            return ;
        }

        /* Get modes list */
        const std::string               &modes = channel->getModesList();

        if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
            client.sendToFD(Message::err_notonchannel_442(serverName, srcName, destName));
        else if (modes.empty()) /* Send there is no modes */
            client.sendToFD(Message::rpl_channelmodesis_324(serverName, srcName, destName, "No mode is set up"));
        else /* Send mode list */
            client.sendToFD(Message::rpl_channelmodesis_324(serverName, srcName, destName, "Actual modes: " + modes));
    }
    else /* Client */
    {
        /* Get modes list */
        const std::string               &modes = client.getModesList();

        if (modes.empty()) /* Send there is no modes */
            client.sendToFD(Message::rpl_umodeis_221(serverName, srcName, "Nothing"));
        else /* Send mode list */
            client.sendToFD(Message::rpl_umodeis_221(serverName, srcName, modes));
    }
}

void Command::modeAdd(const std::string &serverName, const std::string &srcName, const std::string &destName, Client *src, const Server &server, const std::string &modes, const std::string &args) const
{
    if (destName[0] == '#') /* Channel */
    {
        Channel *channel;

        /* Check if the channel exists */
        channel = server.findChannel(destName);
        if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
        {
            src->sendToFD(Message::err_nosuchchannel_403(serverName, srcName, destName));
            return ;
        }

        /* Check if modes exists */
        for (std::size_t i = 1; i < modes.length(); ++i) /* ERR_UMODEUNKNOWNFLAG (501) */
        {
            if (!channel->isMode(modes[i]))
            {
                src->sendToFD(Message::err_umodeunknowflag_501(serverName, srcName));
                return ;
            }
        }

        if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
            src->sendToFD(Message::err_notonchannel_442(serverName, srcName, destName));
        else if (!channel->isOp(*src)) /* ERR_CHANOPRIVSNEEDED (482) */
            src->sendToFD(Message::err_chanoprivsneeded_482(serverName, srcName, destName));
        else /* MODES */
            this->setModes(serverName, srcName, destName, channel, *src, modes, args);
    }
    else /* Client */
    {
        if (srcName != destName) /* ERR_USERSDONTMATCH (502) */
        {
            src->sendToFD(Message::err_usersdontmatch_502(serverName, srcName));
            return ;
        }

        /* Check if modes exists */
        for (std::size_t i = 0; i < modes.length(); ++i) /* ERR_UMODEUNKNOWNFLAG (501) */
        {
            if (!src->isMode(modes[i]) && modes[i] != '+')
            {
                src->sendToFD(Message::err_umodeunknowflag_501(serverName, srcName));
                return ;
            }
        }

        /* Change modes of the user */
        this->setModesClient(serverName, srcName, src, modes);
    }
}

void Command::topic(const Server &server, const Client &src, const std::string &destName, const std::string &topic) const
{
    Channel                         *channel;

    /* Used for messages */
    const std::string               &serverName = server.getName();
    const std::string               &srcName = src.getNickname();
    
    if (destName[0] != '#') /* ERR_NOSUCHCHANNEL (403) */
    {
        src.sendToFD(Message::err_nosuchchannel_403(serverName, srcName, destName));
        return ;
    }

    /* Check if the channel exists */
    channel = server.findChannel(destName);
    if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
    {
        src.sendToFD(Message::err_nosuchchannel_403(serverName, srcName, destName));
        return ;
    }

    /* Used for messages */
    const std::string               &channelName = channel->getName();

    if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
        src.sendToFD(Message::err_notonchannel_442(serverName, srcName, destName));
    else /* TOPIC */
    {
        if (topic.empty()) /* Send the topic */
        {
            if (channel->hasTopic()) /* Send the topic */
                src.sendToFD(Message::rpl_topic_332(serverName, srcName, channelName, topic));
            else /* There is no topic */
                src.sendToFD(Message::rpl_notopic_331(serverName, srcName, channelName));
        }
        else /* Change the topic */
        {
            if (channel->getHasTopicProtection() && !channel->isOp(src)) /* ERR_CHANOPRIVSNEEDED (482) */
                src.sendToFD(Message::err_chanoprivsneeded_482(serverName, srcName, destName));
            else /* Change the topic */
            {
                std::string newTopic = topic.substr(1);

                channel->setTopic(newTopic);
                src.sendToFD(Message::rpl_topic_332(serverName, srcName, channelName, newTopic));
            }
        }
    }
}

void Command::kick(const Server &server, const Client &src, Client *dest, const std::string &message, Channel *channel) const
{
    /* Used for messages */
    const std::string               &serverName = server.getName();
    const std::string               &srcName = src.getNickname();
    const std::string               &destName = dest->getNickname();
    const std::string               &channelName = channel->getName();

    /* Check if the channel and the user exists */
    if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
    {
        src.sendToFD(Message::err_nosuchchannel_403(serverName, srcName, channelName));
        return ;
    }
    else if (dest == NULL) /* ERR_NOSUCHNICK (401) */
    {
        src.sendToFD(Message::err_nosuchnick_401(serverName, destName));
        return ;
    }

    if (channel->findConnectedByName(destName) == channel->getConnectedEnd()) /* ERR_USERNOTINCHANNEL (441) */
        src.sendToFD(Message::err_usernotinchannel_441(serverName, destName, channelName));
    else if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
        src.sendToFD(Message::err_notonchannel_442(serverName, srcName, channelName));
    else if (!channel->isOp(src)) /* ERR_CHANOPRIVSNEEDED (482) */
        src.sendToFD(Message::err_chanoprivsneeded_482(serverName, srcName, channelName));
    else /* KICK */
    {
        /* Send to all users */
        channel->sendToAll(Message::kick(srcName, destName, channelName, message), srcName, true);

        /* Remove the user from the connected list */
        channel->removeConnected(dest->getFd());
    }
}

void Command::invite(const Server &server, const Client &src, Client *dest, Channel *channel) const
{
    /* Used for messages */
    const std::string               &serverName = server.getName();
    const std::string               &srcName = src.getNickname();
    const std::string               &destName = dest->getNickname();
    const std::string               &channelName = channel->getName();

    /* Check if the channel and the user exists */
    if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
    {
        src.sendToFD(Message::err_nosuchchannel_403(serverName, srcName, channelName));
        return ;
    }
    else if (dest == NULL) /* ERR_NOSUCHNICK (401) */
    {
        src.sendToFD(Message::err_nosuchnick_401(serverName, destName));
        return ;
    }
    else if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
        src.sendToFD(Message::err_notonchannel_442(serverName, srcName, channelName));
    else if (channel->findConnectedByName(destName) != channel->getConnectedEnd()) /* ERR_USERONCHANNEL (443) */
        src.sendToFD(Message::err_useronchannel_443(serverName, destName, channelName));
    else if (channel->getHasInvitedList() && !channel->isOp(src)) /* ERR_CHANOPRIVSNEEDED (482) */
        src.sendToFD(Message::err_chanoprivsneeded_482(serverName, srcName, channelName));
    else /* INVITE */
    {
        
    }
}

/* Nick Utils */
bool Command::isNotRightNickname(const std::string &serverName, const std::string &newNickname) const
{
    return (newNickname == serverName);
}

/* MODE Utils */
void Command::setModes(const std::string &serverName, const std::string &srcName, const std::string &channelName, Channel *channel, const Client &src, const std::string &modes, const std::string &args) const
{
    std::string modes2 = "";

    bool    removing = true;
    /* Checking if you need to remove or not */
    for (std::size_t i = 1; i < modes.length(); ++i)
    {
        if (!channel->hasMode(modes[i]))
        {
            removing = false;
            break ;
        }
    }

    /* Parsing modes */
    for (std::size_t i = 1; i < modes.length(); ++i)
    {
        char    letter = modes[i];
        if (modes2.find(letter) == std::string::npos)
            modes2 += letter;
    }
    if (args.empty() && removing) /* Removing */
    {
        /* Removing all modes */
        for (std::size_t i = 0; i < modes2.length(); ++i)
        {
            channel->rmMode(modes2[i]);
            this->rmMode(channel, modes2[i]);
        }
        channel->sendToAll(Message::rpl_channelmodesis_324(serverName, srcName, channelName, "Removing: " + modes2), srcName, true);
    }
    else /* Adding */
    {
        if (args.empty() || modes2.length() > 1)
        {
            /* Setting modes */
            for (std::size_t i = 0; i < modes2.length(); ++i)
            {
                channel->addMode(modes2[i]);
                this->addMode(channel, modes2[i]);
            }
            channel->sendToAll(Message::rpl_channelmodesis_324(serverName, srcName, channelName, "Adding: " + modes2), srcName, true);
        }
        else /* Change restrictions */
            this->changeRestriction(serverName, channel, src, modes2[0], args);
    }
}

void Command::setModesClient(const std::string &serverName, const std::string &srcName, Client *src, const std::string &modes) const
{
    std::string modes2 = "";

    /* Parsing modes */
    for (std::size_t i = 0; i < modes.length(); ++i)
    {
        char    letter = modes[i];
        if (modes2.find(letter) == std::string::npos)
            modes2 += letter;
    }

    if (modes2[0] != '+') /* Removing */
    {
        /* Removing all modes */
        for (std::size_t i = 0; i < modes2.length(); ++i)
            src->rmMode(modes2[i]);
        src->sendToFD(Message::rpl_umodeis_221(serverName, srcName, modes));
    }
    else /* Adding */
    {
        /* Setting modes */
        for (std::size_t i = 1; i < modes2.length(); ++i)
            src->addMode(modes2[i]);
        src->sendToFD(Message::rpl_umodeis_221(serverName, srcName, modes));
    }
}

void Command::addMode(Channel *channel, const char &mode) const
{
    switch (mode)
    {
        case 'i':
            channel->setHasInvitedList(true);
            break ;
        case 't':
            channel->setHasTopicProtection(true);
            break ;
        case 'k':
            channel->setHasPassword(true);
            break ;
        case 'l':
            channel->setHasLimit(true);
            break ;
        default:
            break ;
    }   
}

void Command::rmMode(Channel *channel, const char &mode) const
{
    switch (mode)
    {
        case 'i':
            channel->setHasInvitedList(false);
            channel->clearInvited();
            break ;
        case 't':
            channel->setHasTopicProtection(false);
            break ;
        case 'k':
            channel->setHasPassword(false);
            channel->setPassword("");
            break ;
        case 'l':
            channel->setHasLimit(false);
            channel->setLimit(0);
            break ;
        default:
            break ;
    }   
}

void Command::changeRestriction(const std::string &serverName, Channel *channel, const Client &src, const char &mode, const std::string &args) const
{
    switch (mode)
    {
        case 'k':
            channel->setPassword(args);
            channel->sendToAll(Message::rpl_channelmodesis_324(serverName, src.getNickname(), channel->getName(), "Changing the password of the channel (k)"), src.getNickname(), true);
            break ;
        case 'l':
            if (this->isInteger(args))
            {
                channel->setLimit(std::atoi(args.c_str()));
                channel->sendToAll(Message::rpl_channelmodesis_324(serverName, src.getNickname(), channel->getName(), "Changing the connection limit to: " + args + " (l)"), src.getNickname(), true);
            }
            else
                src.sendToFD(Message::err_umodeunknowflag_501(serverName, src.getNickname()));
            break ;
        default:
            break ;
    }
}

bool Command::isInteger(const std::string &str) const
{
    if (str.empty())
        return (false);
    if (str.length() != 1 && str[0] == '0')
        return (false);
    for (std::size_t i = 0; i != str.length(); ++i)
    {
        if (!std::isdigit(str[i]))
            return (false);
    }
    return (true);
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
    else if (type == "WHOIS")
        this->type = WHOIS;
    else if (type == "INVITE")
        this->type = INVITE;
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
