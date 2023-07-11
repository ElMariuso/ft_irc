/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 16:55:45 by root              #+#    #+#             */
/*   Updated: 2023/07/03 17:51:41 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP
# include "library.hpp"

class Message
{
    public:
        enum MessageType
        {
            CONNECT,
            DISCONNECT,
            REQUEST,
            RESPONSE,
            UPDATE,
            CHAT
        };
        Message(MessageType type, const std::string& data);
        ~Message();

        /* Getters */
        MessageType getType() const;
        std::string getData() const;

    private:
        MessageType type;
        std::string data;
};

#endif