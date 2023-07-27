/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 21:47:26 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/27 21:22:27 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/library.hpp"

Message::Message(const std::string &name) { this->setName(name); }
Message::~Message() {}

/* Authentification and connection */
std::string Message::connection(const std::string &clientNickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 001 " << clientNickname \
        << " :Login successful!" << "\r\n";
    return (stream.str());
}

std::string Message::welcome(const std::string &clientNickname, const std::string &clientUsername, const std::string &clientHostname, const std::string &date) const
{
    std::ostringstream  stream;
    std::ostringstream  client;

    client << "Welcome to " << clientNickname << "!" << clientUsername << "@" << clientHostname << "!";
    stream << ":" << this->name << " 002 " << clientNickname << " :" << Utils::displayBox(client.str(), 32) << "\r\n" \
        << ":" << this->name << " 002 " << clientNickname << " :Your host is " << this->name \
        << ", running version 0.1" << "\r\n" \
        << ":" << this->name << " 003 " << clientNickname \
        << " :This server was created " << date << "\r\n" \
        << ":" + this->name + " 004 " + clientNickname \
        << " " << this->name << " 0.1 " << "iwosxarqbDG " << "itklo" << "\r\n";
    return (stream.str());
}

/* Commands */
std::string Message::join(const std::string &clientNickname, const std::string &clientUsername, const std::string &clientHostname, const std::string &channelName) const
{
    std::ostringstream  stream;

    stream << ":" << clientNickname << "!" << clientUsername << "@" << clientHostname \
        << " JOIN " << channelName << "\r\n";
    return (stream.str());
}

std::string Message::part(const std::string &clientNickname, const std::string &clientUsername, const std::string &clientHostname, const std::string &channelName, const std::string &message) const
{
    std::ostringstream  stream;

    if (message.empty()) /* If there is no message */
    {
        stream << ":" << clientNickname << "!" << clientUsername << "@" << clientHostname \
            << " PART " << channelName << "\r\n";
    }
    else /* If there is a message */
    {
        stream << ":" << clientNickname << "!" << clientUsername << "@" << clientHostname \
            << " PART " << channelName << " " << message << "\r\n";
    }
    return (stream.str());
}

std::string Message::privmsg(const std::string &srcName, const std::string &destName, const std::string &message) const
{
    std::ostringstream  stream;

    stream << ":" << srcName << " PRIVMSG " \
        << destName << " " << message << "\r\n";
    return (stream.str());
}

std::string Message::mode(const std::string &nickname, const std::string &username, const std::string &hostname, const std::string &channelName, const std::string &modes, const std::string &args) const
{
    std::ostringstream  stream;

    stream << ":" << nickname << "!" << username << "@" << hostname << " MODE " \
        << channelName << " " << modes << " " << args << "\r\n";
    return (stream.str());
}

std::string Message::nick(const std::string &nickname, const std::string &username, const std::string &hostname, const std::string &newNickname) const
{
    std::ostringstream  stream;

    stream << ":" << nickname << "!" << username << "@" << hostname << " NICK " << newNickname << "\r\n";
    return (stream.str());
}

std::string Message::kick(const std::string &srcName, const std::string &destName, const std::string &channelName, const std::string &message) const
{
    std::ostringstream  stream;

    if (message.empty()) /* If there is no message */
    {
       stream << ":" << srcName <<  " KICK " << channelName << " " << destName << "\r\n";
    }
    else /* If there is a message */
    {
        stream << ":" << srcName <<  " KICK " << channelName << " " << destName << " " \
            << message << "\r\n";
    }
    return (stream.str());
}

std::string Message::ping(const std::string &name) const
{
    std::ostringstream  stream;

    stream << "PING :" << name << "\r\n";
    return (stream.str());
}

std::string Message::pong() const
{
    std::ostringstream  stream;

    stream << "PONG " << this->name << "\r\n";
    return (stream.str());
}

/* RPL */
std::string Message::rpl_umodeis_221(const std::string &nickname, const std::string &modes) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 221 " << nickname << " " \
        << modes << "\r\n";
    return (stream.str());
}

std::string Message::rpl_whoisuser_311(const std::string &nickname, const std::string &srcNickname, const std::string &srcUsername, const std::string &srcHostname, const std::string &srcRealname) const
{
    std::ostringstream  stream;
    
    stream << ":" << this->name << " 311 " << nickname << " " << srcNickname \
        << " " << srcUsername << " " << srcHostname << " * :" << srcRealname << "\r\n";
    return (stream.str());
}

