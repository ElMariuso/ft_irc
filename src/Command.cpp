/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:31 by mthiry            #+#    #+#             */
/*   Updated: 2023/08/02 02:49:21 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Command.hpp"

Command::Command() {}

Command::Command(const std::string &message)
{
    this->setMessage(message);
    this->setType();
    if (this->args.empty())
        this->setArgs();
    else
        this->args.pop_back();
}
Command::~Command() {}

/* Commands */
void Command::nick(const Server &server, Client *client, const std::string &name) const
{
    /* Used for messages */
    const std::string   &nickname = client->getNickname();
    const std::string   &username = client->getUsername();
    const std::string   &hostname = client->getHostname();

    if (name.empty()) /* ERR_NONICKNAMEGIVEN (431) */
        client->sendToFD(server.err_nonicknamegiven_431());
    else if (this->isNotRightNickname(server.getName(), name)) /* ERR_ERRONEUSNICKNAME (432) */
        client->sendToFD(server.err_erroneusnickname_432(name));
    else if (server.findClientByName(name) != server.getClientsListEnd()) /* ERR_NICKNAMEINUSE (433) */
        client->sendToFD(server.err_nicknameinuse_433(name));
    else /* NICK */
    {
        const std::map<std::string, Channel*>   &channels = server.getChannelsList();
        for (std::map<std::string, Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it)
        {
            Channel *channel = it->second;
            if (channel->findConnected(client->getFd()))
                channel->sendToAll(server.nick(nickname, username, hostname, name), nickname, false);
        }
        client->sendToFD(server.nick(nickname, username, hostname, name));
        client->setNickname(name);
    }
}

