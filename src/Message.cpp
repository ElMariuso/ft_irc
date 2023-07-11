/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 17:35:11 by root              #+#    #+#             */
/*   Updated: 2023/07/03 17:41:40 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/library.hpp"

Message::Message(MessageType type, const std::string& data)
{
    this->type = type;
    this->data = data;
}

Message::~Message() {}

/* Getters */
Message::MessageType Message::getType() const { return (this->type); }
std::string Message::getData() const { return (this->data); }