/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 21:47:26 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/23 01:30:29 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/library.hpp"

/* Authentification and connection */
std::string Message::connection(const std::string &serverName, const std::string &clientNickname)
{
    std::ostringstream  stream;

    stream << ":" << serverName << " 001 " << clientNickname \
        << " :Please authenticate to get the rest of the server" << "\r\n";
    return (stream.str());
}

std::string Message::welcome(const std::string &serverName, const std::string &clientNickname, const std::string &clientUsername, const std::string &clientHostname, const std::string &date)
{
    std::ostringstream  stream;
    std::ostringstream  client;

    client << "Welcome to " << clientNickname << "!" << clientUsername << "@" << clientHostname << "!";
    stream << ":" << serverName << " 001 " << clientNickname << " :" << Utils::displayBox(client.str(), 32) << "\r\n" \
        << ":" << serverName << " 002 " << clientNickname << " :Your host is " << serverName \
        << ", running version 0.1" << "\r\n" \
        << ":" << serverName << " 003 " << clientNickname \
        << " :This server was created " << date << "\r\n" \
        << ":" + serverName + " 004 " + clientNickname \
        << " " << serverName << " 0.1 " << "iwosxarqbDG " << "itklo" << "\r\n";
    return (stream.str());
}

/* Commands */
std::string Message::join(const std::string &clientNickname, const std::string &clientUsername, const std::string &clientHostname, const std::string &channelName)
{
    std::stringstream   stream;

    stream << ":" << clientNickname << "!" << clientUsername << "@" << clientHostname \
        << " JOIN " << channelName << "\r\n";
    return (stream.str());
}

std::string Message::part(const std::string &clientNickname, const std::string &clientUsername, const std::string &clientHostname, const std::string &channelName, const std::string &message)
{
    std::stringstream   stream;

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

std::string Message::privmsg(const std::string &srcName, const std::string &destName, const std::string &message)
{
    std::stringstream   stream;

    stream << ":" << srcName << " PRIVMSG " \
        << destName << " " << message << "\r\n";
    return (stream.str());
}

std::string Message::nick(const std::string &serverName, const std::string &newNickname)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 001 " << newNickname \
        << " :You're now known as " << newNickname << "\r\n";
    return (stream.str());
}

std::string Message::kick(const std::string &srcName, const std::string &destName, const std::string &channelName, const std::string &message)
{
    std::stringstream   stream;

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

/* RPL */
std::string Message::rpl_umodeis_221(const std::string &serverName, const std::string &nickname, const std::string &modes)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 221 " << nickname << " " \
        << modes << "\r\n";
    return (stream.str());
}

std::string Message::rpl_channelmodesis_324(const std::string &serverName, const std::string &clientNickname, const std::string &channelName, const std::string &modes)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 324 " << clientNickname << " " \
        << channelName << " " << modes << "\r\n";
    return (stream.str());
}

std::string Message::rpl_notopic_331(const std::string &serverName, const std::string &clientNickname, const std::string &channelName)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 331 " << clientNickname << " " << channelName \
        << " :No topic is set" << "\r\n";
    return (stream.str());
}

std::string Message::rpl_topic_332(const std::string &serverName, const std::string &clientNickname, const std::string &channelName, const std::string &topic)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 332 " << clientNickname << " " << channelName \
        << " :" << topic << "\r\n";
    return (stream.str());
}

std::string Message::rpl_inviting_341(const std::string &serverName, const std::string &srcNickname, const std::string &channelName, const std::string &destNickname)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 341 " << srcNickname << " " << channelName << " " << destNickname << "\r\n";
    return (stream.str());
}

std::string Message::rpl_namreplay_353(const std::string &serverName, const std::string &clientNickname, const std::string &channelName, const Channel &channel)
{
    std::stringstream               stream;
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
            prefix = (channel.isOp(client)) ? '@' : '+';
            userListStr += prefix;
            userListStr += client.getNickname();
            userListStr += ' ';
        }
    }
    stream << ":" << serverName << " 353 " << clientNickname << " = " << channelName \
        << " :" << userListStr << "\r\n";
    return (stream.str());
}

