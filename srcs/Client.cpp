#include "Client.hpp"

Client::Client()
    : _fd(-1), _nickname(""), _readBuffer(""), _writeBuffer("")
{
}

Client::Client(int fd)
    : _fd(fd), _nickname(""), _readBuffer(""), _writeBuffer("")
{
}

Client::Client(const Client &other)
{
    *this = other;
}

Client &Client::operator=(const Client &other)
{
    if (this != &other)
    {
        _fd = other._fd;
        _nickname = other._nickname;
        _readBuffer = other._readBuffer;
        _writeBuffer = other._writeBuffer;
    }
    return *this;
}

Client::~Client()
{
}

int Client::getFd() const
{
    return _fd;
}

void Client::setFd(int fd)
{
    _fd = fd;
}

const std::string &Client::getNickname() const
{
    return _nickname;
}

void Client::setNickname(const std::string &nick)
{
    _nickname = nick;
}

std::string &Client::getReadBuffer()
{
    return _readBuffer;
}

void Client::appendReadBuffer(const std::string &data)
{
    _readBuffer += data;
}

void Client::eraseReadBuffer(size_t len)
{
    if (len >= _readBuffer.length())
        _readBuffer.clear();
    else
        _readBuffer.erase(0, len);
}

std::string &Client::getWriteBuffer()
{
    return _writeBuffer;
}

void Client::appendWriteBuffer(const std::string &data)
{
    _writeBuffer += data;
}

void Client::eraseWriteBuffer(size_t len)
{
    if (len >= _writeBuffer.length())
        _writeBuffer.clear();
    else
        _writeBuffer.erase(0, len);
}