std::string Message::rpl_whoisserver_312(const std::string &nickname, const std::string &destNickname, const std::string &serverName, const std::string &serverInfo) const
{
    std::ostringstream  stream;
    
    stream << ":" << this->name << " 312 " << nickname << " " << destNickname \
        << " " << serverName << " :" << serverInfo << "\r\n";
    return (stream.str());
}

std::string Message::rpl_whoisoperator_313(const std::string &nickname, const std::string &destNickname) const
{
    std::ostringstream  stream;
    
    stream << ":" << this->name << " 313 " << nickname << " " << destNickname \
        << " :is an IRC operator" << "\r\n";
    return (stream.str());
}

std::string Message::rpl_whoisidle_317(const std::string &nickname, const std::string &destNickname, int idleTime, int signonTime) const
{
    std::ostringstream  stream;
    
    stream << ":" << this->name << " 317 " << nickname << " " << destNickname \
        << " " << idleTime << " " << signonTime << " :seconds idle, signon time" << "\r\n";
    return (stream.str());
}

std::string Message::rpl_endofwhois_318(const std::string &nickname, const std::string &destNickname) const
{
    std::ostringstream  stream;
    
    stream << ":" << this->name << " 318 " << nickname << " " << destNickname << " :End of /WHOIS list." << "\r\n";
    return (stream.str());
}

std::string Message::rpl_whoischannels_319(const std::string &nickname, const std::string &destNickname, const Server &server) const
{
    std::ostringstream                      stream;
    const std::map<std::string, Channel*>   &channels = server.getChannelsList();
    std::string                             channelListStr;

    for (std::map<std::string, Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it)
    {
        Channel                                 *channel = it->second;
        std::map<int, Client*>::const_iterator  cit = channel->findConnectedByName(destNickname);
        if (cit != channel->getConnectedEnd())
        {
            channelListStr += channel->getName();
            channelListStr += ' ';
        }
    }
    stream << ":" << this->name << " 319 " << nickname << " " << destNickname << " :" << channelListStr << "\r\n";
    return (stream.str());
}

std::string Message::rpl_list_322(const std::string &nickname, const std::string &channelName, const std::string &topic) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 322 " << nickname << " " << channelName \
        << " :" << topic << "\r\n";
    return (stream.str());
}

std::string Message::rpl_listend_323(const std::string &nickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 323 " << nickname << " " \
        << ":End of LIST" << "\r\n";
    return (stream.str());
}

std::string Message::rpl_channelmodesis_324(const std::string &clientNickname, const std::string &channelName, const std::string &modes) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 324 " << clientNickname << " " \
        << channelName << " " << modes << "\r\n";
    return (stream.str());
}

std::string Message::rpl_creationtime_329(const std::string &nickname, const std::string &channelName, const std::string &creationdate) const
{
    std::ostringstream stream;
    
    stream << ":" << this->name << " 329 " << nickname << " " << channelName << " " << creationdate << "\r\n";
    return (stream.str());
}

std::string Message::rpl_notopic_331(const std::string &clientNickname, const std::string &channelName) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 331 " << clientNickname << " " << channelName \
        << " :No topic is set" << "\r\n";
    return (stream.str());
}

std::string Message::rpl_topic_332(const std::string &clientNickname, const std::string &channelName, const std::string &topic) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 332 " << clientNickname << " " << channelName \
        << " :" << topic << "\r\n";
    return (stream.str());
}

std::string Message::rpl_inviting_341(const std::string &srcNickname, const std::string &channelName, const std::string &destNickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 341 " << srcNickname << " " << channelName << " " << destNickname << "\r\n";
    return (stream.str());
}

std::string Message::rpl_namreplay_353(const std::string &clientNickname, const std::string &channelName, const Channel &channel) const
{
    std::ostringstream              stream;
    char                            prefix;
    std::string                     userListStr;

    /* Get all connected users */
    const std::map<int, Client*>    &userList = channel.getConnected();

    if (userList.empty())
        userListStr = "";
    else
    {
        for (std::map<int, Client*>::const_iterator it = userList.begin(); it != userList.end(); ++it)
        {
            const Client &client = *(it->second);
            prefix = (channel.isOp(client)) ? '@' : ' ';
            userListStr += prefix;
            userListStr += client.getNickname();
            userListStr += ' ';
        }
    }
    stream << ":" << this->name << " 353 " << clientNickname << " = " << channelName \
        << " :" << userListStr << "\r\n";
    return (stream.str());
}

std::string Message::rpl_endofnames_366(const std::string &clientNickname, const std::string &channelName) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 366 " << clientNickname << " " << channelName \
        << " :End of NAMES list" << "\r\n";
    return (stream.str());
}

