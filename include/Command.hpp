/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 15:32:51 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/12 15:38:39 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
# define COMMAND_HPP
# include "library.hpp"

enum MessageType
{
    UNKNOW,
    MSGPRIV,
    NICK
};

class Command
{
    public:
        Command(const std::string &message);
        ~Command();

        /* Setters */
        void        setMessage(const std::string &message);
        void        setType();

        /* Getters */
        std::string getMessage();
        MessageType getType();
        
    private:
        std::string message;
        MessageType type;
};

#endif