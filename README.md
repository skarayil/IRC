*This project has been created as part of the 42 curriculum by skarayil, hasivaci.*

# ft_irc — Internet Relay Chat Server

A custom, robust IRC (Internet Relay Chat) server implemented in C++98 compliant with [RFC 1459](https://tools.ietf.org/html/rfc1459) and [RFC 2812](https://tools.ietf.org/html/rfc2812). The server handles multiple concurrent client connections using non-blocking I/O and a single `poll()` event loop without multithreading or external networking libraries.

---

## Description

**ft_irc** is a 42 school curriculum project that replicates a real-world IRC server. IRC is an established application-layer protocol designed for text-based, real-time communication across distributed networks.

The server allows clients to:
- Authenticate with a server password (`PASS`).
- Register a unique nickname (`NICK`) and username (`USER`).
- Join public or private chat rooms (`JOIN`, `PART`).
- Exchange public channel messages and direct private messages (`PRIVMSG`).
- Manage channel operator permissions and enforce channel modes (`MODE`, `KICK`, `INVITE`, `TOPIC`).
- Handle graceful disconnections and signal shutdowns (`QUIT`, `SIGINT`).

---

## Architecture & Design

- **Language Standard:** Strict C++98 (`-Wall -Wextra -Werror -std=c++98`).
- **I/O Multiplexing:** Single-threaded asynchronous event loop driven by `poll()`.
- **Non-blocking Sockets:** All sockets (listening socket and accepted client descriptors) are set to non-blocking mode via `fcntl(fd, F_SETFL, O_NONBLOCK)`.
- **Buffer & Packet Assembly:** Incoming TCP streams are buffered per-client to reliably handle packet fragmentation (e.g., partial commands or `\r\n` split across network packets) and protected against buffer flooding attacks (> 4096 bytes without delimiter).
- **Clean State Management:** Robust cleanup on disconnections; prevents dangling channel members, broken operator assignments, and memory leaks.

---

## Features & Supported Commands

### Client Commands
| Command | Description | Example |
| :--- | :--- | :--- |
| `PASS` | Authenticates connection against server password | `PASS secret123` |
| `NICK` | Sets or changes user nickname | `NICK alice` |
| `USER` | Sets username, hostname, servername, and realname | `USER alice 0 * :Alice Smith` |
| `JOIN` | Joins or creates a channel (with optional key) | `JOIN #general` or `JOIN #secret key123` |
| `PART` | Leaves one or more channels | `PART #general :Leaving now` |
| `PRIVMSG` | Sends a message to a channel or direct to a user | `PRIVMSG bob :Hello!` |
| `KICK` | Removes a user from a channel (Operator only) | `KICK #general bob :Spamming` |
| `INVITE`| Invites a user to an invite-only channel (Operator only) | `INVITE charlie #general` |
| `TOPIC` | Views or updates channel topic (Operator restricted if `+t`) | `TOPIC #general :Welcome!` |
| `MODE`  | Inspects or modifies channel modes | `MODE #general +it` |
| `QUIT`  | Disconnects cleanly from the server | `QUIT :Goodbye!` |
| `PING` / `PONG` | Verifies client-server liveness | `PING 127.0.0.1` |
| `WHO` / `WHOIS` | Queries information about users on the server | `WHOIS alice` |

### Channel Modes
Channel operators can control access and rules using `MODE <channel> <flags> [params]`:
- `+i` / `-i` : Set / remove **Invite-Only** channel.
- `+t` / `-t` : Set / remove **Topic Restriction** (only operators can alter the topic).
- `+k` / `-k` : Set / remove **Channel Key** (password protected channel).
- `+o` / `-o` : Give / revoke **Channel Operator** status to a user.
- `+l` / `-l` : Set / remove **User Limit** (caps maximum concurrent members).

---

## Instructions

### Compilation

Build the project using the provided Makefile:
```bash
make
```
This generates the executable `./ircserv`. Other available Makefile targets:
- `make clean`: Removes intermediate object files.
- `make fclean`: Removes object files and the `ircserv` binary.
- `make re`: Recompiles the entire project from scratch.

### Execution

Launch the server by specifying a listening port and a network password:
```bash
./ircserv <port> <password>
```

Example:
```bash
./ircserv 6667 mysecretpassword
```

### Connecting Clients

#### 1. Using Netcat (`nc`)
```bash
nc -C 127.0.0.1 6667
```
Once connected, send the registration commands:
```text
PASS mysecretpassword
NICK alice
USER alice 0 * :Alice Wonderland
JOIN #general
PRIVMSG #general :Hello everyone!
```

#### 2. Using Irssi
```bash
irssi
/connect 127.0.0.1 6667 mysecretpassword alice
/join #general
```

#### 3. Using HexChat
1. Open HexChat -> **Network List** -> **Add**.
2. Name the network (e.g., `ft_irc_local`).
3. Click **Edit...** and set the server address to:
   ```text
   127.0.0.1/6667
   ```
4. In the **Password** field, enter your server password (`mysecretpassword`).
5. Uncheck **Use SSL for all servers on this network**.
6. Set your desired **Nick name** and **User name**.
7. Connect and join channels (`/join #general`).

---

## Bonus Feature: Joke Bot

The server includes an integrated interactive and automated **Joke Bot** (`JokerBot`):
- **Interactive Triggers:** Any user in a channel can send `!espri` or `!joke` via `PRIVMSG`:
  ```text
  PRIVMSG #channel :!espri
  PRIVMSG #channel :!joke
  ```
  The bot immediately replies with a randomized programming or geek joke.
- **Automated Broadcast:** Every 5 minutes (300 seconds), the bot automatically broadcasts a curated joke to all currently active channels on the server.
- **Clean Architecture:** Designed with C++98 enums and a centralized repository function without hardcoded magic numbers or global memory leaks.

---

## Resources

### References
- [RFC 1459 — Internet Relay Chat Protocol](https://tools.ietf.org/html/rfc1459)
- [RFC 2812 — Internet Relay Chat: Client Protocol](https://tools.ietf.org/html/rfc2812)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- Linux Programmer's Manual: `poll(2)`, `fcntl(2)`, `socket(2)`, `bind(2)`, `listen(2)`, `accept(2)`

### AI Usage

AI tools were used for understanding project requirements, researching protocol resources, and assisting with error handling. All implementation, code, and testing were conducted and verified by the developers.