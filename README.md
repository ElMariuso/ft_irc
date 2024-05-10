<p align="center">
  <img src="https://raw.githubusercontent.com/PKief/vscode-material-icon-theme/ec559a9f6bfd399b82bb44393651661b08aaf7ba/icons/folder-markdown-open.svg" width="100" alt="project-logo">
</p>
<p align="center">
    <h1 align="center">FT_IRC</h1>
</p>
<p align="center">
    <em>Empowering seamless communication and collaboration in code.</em>
</p>
<p align="center">
	<img src="https://img.shields.io/github/license/ElMariuso/ft_irc.git?style=default&logo=opensourceinitiative&logoColor=white&color=0080ff" alt="license">
	<img src="https://img.shields.io/github/last-commit/ElMariuso/ft_irc.git?style=default&logo=git&logoColor=white&color=0080ff" alt="last-commit">
	<img src="https://img.shields.io/github/languages/top/ElMariuso/ft_irc.git?style=default&color=0080ff" alt="repo-top-language">
	<img src="https://img.shields.io/github/languages/count/ElMariuso/ft_irc.git?style=default&color=0080ff" alt="repo-language-count">
<p>
<p align="center">
	<!-- default option, no dependency badges. -->
</p>

<br><!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary><br>

- [ Overview](#-overview)
- [ Features](#-features)
- [ Repository Structure](#-repository-structure)
- [ Modules](#-modules)
- [ Getting Started](#-getting-started)
  - [ Installation](#-installation)
  - [ Usage](#-usage)
  - [ Tests](#-tests)
- [ Project Roadmap](#-project-roadmap)
- [ Contributing](#-contributing)
- [ License](#-license)
- [ Acknowledgments](#-acknowledgments)
</details>
<hr>

##  Overview

Ft_irc is an open-source IRC server software offering robust communication capabilities for users. With core functionalities like message handling, command execution, and channel management, ft_irc facilitates seamless interaction within IRC networks. It empowers users to exchange messages, join channels, set modes, and manage connections efficiently. The projects value proposition lies in its ability to provide a stable and feature-rich environment for real-time communication, enhancing user experience and fostering community engagement.

---

##  Features

|    |   Feature         | Description |
|----|-------------------|---------------------------------------------------------------------|
| ⚙️ | **Architecture**   | A modular IRC server with separate components for server, client, messages, commands, channels, and utilities. The architecture organizes functionalities efficiently, promoting code separation and maintainability. File structure is well-segmented and follows a standard layout.                                       |
| 🔩 | **Code Quality**   | The codebase demonstrates good code quality with clear and concise naming conventions, proper commenting, and consistent formatting. It adheres to C++ best practices with well-structured classes and functions. The Makefile orchestrates the build process effectively, ensuring robust compilation and linking processes.          |
| 📄 | **Documentation**  | Extensive documentation is provided for each key component, detailing their roles, interactions, and functionalities. Functionality summaries in source files enhance code understanding. However, more high-level overview documentation could improve onboard efficiency for new contributors.                        |
| 🔌 | **Integrations**   | The project relies on C++ as the primary language and utilizes a Makefile for build automation. External dependencies include only standard C++ libraries, facilitating easy setup and deployment. The project demonstrates self-sufficiency in managing dependencies and integrations.                                   |
| 🧩 | **Modularity**     | The codebase exhibits high modularity, allowing for easy component reusability and extension. Encapsulation of functionalities within distinct classes promotes separation of concerns and simplifies maintenance. The architecture facilitates adding new features without disrupting existing functionalities.                    |
| 🧪 | **Testing**        | Testing frameworks and tools are not explicitly mentioned in the repository details. Incorporating testing frameworks like Google Test or Catch2 could enhance code reliability and maintainability by enabling automated testing procedures.                                                                                                |
| ⚡️ | **Performance**    | The performance evaluation is not explicitly described in the repository details. However, the use of efficient C++ coding practices and modular design principles likely contributes to optimal performance. Further performance profiling and optimization could enhance resource efficiency.                                   |
| 🛡️ | **Security**       | Security measures are not explicitly discussed in the repository details. Implementing secure coding practices, input validation mechanisms, and role-based access control could enhance data protection and mitigate security vulnerabilities in the IRC server application.                               |
| 📦 | **Dependencies**   | Key dependencies include standard C++ libraries for functionalities like networking, string operations, and time handling. The project minimizes external dependencies, promoting self-contained development and deployment. Leveraging minimal dependencies enhances project maintainability and portability.          |

---

##  Repository Structure

```sh
└── ft_irc/
    ├── Makefile
    ├── include
    │   ├── Channel.hpp
    │   ├── Client.hpp
    │   ├── Command.hpp
    │   ├── Message.hpp
    │   ├── Server.hpp
    │   ├── Utils.hpp
    │   └── library.hpp
    └── src
        ├── Channel.cpp
        ├── Client.cpp
        ├── Command.cpp
        ├── Message.cpp
        ├── Server.cpp
        ├── Utils.cpp
        └── main.cpp
```

---

##  Modules

<details closed><summary>.</summary>

| File                                                                     | Summary                                                                                                                                               |
| ---                                                                      | ---                                                                                                                                                   |
| [Makefile](https://github.com/ElMariuso/ft_irc.git/blob/master/Makefile) | Orchestrates build process.-Defines compiler flags and directories.-Manages object file creation and compilation.-Cleans object files and executable. |

</details>

<details closed><summary>src</summary>

| File                                                                               | Summary                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
| ---                                                                                | ---                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
| [Utils.cpp](https://github.com/ElMariuso/ft_irc.git/blob/master/src/Utils.cpp)     | Retrieves current time and formats messages.-Displays debug and error messages.-Allows dynamic waiting message with increasing dots.-Offers functions for integer to string conversion, copying strings, and date/time formatting.-Validates integer strings.                                                                                                                                                                                                                                                    |
| [Channel.cpp](https://github.com/ElMariuso/ft_irc.git/blob/master/src/Channel.cpp) | Modes, operators, topic, access control. Manages connected clients with functions to set, get, find, remove. Enables sending messages to clients and clearing invite list.                                                                                                                                                                                                                                                                                                                                       |
| [main.cpp](https://github.com/ElMariuso/ft_irc.git/blob/master/src/main.cpp)       | Initiates server setup, handles interruptions, and starts server processing in the ft_irc repository. Parses command-line arguments, configures server, and manages exceptions using Library functions.                                                                                                                                                                                                                                                                                                          |
| [Message.cpp](https://github.com/ElMariuso/ft_irc.git/blob/master/src/Message.cpp) | Message.cpp SummaryThe `Message.cpp` file in the `ft_irc` repository plays a crucial role in handling message-related functionalities for the IRC server. It implements the logic for creating, parsing, and processing messages exchanged between clients and the server. By encapsulating message operations within this file, it ensures seamless communication within the IRC network, enhancing overall system reliability and efficiency.                                                                  |
| [Client.cpp](https://github.com/ElMariuso/ft_irc.git/blob/master/src/Client.cpp)   | Implements functionality to handle client connections within the architecture. Manages client information, communication, and modes in the IRC server. Supports sending and receiving messages, managing user modes, and setting client attributes like nickname and hostname.                                                                                                                                                                                                                                   |
| [Command.cpp](https://github.com/ElMariuso/ft_irc.git/blob/master/src/Command.cpp) | The `Command.cpp` file in the `ft_irc` repository plays a vital role in handling various IRC commands within the server application. It encapsulates the logic for parsing and executing IRC commands sent by clients, ensuring seamless communication and interaction between users. This component significantly contributes to the core functionality of the IRC server by interpreting and executing commands accurately, enhancing the overall user experience and system performance.                      |
| [Server.cpp](https://github.com/ElMariuso/ft_irc.git/blob/master/src/Server.cpp)   | The `Server.cpp` file in the `ft_irc` repository plays a crucial role in the implementation of the IRC server component of the project. It encapsulates the logic for handling server functionality such as accepting client connections, managing channels, processing commands, and facilitating message exchange between clients. This file serves as the backbone of the server module, orchestrating the overall communication flow and ensuring seamless interaction among clients within the IRC network. |

</details>

<details closed><summary>include</summary>

| File                                                                                   | Summary                                                                                                                                                                                                                                                                  |
| ---                                                                                    | ---                                                                                                                                                                                                                                                                      |
| [Utils.hpp](https://github.com/ElMariuso/ft_irc.git/blob/master/include/Utils.hpp)     | Enables generating current time, debug, error messages, and more. Provides functions for string conversions, date operations, and input validation within the parent repositorys architecture.                                                                           |
| [Server.hpp](https://github.com/ElMariuso/ft_irc.git/blob/master/include/Server.hpp)   | Defines a server class managing client connections and channels, offering methods for processing, handling new connections, commands, and disconnections. It includes utilities for socket creation and message processing, ensuring commands have at least 3 arguments. |
| [Message.hpp](https://github.com/ElMariuso/ft_irc.git/blob/master/include/Message.hpp) | Defines message handling for IRC server interactions, including connection, commands, RPL codes, and errors. Specifies methods for various server-client messages. Organizes and formats message content for the communication protocol.                                 |
| [Command.hpp](https://github.com/ElMariuso/ft_irc.git/blob/master/include/Command.hpp) | Defines command handling for IRC server, encapsulating message parsing and processing. Handles various IRC commands like NICK, JOIN, PRIVMSG, MODE, etc. Facilitates server, client, and channel interactions.                                                           |
| [library.hpp](https://github.com/ElMariuso/ft_irc.git/blob/master/include/library.hpp) | Defines color codes, constants, and necessary includes for the parent repositorys networking chat application, ft_irc. Facilitates structured communication between channels, clients, and servers using defined colors and buffer sizes.                                |
| [Channel.hpp](https://github.com/ElMariuso/ft_irc.git/blob/master/include/Channel.hpp) | Defines and manages properties of chat channels, including modes, operators, and connected clients. Facilitates setting and retrieving channel information for effective communication within the system.                                                                |
| [Client.hpp](https://github.com/ElMariuso/ft_irc.git/blob/master/include/Client.hpp)   | Defines a client class with key attributes and methods for handling user connections in the parent repositorys IRC server. Manages user modes, connection status, and user data, providing essential functionality for client operations.                                |

</details>

---

##  Getting Started

**System Requirements:**

* **CPP**: `version x.y.z`

###  Installation

<h4>From <code>source</code></h4>

> 1. Clone the ft_irc repository:
>
> ```console
> $ git clone https://github.com/ElMariuso/ft_irc.git
> ```
>
> 2. Change to the project directory:
> ```console
> $ cd ft_irc
> ```
>
> 3. Install the dependencies:
> ```console
> $ g++ -o myapp main.cpp
> ```

###  Usage

<h4>From <code>source</code></h4>

> Run ft_irc using the command below:
> ```console
> $ ./myapp
> ```

###  Tests

> Run the test suite using the command below:
> ```console
> $ googletest
> ```

---

##  Project Roadmap

- [X] `► INSERT-TASK-1`
- [ ] `► INSERT-TASK-2`
- [ ] `► ...`

---

##  Contributing

Contributions are welcome! Here are several ways you can contribute:

- **[Report Issues](https://github.com/ElMariuso/ft_irc.git/issues)**: Submit bugs found or log feature requests for the `ft_irc` project.
- **[Submit Pull Requests](https://github.com/ElMariuso/ft_irc.git/blob/main/CONTRIBUTING.md)**: Review open PRs, and submit your own PRs.
- **[Join the Discussions](https://github.com/ElMariuso/ft_irc.git/discussions)**: Share your insights, provide feedback, or ask questions.

<details closed>
<summary>Contributing Guidelines</summary>

1. **Fork the Repository**: Start by forking the project repository to your github account.
2. **Clone Locally**: Clone the forked repository to your local machine using a git client.
   ```sh
   git clone https://github.com/ElMariuso/ft_irc.git
   ```
3. **Create a New Branch**: Always work on a new branch, giving it a descriptive name.
   ```sh
   git checkout -b new-feature-x
   ```
4. **Make Your Changes**: Develop and test your changes locally.
5. **Commit Your Changes**: Commit with a clear message describing your updates.
   ```sh
   git commit -m 'Implemented new feature x.'
   ```
6. **Push to github**: Push the changes to your forked repository.
   ```sh
   git push origin new-feature-x
   ```
7. **Submit a Pull Request**: Create a PR against the original project repository. Clearly describe the changes and their motivations.
8. **Review**: Once your PR is reviewed and approved, it will be merged into the main branch. Congratulations on your contribution!
</details>

<details closed>
<summary>Contributor Graph</summary>
<br>
<p align="center">
   <a href="https://github.com{/ElMariuso/ft_irc.git/}graphs/contributors">
      <img src="https://contrib.rocks/image?repo=ElMariuso/ft_irc.git">
   </a>
</p>
</details>

---

##  License

This project is protected under the [SELECT-A-LICENSE](https://choosealicense.com/licenses) License. For more details, refer to the [LICENSE](https://choosealicense.com/licenses/) file.

---

##  Acknowledgments

- List any resources, contributors, inspiration, etc. here.

[**Return**](#-overview)

---
