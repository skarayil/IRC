#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <iostream>

class Client
{
    private:
        int         _fd;
        std::string _nickname;
        std::string _readBuffer;
        std::string _writeBuffer;

    public:
        Client();
        Client(int fd);
        Client(const Client &other);
        Client &operator=(const Client &other);
        ~Client();


        int                 getFd() const;
        void                setFd(int fd);

        const std::string   &getNickname() const;
        void                setNickname(const std::string &nick);

        std::string         &getReadBuffer();
        void                appendReadBuffer(const std::string &data);
        void                eraseReadBuffer(size_t len);

        std::string         &getWriteBuffer();
        void                appendWriteBuffer(const std::string &data);
        void                eraseWriteBuffer(size_t len);
};

#endif