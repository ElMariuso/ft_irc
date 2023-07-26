/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:30:52 by mthiry            #+#    #+#             */
/*   Updated: 2023/07/26 23:51:28 by mthiry           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "library.hpp"

class Client;
class Channel;
class Command;

class Server: public Message
{
    public:
        Server(const std::string &port_str, const std::string &password, const std::string &name);
        ~Server();

        /* Main Process */
        int                                             processServer();
        void                                            setServerPfd();
        void                                            pingSystem(time_t currentTime);
        int                                             pollChecking();
        int                                             checkForNewConnection();
        void                                            browseClients(time_t currentTime);

        /* New connection */
        int                                             acceptNewConnection();
        Client*                                         addNewClient(const int client_socket);

        /* Commands */
        int                                             handleEvent(const int client_socket);
        /**
        * @brief Handles incoming messages from a client. 
        * 
        * The message is first parsed into a command, then it retrieves the associated client and ensures that
        * the argument vector is properly formatted by calling `setArgsCommands`. 
        * The function then handles different cases based on the command type and client's authentication status.
        * 
        * @param message The received message string to be processed.
        * @param client_socket The client socket from which the message was received.
        */
        void                                            getMessages(const std::string &message, const int client_socket);
        /**
        * @brief Helper function to ensure that a command's argument vector has at least 3 elements.
        * If the given command's arguments are less than 3, the missing arguments are filled with empty strings.
        *
        * @param command The command whose arguments are to be checked.
        * @return std::vector<std::string> Vector of at least 3 elements containing command arguments.
        */
        std::vector<std::string>                        setArgsCommands(const Command &command);
        void                                            withoutAuthentification(const Command &command, Client *client, const std::string &arg0);
        void                                            withAuthentification(const Command &command, Client *client, const std::vector<std::string> &args);

        /* Logout */
        void                                            handleDisconnection(const int client_socket, const std::string &message);
        
        /* Utils */
        /**
        * Creates a server socket and binds it to the specified port. 
        *
        * @param port The port to which the server socket should be bound.
        * @return 0 if successful, or one of the following error codes:
        *     -1 if the creation of the server socket options failed
        *     -2 if the server socket creation failed
        *     -3 if binding the server socket to the port failed
        *     -4 if listening on the server socket failed
        */
        int                                             createServerSocket(const int port);
        std::vector<std::string>                        splitCommands(const std::string &message, const char delimiter);

        /* Setters */
        void                                            setServerSocket(int serverSocket);
        void                                            setPassword(std::string password);
        void                                            setFd(struct pollfd fd);
        void                                            setFds(std::list<struct pollfd> fds);
        void                                            setClient(const int &fd, Client *client);
        void                                            setClients(std::map<int, Client*> clients);
        void                                            setChannel(std::string name, Channel *channel);
        void                                            setChannels(std::map<std::string, Channel*> channels);
        void                                            setDate(const std::string &date);

        /* Removers */
	    void                                            removeChannel(Channel *channel);

        /* Getters */
        int                                             getServerSocket() const;
        std::string                                     getPassword() const;
        std::list<struct pollfd>                        getFds() const;
        std::map<int, Client*>                          getClientsList() const;
        std::map<std::string, Channel*>                 getChannelsList() const;
        std::string                                     getDate() const;

        std::map<int, Client*>::const_iterator          getClientsListEnd() const;
        std::map<std::string, Channel*>::const_iterator getChannelsListEnd() const;

        /* Finders */
        Client*                                         findClient(const int &fd) const;
        Channel*                                        findChannel(const std::string &name) const;
        std::map<int, Client*>::const_iterator          findClientByName(const std::string &name) const;

        /* Senders */
        void                                            sendToAll(const std::string &message);

    private:
        int                             serverSocket;
        std::string                     password;
        std::list<struct pollfd>        fds;
        std::map<int, Client*>          clientsList;
        std::map<std::string, Channel*> channelsList;
        std::string                     date;
};

#endif

/* DOCUMENTATION (generated by ChatGPT for the majority) */

/* Commands */

/** @function void getMessages(const std::string &message, const int client_socket);
 * @brief Handles incoming messages from clients, parses and processes commands according to their types and client's authentication status.
 *
 * @param message The received message string from the client.
 * @param client_socket The client socket from which the message was received.
 *
 * This function performs the following steps:
 * 1. It creates a Command object from the received message string.
 * 2. It calls `setArgsCommands` to ensure that the command has at least 3 arguments.
 * 3. It retrieves the client associated with the client_socket from the clientsList.
 * 4. If the client exists and the command type is known, it updates the client's last activity time.
 * 5. If the command is a QUIT command, it handles the disconnection and exits the function.
 * 6. If the client is connected but not authenticated, it processes the command with `withoutAuthentication`.
 * 7. If the client is connected and authenticated, it processes the command with `withAuthentication`.
 * 8. If the client is connected but not authenticated, it sends a "not registered" error message to the client.
 * 9. If the command type is unknown, it logs a "unknown command" debug message.
 * 10. If the client does not exist in the clientsList, it logs a "client not found" error message.
 */

/** @function std::vector<std::string> setArgsCommands(const Command &command);
 * @brief Helper function that fills up a command's arguments to ensure there are at least 3 elements.
 * This function is designed to prepare the arguments for command processing by ensuring there are 
 * at least three elements in the arguments vector. If there are less than three arguments, it fills 
 * the remaining elements with empty strings.
 * 
 * @param command The command whose arguments need to be ensured.
 * 
 * @return A vector of strings which is guaranteed to have at least 3 elements.
 * 
 * This function works as follows:
 * 1. It first gets the arguments from the command using `command.getArgs()`.
 * 2. It checks if the size of the arguments is less than 3.
 * 3. If yes, it pushes back empty strings into the arguments until its size becomes 3.
 * 4. Finally, it returns the modified arguments vector.
 */

/* Utils */
/** @function int getServerSocket() const;
 * @brief Creates a server socket and configures it to listen for incoming client connections.
 * The function performs the following steps in order:
 * 
 * 1. It creates a server socket using the socket() system call.
 * 
 * 2. It sets the server socket option to allow the reuse of the address (SO_REUSEADDR) 
 *    using the setsockopt() system call. This is useful for quickly restarting the server 
 *    if it was previously stopped, as it allows the socket to be bound to an address 
 *    that is in the TIME_WAIT state.
 * 
 * 3. It sets the server socket to be non-blocking using the fcntl() system call. This means 
 *    that the system calls applied to the socket will return immediately instead of waiting 
 *    for the operation to complete.
 * 
 * 4. It binds the server socket to the specified port on the local host using the bind() 
 *    system call. The server can then receive connections from clients on this port.
 * 
 * 5. It puts the server socket in the listening state using the listen() system call. The 
 *    server socket is now ready to accept incoming client connections.
 * 
 * If any of the system calls fail, the function will close the server socket (if it was 
 * successfully created) and return an error code. If the function completes successfully, 
 * it returns 0.
 * 
 * @param port The port number on which the server socket should listen for connections.
 * 
 * @return 0 if the server socket was successfully created and configured, or an error code 
 *         if any of the system calls failed. The error codes are:
 *         - -1 if setting the socket option failed.
 *         - -2 if creating the server socket failed.
 *         - -3 if setting the server socket to be non-blocking failed.
 *         - -4 if binding the server socket failed.
 *         - -5 if putting the server socket in the listening state failed.
 */