void Command::join(Server *server, Client *client, const std::string &name, const std::string &password) const
{
    Channel                                 *channel;
    /* Used for messages */
    const std::string                       &clientName = client->getNickname();
    const std::string                       &clientUser = client->getUsername();
    const std::string                       &clientHost = client->getHostname();

    if (name.empty())
    {
        client->sendToFD(server->err_needmoreparams_461(clientName));
        return ;
    }
    channel = server->findChannel(name);
    if (channel != NULL) /* JOIN with channel */
    {
        /* Get some lists */
        const std::map<int, Client*>        &connected = channel->getConnected();
        const std::map<std::string, bool>   &invited = channel->getInvited();
        const std::string                   &passwordChannel = channel->getPassword();

        if (channel->hasMode('l') && connected.size() >= channel->getLimit()) /* ERR_CHANNELISFULL (471) */
            client->sendToFD(server->err_channelisfull_471(clientName, name));
        else if (channel->hasMode('i') && invited.find(clientName) == invited.end()) /* ERR_INVITEONLYCHAN (473) */
            client->sendToFD(server->err_inviteonlychan_473(clientName, name));
        else if (channel->hasMode('k') && password != passwordChannel) /* ERR_BADCHANNELKEY (475) */
            client->sendToFD(server->err_badchannelkey_475(clientName, name));
        else if (channel->findConnectedByName(clientName) != channel->getConnectedEnd()) /* ERR_USERONCHANNEL (443) */
            client->sendToFD(server->err_useronchannel_443(clientName, name));
        else /* JOIN */
        {
            /* Connect the new user */
            channel->setConnected(client);
            
            /* Send JOIN notification to all */
            channel->sendToAll(server->join(clientName, clientUser, clientHost, name), clientName, true);

            /* Check for a topic */
            /* Send a message for the new user */
            std::ostringstream  stream;
            
            stream << server->rpl_namreplay_353(clientName, name, *channel) << server->rpl_endofnames_366(clientName, name);
            if (!channel->hasTopic()) /* RPL_NOTOPIC (331) */
                stream << server->rpl_notopic_331(clientName, name);
            else /* RPL_TOPIC (332) */
                stream << server->rpl_topic_332(clientName, name, channel->getTopic()) << server->rpl_creationtime_329(clientName, name, channel->getDate());
            client->sendToFD(stream.str());
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
        std::ostringstream  stream;

        stream << server->join(clientName, clientUser, clientHost, name) << server->rpl_notopic_331(clientName, name) \
            << server->rpl_namreplay_353(clientName, name, *channel) << server->rpl_endofnames_366(clientName, name) \
            << server->rpl_creationtime_329(clientName, name, channel->getDate());

        /* Send confirmation message */
        client->sendToFD(stream.str());
    }
}

void Command::part(Server *server, const Client &client, Channel *channel, const std::string &name, const std::string &message, bool isQuit) const
{
    /* Used for messages */
    const std::string               &clientName = client.getNickname();
    const std::string               &clientUser = client.getUsername();
    const std::string               &clientHost = client.getHostname();
    
    if (!channel) /* ERR_NOSUCHCHANNEL (403) */
        client.sendToFD(server->err_nosuchchannel_403(clientName, name));
    else if (channel && (channel->findConnectedByName(clientName) == channel->getConnectedEnd())) /* ERR_NOTONCHANNEL (442) */
        client.sendToFD(server->err_notonchannel_442(clientName, name));
    else if (channel) /* PART */
    {
        /* Send to all users and Remove the user from the connected list */
        if (isQuit)
            channel->sendToAll(server->part(clientName, clientUser, clientHost, name, message), clientName, false);
        else
            channel->sendToAll(server->part(clientName, clientUser, clientHost, name, message), clientName, true);
        channel->removeConnected(client.getFd());
        if (channel->getConnected().empty()) /* Delete the channel if there is no user left */
            server->removeChannel(channel);
    }
}

void Command::privmsg(const Server &server, const Client &src, const std::string &destName, const std::string &message) const
{
    Channel             *channel;
    Client              *client;
    /* Used for messages */
    const std::string   &srcName = src.getNickname();
    
    if (destName.empty() || message.empty())
        src.sendToFD(server.err_needmoreparams_461(srcName));
    else if (destName[0] == '#')
    {
        /* Check if the channel exists */
        channel = server.findChannel(destName);
        if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
            src.sendToFD(server.err_nosuchchannel_403(srcName, destName));
        else if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
            src.sendToFD(server.err_notonchannel_442(srcName, destName));
        else /* PRIVMSG */
            channel->sendToAll(server.privmsg(srcName, destName, message), srcName, false);
    }
    else
    {
        std::map<int, Client*>::const_iterator  it = server.findClientByName(destName);
        
        /* Check if the user exists */
        if (it == server.getClientsListEnd()) /* ERR_NOSUCHNICK (401) */
            src.sendToFD(server.err_nosuchnick_401(srcName, destName));
        else
        {
            client = it->second;
            client->sendToFD(server.privmsg(srcName, destName, message));
        }
    }
}

void Command::mode(const Server &server, Client *src, const std::string &destName, const std::string &modes, const std::string &args) const
{
    /* Used for messages */
    const std::string   &srcName = src->getNickname();

    if (destName.empty())
        src->sendToFD(server.err_needmoreparams_461(srcName));
    else if (modes.empty()) /* Check modes */
        this->modeCheck(srcName, destName, *src, server);
    else /* Add modes */
        this->modeAdd(srcName, destName, src, server, modes, args);
}

void Command::modeCheck(const std::string &srcName, const std::string &destName, const Client &client, const Server &server) const
{
    if (destName[0] == '#') /* Channel */
    {
        Channel *channel;

        /* Check if the channel exists */
        channel = server.findChannel(destName);
        if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
        {
            client.sendToFD(server.err_nosuchchannel_403(srcName, destName));
            return ;
        }

        /* Get modes list */
        const std::string               &modes = channel->getModesList();

        if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
            client.sendToFD(server.err_notonchannel_442(srcName, destName));
        else if (modes.empty()) /* Send there is no modes */
            client.sendToFD(server.rpl_channelmodesis_324(srcName, destName, ""));
        else /* Send mode list */
            client.sendToFD(server.rpl_channelmodesis_324(srcName, destName, "+" + modes));
    }
    else /* Client */
    {
        /* Get modes list */
        const std::string               &modes = client.getModesList();

        if (modes.empty()) /* Send there is no modes */
            client.sendToFD(server.rpl_umodeis_221(srcName, ""));
        else /* Send mode list */
            client.sendToFD(server.rpl_umodeis_221(srcName, modes));
    }
}

void Command::modeAdd(const std::string &srcName, const std::string &destName, Client *src, const Server &server, const std::string &modes, const std::string &args) const
{
    if (destName[0] == '#') /* Channel */
    {
        Channel *channel;

        /* Check if the channel exists */
        channel = server.findChannel(destName);
        if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
        {
            src->sendToFD(server.err_nosuchchannel_403(srcName, destName));
            return ;
        }

        /* Check if modes exists */
        for (std::size_t i = 1; i < modes.length(); ++i) /* ERR_UMODEUNKNOWNFLAG (501) */
        {
            if (!channel->isMode(modes[i]))
            {
                src->sendToFD(server.err_umodeunknowflag_501(srcName));
                return ;
            }
        }

        if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
            src->sendToFD(server.err_notonchannel_442(srcName, destName));
        else if (!channel->isOp(*src)) /* ERR_CHANOPRIVSNEEDED (482) */
            src->sendToFD(server.err_chanoprivsneeded_482(srcName, destName));
        else /* MODES */
            this->setModes(server, srcName, destName, channel, *src, modes, args);
    }
    else /* Client */
    {
        const std::string   &defaultNickname = src->getDefaultNickname();
        
        if (srcName != destName && defaultNickname != destName) /* ERR_USERSDONTMATCH (502) */
        {
            src->sendToFD(server.err_usersdontmatch_502(srcName));
            return ;
        }

        /* Check if modes exists */
        for (std::size_t i = 0; i < modes.length(); ++i) /* ERR_UMODEUNKNOWNFLAG (501) */
        {
            if (!src->isMode(modes[i]) && modes[i] != '+')
            {
                src->sendToFD(server.err_umodeunknowflag_501(srcName));
                return ;
            }
        }

        /* Change modes of the user */
        this->setModesClient(server, srcName, src, modes);
    }
}

void Command::topic(const Server &server, const Client &src, const std::string &destName, const std::string &topic) const
{
    Channel                         *channel;

    /* Used for messages */
    const std::string               &srcName = src.getNickname();
    
    if (destName.empty())
    {
        src.sendToFD(server.err_needmoreparams_461(srcName));
        return ;
    }
    if (destName[0] != '#') /* ERR_NOSUCHCHANNEL (403) */
    {
        src.sendToFD(server.err_nosuchchannel_403(srcName, destName));
        return ;
    }

    /* Check if the channel exists */
    channel = server.findChannel(destName);
    if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
    {
        src.sendToFD(server.err_nosuchchannel_403(srcName, destName));
        return ;
    }

    /* Used for messages */
    const std::string               &channelName = channel->getName();

    if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
        src.sendToFD(server.err_notonchannel_442(srcName, destName));
    else /* TOPIC */
    {
        if (topic.empty()) /* Send the topic */
        {
            if (channel->hasTopic()) /* Send the topic */
                src.sendToFD(server.rpl_topic_332(srcName, channelName, topic));
            else /* There is no topic */
                src.sendToFD(server.rpl_notopic_331(srcName, channelName));
        }
        else /* Change the topic */
        {
            if (channel->hasMode('t') && !channel->isOp(src)) /* ERR_CHANOPRIVSNEEDED (482) */
                src.sendToFD(server.err_chanoprivsneeded_482(srcName, destName));
            else /* Change the topic */
            {
                std::string newTopic = topic.substr(1);

                channel->setTopic(newTopic);
                src.sendToFD(server.rpl_topic_332(srcName, channelName, newTopic));
            }
        }
    }
}

void Command::list(const Server &server, const Client &src, const std::string &destName) const
{
    const std::map<std::string, Channel*>   &map = server.getChannelsList();
    /* Used for messages */
    const std::string                       &srcName = src.getNickname();
    
    if (destName.empty()) /* All channels*/
    {
        if (map.size() > 100) /* ERR_TOMANYMATCHES (416) */
            src.sendToFD(server.err_toomanymatches_416(srcName));
        else /* LIST */
        {
            for (std::map<std::string, Channel*>::const_iterator it = map.begin(); it != map.end(); ++it)
                src.sendToFD(server.rpl_list_322(srcName, it->second->getName(), it->second->getTopic()));
            src.sendToFD(server.rpl_listend_323(srcName));
        }
    }
    else /* Specific informations */
    {
        Channel *channel = server.findChannel(destName);
        
        if (!channel) /* ERR_NOSUCHCHANNEL (403) */
            src.sendToFD(server.err_nosuchchannel_403(srcName, destName));
        else
            src.sendToFD(server.rpl_list_322(srcName, channel->getName(), channel->getTopic()));
    }
}

void Command::names(const Server &server, const Client &src, const std::vector<std::string> &args) const
{
    Channel *channel;
    
    /* Used for messages */
    const std::string                       &srcName = src.getNickname();

    for (std::size_t i = 0; i != args.size(); ++i)
    {
        if (args[i].empty())
            continue ;
        channel = server.findChannel(args[i]);
        if (!channel) /* ERR_NOSUCHCHANNEL (403) */
        {
            src.sendToFD(server.err_nosuchchannel_403(srcName, args[i]));
            continue ;
        }

        const std::map<int, Client*>    &connected = channel->getConnected();

        if (connected.size() > 100) /* ERR_TOMANYMATCHES (416) */
            src.sendToFD(server.err_toomanymatches_416(srcName));
        else /* NAMES */
            src.sendToFD(server.rpl_namreplay_353(srcName, args[i], *channel) + server.rpl_endofnames_366(srcName, args[i]));
    }
}

void Command::kick(const Server &server, const Client &src, Client *dest, const std::string &message, Channel *channel) const
{
    /* Used for messages */
    const std::string               &srcName = src.getNickname();
    const std::string               &destName = dest->getNickname();
    const std::string               &channelName = channel->getName();

    /* Check if the channel and the user exists */
    if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
    {
        src.sendToFD(server.err_nosuchchannel_403(srcName, channelName));
        return ;
    }
    else if (dest == NULL) /* ERR_NOSUCHNICK (401) */
    {
        src.sendToFD(server.err_nosuchnick_401(srcName, destName));
        return ;
    }

    if (channel->findConnectedByName(destName) == channel->getConnectedEnd()) /* ERR_USERNOTINCHANNEL (441) */
        src.sendToFD(server.err_usernotinchannel_441(destName, channelName));
    else if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
        src.sendToFD(server.err_notonchannel_442(srcName, channelName));
    else if (!channel->isOp(src)) /* ERR_CHANOPRIVSNEEDED (482) */
        src.sendToFD(server.err_chanoprivsneeded_482(srcName, channelName));
    else /* KICK */
    {
        /* Send to all users */
        channel->sendToAll(server.kick(srcName, destName, channelName, message), srcName, true);

        /* Remove the user from the connected list */
        channel->removeConnected(dest->getFd());
    }
}

void Command::whois(const Server &server, const Client &src, const std::string &destName) const
{
    std::map<int, Client*>::const_iterator  it = server.findClientByName(destName);

    /* Used for messages */
    const std::string   &srcNickname = src.getNickname();
    
    if (destName.empty())
        src.sendToFD(server.err_needmoreparams_461(srcNickname));
    else if (it == server.getClientsListEnd()) /* ERR_NOSUCHNICK (401) */
        src.sendToFD(server.err_nosuchnick_401(srcNickname, destName));
    else
    {
        /* Used for messages */
        std::ostringstream  stream;
        const std::string   &destNickname = it->second->getNickname();
        const std::string   &destUsername = it->second->getUsername();
        const std::string   &destHostname = it->second->getHostname();
        const std::string   &destRealname = it->second->getRealname();

        stream << server.rpl_whoisuser_311(srcNickname, destNickname, destUsername, destHostname, destRealname) \
            << server.rpl_whoisserver_312(srcNickname, destNickname, server.getName(), "Informations") \
            << server.rpl_whoischannels_319(srcNickname, destNickname, server) \
            << server.rpl_whoisidle_317(srcNickname, destNickname, 0, 0) \
            << server.rpl_endofwhois_318(srcNickname, destName);
        src.sendToFD(stream.str());
    }
}

void Command::invite(const Server &server, const Client &src, const std::string &destName, const std::string &channelName) const
{
    if (destName.empty() || channelName.empty())
    {
        src.sendToFD(server.err_needmoreparams_461(src.getNickname()));
        return ;
    }

    Client                                  *dest;
    Channel                                 *channel = server.findChannel(channelName);
    const std::string                       &srcName = src.getNickname();
    std::map<int, Client*>::const_iterator  itClient = server.findClientByName(destName);
    
    /* Check if the channel and the user exists */
    if (channel == NULL) /* ERR_NOSUCHCHANNEL (403) */
    {
        src.sendToFD(server.err_nosuchchannel_403(srcName, channelName));
        return ;
    }
    else if (itClient == server.getClientsListEnd()) /* ERR_NOSUCHNICK (401) */
    {
        src.sendToFD(server.err_nosuchnick_401(srcName, destName));
        return ;
    }
    dest = itClient->second;

    /* Send error or response if both channel and user exists */
    if (channel->findConnectedByName(srcName) == channel->getConnectedEnd()) /* ERR_NOTONCHANNEL (442) */
        src.sendToFD(server.err_notonchannel_442(srcName, channelName));
    else if (channel->findConnectedByName(destName) != channel->getConnectedEnd()) /* ERR_USERONCHANNEL (443) */
        src.sendToFD(server.err_useronchannel_443(destName, channelName));
    else if (channel->hasMode('i') && !channel->isOp(src)) /* ERR_CHANOPRIVSNEEDED (482) */
        src.sendToFD(server.err_chanoprivsneeded_482(srcName, channelName));
    else /* INVITE */
    {
        dest->sendToFD(server.rpl_inviting_341(srcName, channelName, destName));
        channel->sendToAll(server.rpl_inviting_341(srcName, channelName, destName), srcName, true);

        /* Increase invited list */
        if (channel->hasMode('i'))
            channel->setInvited(destName);   
    }
}

void Command::user(const Server &server, Client *client) const
{
    std::string realname;
    std::string bitmask;
    int         bitlen;
    
    if (this->args.size() < 4)
    {
        client->sendToFD(server.err_needmoreparams_461(client->getNickname()));
        return ;
    }
    if (client->getIsRegistered())
    {
        client->sendToFD(server.err_alreadyregistered_462(client->getNickname()));
        return ;
    }
    client->setIsRegistered(true);
    client->setUsername(this->args.at(0));
    bitmask = this->args.at(1);
    bitlen = bitmask.size();
    if (bitmask[bitlen - 2] == 1)
        client->addMode('w');
    if (bitmask[bitlen - 3] == 1)
        client->addMode('i');
    for (std::size_t i = 3; i < this->args.size(); i++)
        realname = realname + this->args.at(i);
    if (realname[0] == ':')
        realname.erase(0, 1);
    client->setRealname(realname);
}

/* Nick Utils */
bool Command::isNotRightNickname(const std::string &serverName, const std::string &newNickname) const { return (newNickname == serverName || newNickname.substr(0, 8) == "ft_Guest"); }

/* MODE Utils */
void Command::setModes(const Server &server, const std::string &srcName, const std::string &channelName, Channel *channel, const Client &src, const std::string &modes, const std::string &args) const
{
    std::string         modes2 = "";

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
            switch (modes2[i])
            {
                case 'i':
                    channel->clearInvited();
                    break ;
                case 't':
                    break ;
                case 'k':
                    channel->setPassword("");
                    break ;
                case 'l':
                    channel->setLimit(0);
                    break ;
                default:
                    break ;
            }
        }
        channel->sendToAll(server.rpl_channelmodesis_324(srcName, channelName, "Removing: " + modes2), srcName, true);
    }
    else /* Adding */
    {
        if (args.empty() || modes2.length() > 1)
        {
            /* Setting modes */
            for (std::size_t i = 0; i < modes2.length(); ++i)
                channel->addMode(modes2[i]);
            channel->sendToAll(server.rpl_channelmodesis_324(srcName, channelName, "Adding: " + modes2), srcName, true);
        }
        else /* Change restrictions */
            this->changeRestriction(server, channel, src, modes2[0], args);
    }
}