std::string Message::rpl_motd_372(const std::string &nickname, const std::string &message) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 372 " << nickname \
        << " :- " << message << "\r\n";
    return (stream.str());
}

std::string Message::rpl_motdstart_375(const std::string &nickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 375 " << nickname \
        << " :- " << this->name << " Message of the day - " << "\r\n";
    return (stream.str());
}

std::string Message::rpl_endofmotd_376(const std::string &nickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 376 " << nickname \
        << " :End of MOTD command" << "\r\n";
    return (stream.str());
}

/* Errors */
std::string Message::err_nosuchnick_401(const std::string &nickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 401 " << nickname \
        << " :No such nick/channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_nosuchserver_402(const std::string &nickname, const std::string &destName) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 402 " << nickname << " " \
        << destName << " :No such server" << "\r\n";
    return (stream.str());
}

std::string Message::err_nosuchchannel_403(const std::string &clientNickname, const std::string &channelName) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 403 " << clientNickname << " " << channelName \
        << " :No such channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_cannotsendtochan_404(const std::string &nickname) const
{
    std::ostringstream  stream;
    
    stream << ":" << this->name << " 404 " << nickname \
        << " :Cannot send to channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_toomanymatches_416(const std::string &nickname) const
{
    std::ostringstream  stream;
    
    stream << ":" << this->name << " 416 " << nickname \
        << " :Too many matches" << "\r\n";
    return (stream.str());
}

std::string Message::err_nonicknamegiven_431() const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 431 *" \
        << " :No nickname given" << "\r\n";
    return (stream.str());
}

std::string Message::err_erroneusnickname_432(const std::string &nickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 432 * " << nickname \
        << " :Erroneous nickname" << "\r\n";
    return (stream.str());
}

std::string Message::err_nicknameinuse_433(const std::string &nickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 433 * " << nickname \
        << " :Nickname is already in use" << "\r\n";
    return (stream.str());
}

std::string Message::err_usernotinchannel_441(const std::string &clientNickname, const std::string &channelName) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 441 " << clientNickname << " " << channelName \
        << " :They aren't on that channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_notonchannel_442(const std::string &clientNickname, const std::string &channelName) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 442 " << clientNickname << " " << channelName \
        << " :You're not on that channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_useronchannel_443(const std::string &clientNickname, const std::string &channelName) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 443 " << clientNickname << " " << channelName \
        << " :is already on channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_notregistered_451(const std::string &nickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 451 " << nickname << " :You have not registered" << "\r\n";
    return (stream.str());
}

std::string Message::err_needmoreparams_461(const std::string &nickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 461 " << nickname << " :Not enough parameters (user command)" << "\r\n";
    return (stream.str());
}

std::string Message::err_alreadyregistered_462(const std::string &nickname) const
{
    std::ostringstream   stream;

    stream << ":" << this->name << " 462 " << nickname << " :Unauthorized command (already registered)" << "\r\n";
    return (stream.str());
}

std::string Message::err_passwdmismatch_464(const std::string &nickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 464 " << nickname << " :Password incorrect" << "\r\n";
    return (stream.str());
}

std::string Message::err_channelisfull_471(const std::string &clientNickname, const std::string &channelName) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 471 " << clientNickname << " " << channelName \
        << " :Cannot join channel(+l) - Channel user limit reached" << "\r\n";
    return (stream.str());
}

std::string Message::err_inviteonlychan_473(const std::string &clientNickname, const std::string &channelName) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 473 " << clientNickname << " " << channelName \
        << " :Cannot join channel(+i) - Invite only" << "\r\n";
    return (stream.str());
}

std::string Message::err_badchannelkey_475(const std::string &clientNickname, const std::string &channelName) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 475 " << clientNickname << " " << channelName \
        << " :Cannot join channel(+k) - Bad channel key" << "\r\n";
    return (stream.str());
}

std::string Message::err_chanoprivsneeded_482(const std::string &clientNickname, const std::string &channelName) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 482 " << clientNickname << " " << channelName \
        << " :You're not channel operator" << "\r\n";
    return (stream.str()); 
}

std::string Message::err_umodeunknowflag_501(const std::string &nickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 501 " << nickname \
        << " :Unknown MODE flag" << "\r\n";
    return (stream.str());    
}

std::string Message::err_usersdontmatch_502(const std::string &nickname) const
{
    std::ostringstream  stream;

    stream << ":" << this->name << " 502 " << nickname \
        << " :Cannot change mode for other users" << "\r\n";
    return (stream.str());    
}

/* Setters */
void Message::setName(const std::string &name) { this->name = name; }

/* Getters */
std::string Message::getName() const { return (this->name); }