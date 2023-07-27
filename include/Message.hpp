/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 21:46:41 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/27 14:00:00 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP
# include "library.hpp"

class Channel;
class Message;
class Server;

class Message
{
    public:
        Message(const std::string &serverName);
        ~Message();

        /* Authentification and connection */
        std::string connection(const std::string &clientNickname) const;
        std::string welcome(const std::string &clientNickname, const std::string &clientUsername, const std::string &clientHostname, const std::string &date) const;

        /* Commands */
        std::string join(const std::string &clientNickname, const std::string &clientUsername, const std::string &clientHostname, const std::string &channelName) const;
        std::string part(const std::string &clientNickname, const std::string &clientUsername, const std::string &clientHostname, const std::string &channelName, const std::string &message) const;
        std::string privmsg(const std::string &srcName, const std::string &destName, const std::string &message) const;
        std::string mode(const std::string &nickname, const std::string &username, const std::string &hostname, const std::string &channelName, const std::string &modes, const std::string &args) const;
        std::string nick(const std::string &nickname, const std::string &username, const std::string &hostname, const std::string &newNickname) const;
        std::string kick(const std::string &srcName, const std::string &destName, const std::string &channelName, const std::string &message) const;
        std::string ping(const std::string &name) const;
        std::string pong() const;

        /* RPL */
        std::string rpl_umodeis_221(const std::string &nickname, const std::string &modes) const;
        std::string rpl_list_322(const std::string &nickname, const std::string &channelName, const std::string &topic) const;
        std::string rpl_listend_323(const std::string &nickname) const;
        std::string rpl_channelmodesis_324(const std::string &clientNickname, const std::string &channelName, const std::string &modes) const;
        std::string rpl_creationtime_329(const std::string &nickname, const std::string &channelName, const std::string &creationdate) const;
        std::string rpl_notopic_331(const std::string &clientNickname, const std::string &channelName) const;
        std::string rpl_topic_332(const std::string &clientNickname, const std::string &channelName, const std::string &topic) const;
        std::string rpl_inviting_341(const std::string &srcNickname, const std::string &channelName, const std::string &destNickname) const;
        std::string rpl_namreplay_353(const std::string &clientNickname, const std::string &channelName, const Channel &channel) const;
        std::string rpl_endofnames_366(const std::string &clientNickname, const std::string &channelName) const;
        std::string rpl_motd_372(const std::string &nickname, const std::string &message) const;
        std::string rpl_motdstart_375(const std::string &nickname) const;
        std::string rpl_endofmotd_376(const std::string &nickname) const;

        /* Errors */
        std::string err_nosuchnick_401(const std::string &nickname) const;
        std::string err_nosuchserver_402(const std::string &nickname, const std::string &destName) const;
        std::string err_nosuchchannel_403(const std::string &clientNickname, const std::string &channelName) const;
        std::string err_cannotsendtochan_404(const std::string &nickname) const;
        std::string err_toomanymatches_416(const std::string &nickname) const;
        std::string err_nonicknamegiven_431() const;
        std::string err_erroneusnickname_432(const std::string &nickname) const;
        std::string err_nicknameinuse_433(const std::string &nickname) const;
        std::string err_usernotinchannel_441(const std::string &clientNickname, const std::string &channelName) const;
        std::string err_notonchannel_442(const std::string &clientNickname, const std::string &channelName) const;
        std::string err_useronchannel_443(const std::string &clientNickname, const std::string &channelName) const;
        std::string err_notregistered_451(const std::string &nickname) const;
        std::string err_needmoreparams_461(const std::string &nickname) const;
        std::string err_alreadyregistered_462(const std::string &nickname) const;
        std::string err_passwdmismatch_464(const std::string &nickname) const;
        std::string err_channelisfull_471(const std::string &clientNickname, const std::string &channelName) const;
        std::string err_inviteonlychan_473(const std::string &clientNickname, const std::string &channelName) const;
        std::string err_badchannelkey_475(const std::string &clientNickname, const std::string &channelName) const;
        std::string err_chanoprivsneeded_482(const std::string &clientNickname, const std::string &channelName) const;
        std::string err_umodeunknowflag_501(const std::string &nickname) const;
        std::string err_usersdontmatch_502(const std::string &nickname) const;

        /* Setters */
        void        setName(const std::string &name);

        /* Getters */
        std::string getName() const;

    protected:
        std::string name;
};

#endif