void Command::setModesClient(const Server &server, const std::string &srcName, Client *src, const std::string &modes) const
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
        src->sendToFD(server.rpl_umodeis_221(srcName, modes));
    }
    else /* Adding */
    {
        /* Setting modes */
        for (std::size_t i = 1; i < modes2.length(); ++i)
            src->addMode(modes2[i]);
        src->sendToFD(server.rpl_umodeis_221(srcName, modes));
    }
}

void Command::changeRestriction(const Server &server, Channel *channel, const Client &src, const char &mode, const std::string &args) const
{
    Client              *client;
    const std::string   &srcName = src.getNickname();
    const std::string   &srcUsername = src.getUsername();
    const std::string   &srcHostname = src.getHostname();
    
    switch (mode)
    {
        case 'k':
        {
            channel->setPassword(args);
            channel->sendToAll(server.rpl_channelmodesis_324(srcName, channel->getName(), "Changing the password of the channel (k)"), srcName, true);
            break ;
        }
        case 'l':
        {
            if (this->isInteger(args))
            {
                channel->setLimit(std::atoi(args.c_str()));
                channel->sendToAll(server.rpl_channelmodesis_324(srcName, channel->getName(), "Changing the connection limit to: " + args + " (l)"), srcName, true);
            }
            else
                src.sendToFD(server.err_umodeunknowflag_501(srcName));
            break ;
        }
        case 'o':
        {
            std::map<int, Client*>::const_iterator  itServer = server.findClientByName(args);

            if (itServer == server.getClientsListEnd())
                client = NULL;
            else
                client = itServer->second;
            if (client == NULL)
                src.sendToFD(server.err_nosuchnick_401(srcName, args));
            else if (channel->findConnectedByName(args) == channel->getConnectedEnd()) /* ERR_USERNOTINCHANNEL (441) */
                src.sendToFD(server.err_usernotinchannel_441(args, channel->getName()));
            else
            {
                if (channel->addOp(*client) == 0)
                    channel->sendToAll(server.mode(srcName, srcUsername, srcHostname, channel->getName(), "+o " + channel->getModesList(), args), srcName, true);
                else
                {
                    channel->rmOp(*client);
                    channel->sendToAll(server.mode(srcName, srcUsername, srcHostname, channel->getName(), "-o " + channel->getModesList(), args), srcName, true);
                }
            }
            break ;
        }
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
    size_t      pos;

    type = this->message.substr(0, this->message.find(' '));
    pos = this->message.find(' ');

    if (pos == std::string::npos)
        this->args.push_back("no");
    else
        this->message.erase(0, pos + 1);

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
    else if (type == "motd\r")
        this->type = MOTD;
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
    if (this->message[0] == '\r')
    {
        this->args.push_back("");
        return ;
    }
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
            if (member[member.size() - 1] == '\r' || member[member.size() - 1] == '\n')
            {
                member = member.substr(0, member.length() - 1);
                this->args.back() = member;
            }
        }
    }
}

/* Getters */
std::string Command::getMessage() const { return (this->message); }
MessageType Command::getType() const { return (this->type); }
std::vector<std::string> Command::getArgs() const { return (this->args); }