std::string Message::rpl_endofnames_366(const std::string &serverName, const std::string &clientNickname, const std::string &channelName)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 366 " << clientNickname << " " << channelName \
        << " :End of NAMES list" << "\r\n";
    return (stream.str());
}

/* Errors */
std::string Message::err_nosuchnick_401(const std::string &serverName, const std::string &nickname)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 401 " << nickname \
        << " :No such nick/channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_nosuchchannel_403(const std::string &serverName, const std::string &clientNickname, const std::string &channelName)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 403 " << clientNickname << " " << channelName \
        << " :No such channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_cannotsendtochan_404(const std::string &serverName, const std::string &nickname)
{
    std::stringstream   stream;
    
    stream << ":" << serverName << " 404 " << nickname \
        << " :Cannot send to channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_nonicknamegiven_431(const std::string &serverName)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 431 *" \
        << " :No nickname given" << "\r\n";
    return (stream.str());
}

std::string Message::err_erroneusnickname_432(const std::string &serverName, const std::string &nickname)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 432 * " << nickname \
        << " :Erroneous nickname" << "\r\n";
    return (stream.str());
}

std::string Message::err_nicknameinuse_433(const std::string &serverName, const std::string &nickname)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 433 * " << nickname \
        << " :Nickname is already in use" << "\r\n";
    return (stream.str());
}

std::string Message::err_usernotinchannel_441(const std::string &serverName, const std::string &clientNickname, const std::string &channelName)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 441 " << clientNickname << " " << channelName \
        << " :They aren't on that channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_notonchannel_442(const std::string &serverName, const std::string &clientNickname, const std::string &channelName)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 442 " << clientNickname << " " << channelName \
        << " :You're not on that channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_useronchannel_443(const std::string &serverName, const std::string &clientNickname, const std::string &channelName)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 443 " << clientNickname << " " << channelName \
        << " :is already on channel" << "\r\n";
    return (stream.str());
}

std::string Message::err_notregistered_451(const std::string &serverName, const std::string &nickname)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 451 " << nickname << " :You have not registered" << "\r\n";
    return (stream.str());
}

std::string Message::err_alreadyregistered_462(const std::string &serverName, const std::string &nickname)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 462 " << nickname << " :Unauthorized command (already registered)" << "\r\n";
    return (stream.str());
}

std::string Message::err_passwdmismatch_464(const std::string &serverName, const std::string &nickname)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 464 " << nickname << " :Password incorrect" << "\r\n";
    return (stream.str());
}

std::string Message::err_channelisfull_471(const std::string &serverName, const std::string &clientNickname, const std::string &channelName)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 471 " << clientNickname << " " << channelName \
        << " :Cannot join channel(+l) - Channel user limit reached" << "\r\n";
    return (stream.str());
}

std::string Message::err_inviteonlychan_473(const std::string &serverName, const std::string &clientNickname, const std::string &channelName)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 473 " << clientNickname << " " << channelName \
        << " :Cannot join channel(+i) - Invite only" << "\r\n";
    return (stream.str());
}

std::string Message::err_badchannelkey_475(const std::string &serverName, const std::string &clientNickname, const std::string &channelName)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 475 " << clientNickname << " " << channelName \
        << " :Cannot join channel(+k) - Bad channel key" << "\r\n";
    return (stream.str());
}

std::string Message::err_chanoprivsneeded_482(const std::string &serverName, const std::string &clientNickname, const std::string &channelName)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 482 " << clientNickname << " " << channelName \
        << " :You're not channel operator" << "\r\n";
    return (stream.str()); 
}

std::string Message::err_umodeunknowflag_501(const std::string &serverName, const std::string &nickname)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 501 " << nickname \
        << " :Unknown MODE flag" << "\r\n";
    return (stream.str());    
}

std::string Message::err_usersdontmatch_502(const std::string &serverName, const std::string &nickname)
{
    std::stringstream   stream;

    stream << ":" << serverName << " 502 " << nickname \
        << " :Cannot change mode for other users" << "\r\n";
    return (stream.str());    
}